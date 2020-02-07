#include "MapWorld.h"
#include "../../Utilities/Random.h"

#include <unordered_map>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
// #include <memory>

// Map info
std::shared_ptr<ParameterLink<std::string>> MapWorld::inputFilesPL =
  Parameters::register_parameter("WORLD_MAP-inputFiles", (std::string) "World/MapWorld/maps/geo.txt, World/MapWorld/maps/geo2.txt",
                                 "number of times   the organism can move");

// evaluation info
std::shared_ptr<ParameterLink<int>> MapWorld::movesMultiplierPL =
  Parameters::register_parameter("WORLD_MAP-movesMultiplier", 2,
                                 "number of times the organism can move");

// organism info for inputs and outputs
std::shared_ptr<ParameterLink<int>> MapWorld::sensorInputRangePL =
  Parameters::register_parameter("WORLD_MAP-sensorInputRange", 1,
                                 "range around organism for sensors (0-2)");

std::shared_ptr<ParameterLink<int>> MapWorld::sensorOutputRangePL =
  Parameters::register_parameter("WORLD_MAP-sensorOutputRange", 1,
                                 "range around organism for sensors (0-2)");

std::shared_ptr<ParameterLink<int>> MapWorld::outputCompassSensorPL =
  Parameters::register_parameter("WORLD_MAP-outputCompassSensor", 0,
                                 "range around organism for sensors (1 or 0)");

std::shared_ptr<ParameterLink<std::string>> MapWorld::turnOffSensorsPL =
  Parameters::register_parameter("WORLD_MAP-turnOffSensors", (std::string) "-1,1:1,1",
                                 "output coordinates to turn off");

// obstacle info
std::shared_ptr<ParameterLink<std::string>> MapWorld::obstaclesPL =
  Parameters::register_parameter("WORLD_MAP-obstacles", (std::string) "3,0.5",
                                 "number of obstacles (int), velocity of obstacle (double)");


// evaluation info
std::shared_ptr<ParameterLink<double>> MapWorld::rewardForSensorPL =
  Parameters::register_parameter("WORLD_MAP-rewardForSensor", 1.0,
                                 "reward for getting sensor correct");

std::shared_ptr<ParameterLink<double>> MapWorld::rewardForDirectionPL =
  Parameters::register_parameter("WORLD_MAP-rewardForDirection", 0.0,
                                 "reward for getting sensor correct");
                                 
std::shared_ptr<ParameterLink<int>> MapWorld::rewardForGoalPL =
  Parameters::register_parameter("WORLD_MAP-rewardForGoal", 2,
                                 "reward for getting to destination");

std::shared_ptr<ParameterLink<double>> MapWorld::hitWallDeductionPL =
  Parameters::register_parameter("WORLD_MAP-hitWallDeduction", 1.0,
                                 "reward for getting to destination");

// mandatory info
std::shared_ptr<ParameterLink<std::string>> MapWorld::groupNamePL =
    Parameters::register_parameter("WORLD_NBACK_NAMES-groupNameSpace", (std::string) "root::",
                                   "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> MapWorld::brainNamePL =
    Parameters::register_parameter("WORLD_NBACK_NAMES-brainNameSpace", (std::string) "root::",
                                   "namespace for parameters used to define brain");


/**************************************************************************************
*
*                           MAIN FUNCTIONS
*
***************************************************************************************/


/**
* gets character at position from geo
*
* @param: coordinate pair of geo position
* @return: character on geo
**/
MapWorld::MapWorld(std::shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_)
{
  // create global variables from registered Parameters
  sensorOutputRange = MapWorld::sensorOutputRangePL->get(PT);
  sensorInputRange = MapWorld::sensorInputRangePL->get(PT);
  outputCompassSensor = MapWorld::outputCompassSensorPL->get(PT);

  movesMultiplier = MapWorld::movesMultiplierPL->get(PT);
  rewardForSensor = MapWorld::rewardForSensorPL->get(PT);
  rewardForDirection = MapWorld::rewardForDirectionPL->get(PT);
  rewardForGoal = MapWorld::rewardForGoalPL->get(PT);
  hitWallDeduction = MapWorld::hitWallDeductionPL->get(PT);

  // getting sensors to turn off
  std::vector<std::string> stringSensors;
  convertCSVListToVector(MapWorld::turnOffSensorsPL->get(PT), stringSensors, ':'); // get input file names to parse

  for (auto sensor: stringSensors)
  {
    int comma = sensor.find(",");
    int x = std::stoi(sensor.substr(0,comma));
    int y = std::stoi(sensor.substr(comma+1));

    removeOutputSensors.push_back(std::make_pair(x,y));
  }


  // Getting geomaps
  std::vector<std::string> listFiles;
  convertCSVListToVector(MapWorld::inputFilesPL->get(PT), listFiles, ','); // get input file names to parse

  if (debugPL->get())
  {
    // printing which files are being parsed
    for (int i=0; i < listFiles.size(); i++)
    {
      std::cout << "files " << listFiles[i] << std::endl;
    }
  }

  // generates geos and adds to m_list
  for (int i=0; i < listFiles.size(); i++)
  {
      cGeo newGeo = cGeo(listFiles[i]);
      mMapList.push_back(newGeo);
  }

  // Creating obstacles
  std::vector<std::string> stringObstacles;
  convertCSVListToVector(MapWorld::obstaclesPL->get(PT), stringObstacles, ':'); // get input file names to parse

  for (auto command: stringObstacles)
  {
    int comma = command.find(",");
    int num = std::stoi(command.substr(0,comma));
    double speed = std::stod(command.substr(comma+1));

    for (int obstacle = 0; obstacle < num; obstacle++ )
    {
      cObstacle newObstacle = cObstacle(serialNumber, speed);
      mObstacleList.push_back(newObstacle);      
      serialNumber++;

    }
  }


  // columns to be added to ave file
  popFileColumns.push_back("score"); // score correct answers to grid sensors
  popFileColumns.push_back("score_sensor"); // score correct answers to grid sensors
  popFileColumns.push_back("score_direction"); // score correct answers to direction of object
  popFileColumns.push_back("score_dist"); // score for moving in correct direction
  popFileColumns.push_back("score_goal"); // score for got to destination
  popFileColumns.push_back("score_hit"); // score for moving in correct direction
  popFileColumns.push_back("score_moves"); // score for moving in correct direction
}


/**
* gets character at position from geo
*
* @param: coordinate pair of geo position
* @return: character on geo
**/
void MapWorld::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                        int analyze, int visualize, int debug)
{

  // Initialize information for map with random destination coord
  for (int i = 0; i < mMapList.size(); i++)
  {
    mMapList[i].resetMap();
  }

  int popSize = groups[groupNamePL->get(PT)]->population.size();

  for (int i = 0; i < popSize; i++) 
  {
    evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze,
                 visualize, debug);
  }
}


/**
* gets character at position from geo
*
* @param: coordinate pair of geo position
* @return: character on geo
**/
void MapWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug)
{
  // debug = true;

  auto brain = org->brains[brainNamePL->get(PT)];

  // initialize score and organism
  cCar car = cCar(serialNumber, sensorInputRange, sensorOutputRange, removeOutputSensors);
  serialNumber++;

  double totalSensor = 0;
  double totalCompass = 0; // direction
  double totalDist = 0; // mantahhan distance from point
  double totalScore = 0;
  double totalHit = 0;
  double totalMoves = 0;
  double goal = 0;

  double roundCounter = 0;
  double evalCounter = 0;
  double aveStartDist = 0;


  for (int i = 0; i < mMapList.size(); i++)  // Each map
  {
    // setting organism map
    std::shared_ptr<cGeo> currGeo = std::make_shared<cGeo> (mMapList[i]); 
    std::vector< std::pair<int, int> > listStartPos = currGeo->getStartPositions();

    int startDist = currGeo->getStartDist(); 
    aveStartDist += startDist;
    int movesPerOrganism = startDist*movesMultiplier;


    // adding wall and destination to visualizer
    if (visualize)
    {
      std::string wall_string = "wall\n" + currGeo->walls();
      FileManager::writeToFile("MapWorldData.csv", wall_string);

      std::string xDest = std::to_string(currGeo->getDestPos().first);
      std::string yDest = std::to_string(currGeo->getDestPos().second);
    
      std::string destination_string = "destination\n" + xDest + "," + yDest;
      FileManager::writeToFile("MapWorldData.csv", destination_string);
    }

    for (int pos = 0; pos < listStartPos.size(); pos++)  // Every other starting position
    {
      for (int r = 0; r < 4; r++) // for each facing position
      { 
        brain->resetBrain();// resetting the brain
        evalCounter += 1;

        // setting organism facing direction
        car.setFacing(90*r);

        // setting organism position
        car.setPos(listStartPos[pos]); 

        int moves = 1; // organism move count per evaluation 
        bool done = 0; 
        bool hit = 0;
     
      
        /**************             MOVES             **************/
        while((moves <= movesPerOrganism) &&   !done)
        {
          roundCounter += 1;
          car.configureCompass(currGeo);
     
          // adding coordinates to visualizer organism position (x,y,radians rotation)
          if (visualize)
          {
            FileManager::writeToFile("MapWorldData.csv", car.getPosString());
          }

          if (debug)
          {
            std::cout << currGeo->stringGeo(car.getPos(), car.getFacing(), mObstacleList) << std::endl;
          }
      

          // INPUT to brain 0-: grid sensors
          int counter_input = 0; // input node number
          for(auto grid_sensor : car.getSensors(currGeo)){
            brain->setInput(counter_input, grid_sensor); // input to brain
            counter_input += 1;
          }
      
      
          // INPUT to brain 8-11: mCompass sensors
          for(auto mCompass_sensor : car.getCompass()){
            brain->setInput(counter_input, mCompass_sensor);
            counter_input += 1;
          }
      
      
          //** update brain after inputs **//
          brain->update();
      
      

          int output_counter = 0;
          for(auto grid_sensor : car.getSensorsOutput(currGeo)){
            if (Bit(brain->readOutput(output_counter)) == grid_sensor)
            {
              totalSensor += 1;

              if (debug)
                std::cout << "Sensor Correct" << std::endl;
            }      
            output_counter += 1;
          }
      
          // OUTPUT: which direction
          if (outputCompassSensor)
          {
            for(auto check_side : car.getCompass()){
              if (Bit(brain->readOutput(output_counter)) == check_side)
              {
                totalCompass += 1;

                if (debug)
                  std::cout << "Compass Correct" << std::endl;
              }
              output_counter += 1;
            }
          }
      
      
          // OUTPUT: turn right 90 degrees
          if (Bit(brain->readOutput(output_counter)) == 1){
            car.turn("right");
          }
          output_counter += 1;
      
      
          // OUTPUT: turn left 90 degrees
          if (Bit(brain->readOutput(output_counter)) == 1){
            car.turn("left");
          }
          output_counter += 1;
      
      
          // OUTPUT: move forward
          if (Bit(brain->readOutput(output_counter)) == 1){
            int moved = car.move(currGeo, mObstacleList);
            if (!moved)
            {
              hit = 1;
              if (debug)
                std::cout << "DEDUCTION: Hit wall" << std::endl;
            }
          }
          
          moves += 1;

          //** scoring valid moves **//
          std::pair<int, int> dPos = currGeo->getDestPos();
          std::pair<int, int> cPos = car.getPos();
      
          if (cPos == dPos) // if at destination
          {
            done = 1;
            // car.setPos(listStartPos[pos]);
          }

        } // end move
        totalMoves += (movesPerOrganism-moves);

        if (hit)
          totalHit -= 1*hitWallDeduction;

        if (done)
        {
          goal += 1; 
          if (visualize)
          {
            FileManager::writeToFile("MapWorldData.csv", "\ngoal");
          } 
        } 

        else
        {
          int currDist = std::stoi(currGeo->getCoordString(car.getPos()));
          totalDist += (startDist-currDist)/startDist;

          totalScore += ((startDist-currDist)/startDist)/evalCounter;
        }
        
      
      } // end facing loop
      

    } // end starting pos 
      



  } // end map loop

  totalSensor /= (roundCounter*3);
  totalSensor *= rewardForSensor;

  totalCompass /= (roundCounter*4);
  totalCompass *= rewardForDirection;

  
  // aveStartDist /=  mMapList.size();
  // double evalDist = (aveStartDist*(aveStartDist-1))/2;

  // totalDist /= (evalDist*evalCounter);

  // goal /= evalCounter;
  // goal *= rewardForGoal;

  totalMoves /= 12;

  if (debug){
    std::cout << totalScore << std::endl;
    std::cout << totalSensor << std::endl;
    std::cout << totalCompass << std::endl;
    std::cout << totalDist << std::endl;
    std::cout << goal << std::endl;
    std::cout << totalMoves << std::endl;
  }


  // scoring
  totalScore += (rewardForGoal*goal) + totalSensor + totalCompass + totalHit + totalMoves;

  org->dataMap.append("score", totalScore);
  org->dataMap.append("score_sensor", totalSensor);
  org->dataMap.append("score_direction", totalCompass);
  org->dataMap.append("score_dist", totalDist);
  org->dataMap.append("score_goal", goal);
  org->dataMap.append("score_hit", totalHit);
  org->dataMap.append("score_moves", totalMoves);
  

}


std::unordered_map<std::string, std::unordered_set<std::string>> MapWorld::requiredGroups()
{
  /**
  * gets character at position from geo
  *
  * @param: coordinate pair of geo position
  * @return: character on geo
  **/

  int sensorInputs = 4; // start with compass
  // add sensorRange for how many sensors they're given
  if (sensorInputRange > 0)
    sensorInputs += 5;

  if (sensorInputRange > 1)
    sensorInputs += 9;

  std::string inputs = std::to_string(sensorInputs);


  int sensorOutputs = 3; // 3 outputs for turn right, left, and move
  // optiontional grade sensor ouputs 
  if (sensorOutputRange > 0)
    sensorOutputs += 5;

  if (sensorOutputRange > 1)
    sensorOutputs += 9;

  sensorOutputs -= removeOutputSensors.size();

  // optional grade compass outputs
  if (outputCompassSensor)
    sensorOutputs += 4;

  std::string outputs = std::to_string(sensorOutputs);


  return {{groupNamePL->get(PT), {"B:" + brainNamePL->get(PT) +
          "," + inputs + "," + outputs}}}; // setting number of outputs
  // requires a root group and a brain (in root namespace) and no addtional
  // genome,
  // the brain must have 1 input, and the variable list_nbacks outputs
}





  /**************************************************************************************
  *
  *                                     GEO FUNCTIONS
  *
  ***************************************************************************************/

  /*************************  CONSTRUCTOR FUNCTIONS ************************************/

  /**
  * Constructor
  * string:file file to read and create geo from
  *
  * @param: file to create geo
  **/
  cGeo::cGeo(std::string file)
  {
    // initialize variables
    std::string line;
    std::ifstream myfile(file);

    // parsing file
    if (myfile.is_open()){
      while (std::getline(myfile, line)){
        std::vector<std::string> parsed_line;

        // process line to vector of char
        convertCSVListToVector(line, parsed_line, ','); // get list of geo files

        // initialize mGeoGrid
        mGeoGrid.push_back(parsed_line);
      }
    }

    // document all walls from grid
    for(int i=0; i< mGeoGrid.size()-1; i++){
      for(int k=0; k<mGeoGrid[i].size(); k++){
          if(mGeoGrid[i][k] == "w"){
            mWallList += std::to_string(k) + "," + std::to_string(i) + "\n";
            mWallNum += 1;
          }
      }
    }

    
  }

/************************  HELPER FUNCTIONS ************************/
  /**
  * Pick random open spot
  *
  * @return: coordinate pair
  **/
  void cGeo::resetMap()
  {
    clear();
    mDestination = pickRandomCoord();
    manhattanDistance();
  }

  std::pair<int,int> cGeo::pickRandomCoord()
  {
    std::string open = "w";
    int x = Random::getInt(0, mGeoGrid[0].size()-1);
    int y = Random::getInt(0, mGeoGrid.size()-1);

    std::pair<int,int> randomPos = std::make_pair(x,y);

    // if at wall, repick coordinate
    while (getCoordString(randomPos) == "w"){
      x = Random::getInt(0, mGeoGrid[0].size()-1);
      y = Random::getInt(0, mGeoGrid.size()-1);

      randomPos = std::make_pair(x, y);
    }

    return randomPos;
  }

  /**
  * Manhattan distance on geo grid
  *
  **/
  void cGeo::manhattanDistance()
  {
    std::string open = ".";

    std::vector<std::pair<int, int> > currArr = {mDestination};
    std::vector<std::pair<int, int> > nextArr;
    int counter = 0;

    // loop until the nextArr has the coordinates of all the possible starting poitns
    while (currArr.size() != 0)
    {
      // add possible starting position list to array
      mPossiblePositions.push_back(currArr);

      // iterate through all of current positions
      for (int i = 0; i < currArr.size(); i ++)
      {
        // label on grid
        std::pair<int,int> pos = currArr[i];
        int x = pos.first;
        int y = pos.second;
        mGeoGrid[y][x] = std::to_string(counter);

        std::pair<int,int> front = std::make_pair(x+1, y);
        std::pair<int,int> back = std::make_pair(x-1, y);
        std::pair<int,int> right = std::make_pair(x, y+1);
        std::pair<int,int> left = std::make_pair(x, y-1);

        if ((getCoordString(front) == open) && std::find(nextArr.begin(), nextArr.end(), front) == nextArr.end())
          nextArr.push_back(front);

        if ((getCoordString(back) == open) && std::find(nextArr.begin(), nextArr.end(), back) == nextArr.end())
          nextArr.push_back(back);

        if ((getCoordString(right) == open) && std::find(nextArr.begin(), nextArr.end(), right) == nextArr.end())
          nextArr.push_back(right);

        if ((getCoordString(left) == open) && std::find(nextArr.begin(), nextArr.end(), left) == nextArr.end())
          nextArr.push_back(left);

      }

      currArr = nextArr;
      nextArr.clear();
      counter += 1;
    }

    // set mStart Positions
    try{ 
      setStartPositions(3);
    }
    catch (std::out_of_range& invalid){
      // pass
    }
  }

  /**
  * clears manhattan distance from geo grid
  **/
  void cGeo::clear(){
    mPossiblePositions.clear();
    mStartPositions.clear();


    for (int i = 0; i < mGeoGrid.size(); i++)
    {
      for (int j = 0; j < mGeoGrid[i].size(); j++)
      {     
          if (std::isdigit(mGeoGrid[i][j][0])) // if grid element contains digit then clear 
            mGeoGrid[i][j] = ".";
      }
    }
  }


  /**
  * printing starting position
  *
  **/
  void cGeo::printStarting()
  {
    std::cout << "Size of starting pos" << std::to_string(mStartPositions.size()) << std::endl;

      for (int j = 0; j < mStartPositions.size(); j++){
        std::pair<int,int> pos = mStartPositions[j];
        std::cout << pos.first << "," << pos.second << " ";
      }

  }

  void cGeo::setStartPositions(int num)
  {
    std::vector< std::pair<int, int> > temp = mPossiblePositions[getStartDist()];
    mStartPositions = temp;

    std::vector< std::pair<int, int> > chosen = {};
    int count = 0;

    while (count < num)
    {
      int rand = Random::getInt(0, temp.size()-1);
      std::pair<int, int> pos = temp[rand];
      if (std::find(chosen.begin(), chosen.end(), pos) == chosen.end())
      {
        chosen.push_back(pos);
        count++;
      }
    }

    mStartPositions = chosen;
  }



  /************************  GEOLIST FUNCTIONS FOR PRINTING ************************/


  /**
  * Prints current geo grid
  **/
  std::string cGeo::stringGeo(std::pair<int,int> pos, int facing, std::vector<cObject> listObjects)
  {
    std::vector<std::pair<int,int>> obCoords;
    for (auto obstacle: listObjects)
    {
      obCoords.push_back(obstacle.getPos());
      std::cout << "obstacle " << obstacle.getPosString() << std::endl;
    }

    std::string world;

    for(int i=mGeoGrid.size()-1; i>-1; i--){
      for(int k=0; k<mGeoGrid[i].size(); k++){
        // print organism

        if (i==pos.second && k==pos.first) // print mFacing position
        { 
          if(facing == 0)
            world += "^ ";
          if(facing == 90)
            world += "> ";
          if(facing == 180)
            world +="v ";
          if(facing == 270)
            world += "< ";
        }

        if (std::find(obCoords.begin(), obCoords.end(), std::make_pair(k,i)) != obCoords.end() )
          world += "#";

        else // print destination
        { 
          if (i==mDestination.second && k==mDestination.first)
            world += mDestString+ " ";
          else
            world += mGeoGrid[i][k] + " ";
        }

      }
      world += "\n";
    }
    // world += "\ncurrent position: " + std::to_string(curr_pos.first) + ", " + std::to_string(curr_pos.second) + "\n";
    return world;
  }

  std::string cGeo::dimensions(){
    return std::to_string(mGeoGrid.size()) + "," + std::to_string(mGeoGrid[0].size()) + "\n";
  }

  std::string cGeo::walls(){
    /**
    * returns comma seperated list of all wall offsets
    *
    * @return: string list
    **/
  
    return std::to_string(mWallNum) + "\n" + mWallList;
  }

  /**************************************************************************************
*
*                                OBJECT FUNCTIONS
*
***************************************************************************************/
  /**
  * moves organism
  * UPDATES: current position
  *
  * @return: 1 if valid move, 0 if not
  **/
  int cObject::move(std::shared_ptr<cGeo> geo, std::vector<cObject> objectList)
  {
    std::pair<int,int> newPos;
    int x = mPos.first;
    int y = mPos.second;

    if(mFacing == 0)// move for mFacing north
      newPos = std::make_pair(x, y+1);

    if(mFacing == 90)// move for mFacing east
      newPos = std::make_pair(x+1, y);

    if(mFacing == 180)// move for mFacing south
      newPos = std::make_pair(x, y-1);

    if(mFacing == 270)// move for mFacing west
      newPos = std::make_pair(x-1,y);

    // if grid coord is integer, change position, else return false

    if (std::isdigit(geo->getCoordString(newPos)[0]) && this->checkPosition(newPos, objectList)){
      mPos = newPos;
      return 1;
    }
    else
      return 0;
    
  }
  
  /**
  * turns organism
  * UPDATES: grid_sensors, mCompass, and facing
  *
  * @parameter: direction (left or right) to turn
  **/
  void cObject::turn(std::string direction)
  {
    if (direction == "left")
    {// turning left
      mFacing = (mFacing-90)%360;

      if (mFacing < 0)
        mFacing += 360;
    }

    if (direction == "right")
    {// turning right
      mFacing = (mFacing+90)%360;
    }

  }

  bool cObject::checkPosition(std::pair<int,int> newPos, std::vector<cObject> listObjects)
  {
    bool validMove = 1;
    for (auto object: listObjects)
    {
      if (object != *this && object.getPos() == newPos)
      {
        validMove = 0;
        break;
      }

    }
    return validMove;
  }

/******************* visualizer information ****************/

std::string cObject::getPosString()
{
  std::string x = std::to_string(getPos().first);
  std::string y = std::to_string(getPos().second);
  std::string rot;

  if (mFacing % 180 == 0){
    int temp = (mFacing+180)%360;
    rot = std::to_string(temp);
  }
  else
    rot = std::to_string(mFacing);

  std::string posString = "\npositional\n" + x + "," + y + "," + rot;
  return posString;
}

/**************************************************************************************
*
*                                CAR FUNCTIONS
*
***************************************************************************************/
/******************** GETTER FUNCTIONS *****************/
  std::vector<int> cCar::getCompass()
  {
    /**
    * @return: mCompass variable
    **/
    return mCompass;
  }

  /*
  * returns all positions in grid_sensors
  *
  * @return: vector of ints cooresponding to if coordinate is open or not
  * has object = 1, open = 0
  **/
  std::vector<int> cCar::getSensors(std::shared_ptr<cGeo> geo)
  {
    // std::cout << "sensors: ";
    std::vector< int > retSensors;

    for(int side = 0; side < mGridSensor.size(); side++){ // for grid sensor side
      for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){ // for every sensor in the side

        std::pair<int, int> offset = mGridSensor[side][sensor]; // offset from current position
        std::pair<int, int> actual_location(mPos.first+offset.first, mPos.second+offset.second);

        if (std::abs(offset.first) <= mSensorInputRange && std::abs(offset.second) <= mSensorInputRange)
        {
          // std::cout << std::to_string(offset.first) << ", " << std::to_string(offset.second) << std::endl;

          if (geo->getCoordString(actual_location) != "w")  // if spot is open
          {
            retSensors.push_back(1);
            // std::cout << "1," << std::endl;
          }
          else
          {  
            retSensors.push_back(0);
            // std::cout << "0," << std::endl;
          }
        }

      }
    }

    // std::cout << std::endl;

    return retSensors;
  }

  /*
  *
  * returns all positions in grid_sensors
  *
  * @return: vector of ints cooresponding to if coordinate is open or not
  * has object = 1, open = 0
  **/
  std::vector<int> cCar::getSensorsOutput(std::shared_ptr<cGeo> geo)
  {
    // std::cout << "sensors: ";
    std::vector< int > retSensors;

    for(int side = 0; side < mGridSensor.size(); side++){ // for grid sensor side
      for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){ // for every sensor in the side

        std::pair<int, int> offset = mGridSensor[side][sensor]; // offset from current position
        std::pair<int, int> actual_location(mPos.first+offset.first, mPos.second+offset.second);

        auto it = std::find(mRemoveOutputSensors.begin(), mRemoveOutputSensors.end(), offset);

        if (it == mRemoveOutputSensors.end() && std::abs(offset.first) <= mSensorOutputRange && std::abs(offset.second) <= mSensorOutputRange)
        {
          // std::cout << std::to_string(offset.first) << ", " << std::to_string(offset.second) << std::endl;

          if (geo->getCoordString(actual_location) != "w")  // if spot is open
          {
            retSensors.push_back(1);
            // std::cout << "1," << std::endl;
          }
          else
          {  
            retSensors.push_back(0);
            // std::cout << "0," << std::endl;
          }
        }

      }
    }

    // std::cout << std::endl;

    return retSensors;
  }

  /**
  * returns if the side sensor has object or not
  *
  * @return: vecotr of if left, front, or right have objects
  **/
  std::vector<int> cCar::getSideSensors(int sensorColumns, std::shared_ptr<cGeo> geo)
  {
    // std::cout << "side sensors: ";
    std::vector<int> ret_sensors;

    for(int side = 0; side < mGridSensor.size(); side++){ // for grid sensor side
      int object = 1;

      for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){ // for every sensor in the side
        std::pair<int, int> offset = mGridSensor[side][sensor];// offset from current position
        std::pair<int, int> actual_location(mPos.first+offset.first, mPos.second+offset.second);

        if (offset.first <= sensorColumns && geo->getCoordString(actual_location) == "w")
        {
          object = 0;
          break;
        }
      }
      // std::cout << std::to_string(object) << ", ";
      ret_sensors.push_back(object);// add if side is open or not
    }

    // std::cout << std::endl;
    return ret_sensors;
  }


/************************* SETTER FUNCTIONS ************************/
/*
* Sets member variable facing direction to facing
*/
void cCar::setFacing(int facing)
{
  resetOffsets();
  for (int i = 0; i < (facing/90); i++)
  {
    turn("right");
  }
}

void cCar::resetOffsets()
{
    mFacing = 0;

    mRemoveOutputSensors.clear();
    mRemoveOutputSensors.insert(mRemoveOutputSensors.end(),mRemoveOutputSensorsCopy.begin(),mRemoveOutputSensorsCopy.end());

    mGridSensor.clear();

    // gettting offset coordinates for sensors
    if (mSensorInputRange > 1)
    {
      std::vector< std::pair<int, int> > sLeft2;
      sLeft2.push_back(std::make_pair(-2, 0));
      sLeft2.push_back(std::make_pair(-2, 1));
      sLeft2.push_back(std::make_pair(-2, 2));
      mGridSensor.push_back(sLeft2);
    }

    // gettting offset coordinates for sensors
    std::vector< std::pair<int, int> > sLeft;
    if (mSensorInputRange > 0)
    {
      sLeft.push_back(std::make_pair(-1, 0));
      sLeft.push_back(std::make_pair(-1, 1));

      if (mSensorInputRange > 1)
        sLeft.push_back(std::make_pair(-1, 2));

      mGridSensor.push_back(sLeft);
    }
    
    std::vector< std::pair<int, int> > sFront;
    if (mSensorInputRange > 0)
    {
      sFront.push_back(std::make_pair(0, 1));

      if (mSensorInputRange > 1)
          sFront.push_back(std::make_pair(0, 2));
        
      mGridSensor.push_back(sFront);
    }
    
    std::vector< std::pair<int, int> > sRight;
    if (mSensorInputRange > 0)
    {
      sRight.push_back(std::make_pair(1, 0));
      sRight.push_back(std::make_pair(1, 1));

      if (mSensorInputRange > 1)
        sRight.push_back(std::make_pair(1, 2));

      mGridSensor.push_back(sRight);
    }

    if (mSensorInputRange > 1)
    {
      std::vector< std::pair<int, int> > sRight2;
      sRight2.push_back(std::make_pair(2, 0));
      sRight2.push_back(std::make_pair(2, 1));
      sRight2.push_back(std::make_pair(2, 2));

      mGridSensor.push_back(sRight2);
    }
}

/************************* ALTERING FUNCTIONS ************************/

  /**
  * Resets the mCompass for current position
  *
  **/
  // clear mCompass
  void cCar::configureCompass(std::shared_ptr<cGeo> geo)
  {
    mCompass.clear();

    int front, right, back, left;
    std::pair<int,int> dest = geo->getDestPos();
    // get mCompass directions from facing position
    if (mFacing == 0)
    {
      front = 1 ? (dest.second > mPos.second) : 0; // destination x coor is greater than current x position
      back = 1 ? (dest.second < mPos.second) : 0;
      right = 1 ? (dest.first > mPos.first) : 0;
      left = 1 ? (dest.first < mPos.first) : 0;
    }

    if (mFacing == 90)
    {
      front = 1 ? (dest.first > mPos.first) : 0; // destination x coor is greater than current x position
      back = 1 ? (dest.first < mPos.first) : 0;
      right = 1 ? (dest.second < mPos.second) : 0;
      left = 1 ? (dest.second > mPos.second) : 0;
    }

    if (mFacing == 180)
    {
      front = 1 ? (dest.second < mPos.second) : 0; // destination x coor is greater than current x position
      back = 1 ? (dest.second > mPos.second) : 0;
      right = 1 ? (dest.first < mPos.first) : 0;
      left = 1 ? (dest.first > mPos.first) : 0;
    }

    if (mFacing == 270)
    {
      front = 1 ? (dest.first < mPos.first) : 0; // destination x coor is greater than current x position
      back = 1 ? (dest.first > mPos.first) : 0;
      right = 1 ? (dest.second > mPos.second) : 0;
      left = 1 ? (dest.second < mPos.second) : 0;
    }

    // add to mCompass
    mCompass.push_back(front); // #0
    mCompass.push_back(right); // #1
    mCompass.push_back(back); // #2
    mCompass.push_back(left); // #3
  }

  /**
  * turns organism
  * UPDATES: grid_sensors, mCompass, and facing
  *
  * @parameter: direction (left or right) to turn
  **/
  void cCar::turn(std::string direction)
  {
    if (direction == "left")
    {// turning left
      //update facing position
      mFacing = (mFacing-90)%360;

      if (mFacing < 0)
        mFacing += 360;

      // update grid sensor coordinates
      for(int side = 0; side < mGridSensor.size(); side++){
        for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){
          std::pair<int, int> coord = mGridSensor[side][sensor];
          std::pair<int, int> new_sensor = std::make_pair((coord.second*-1), coord.first);

          mGridSensor[side][sensor] = new_sensor;
        }
      }

      // update remove output sensor coordinates
      for(int sensor = 0; sensor < mRemoveOutputSensors.size(); sensor++)
      {
        std::pair<int, int> coord = mRemoveOutputSensors[sensor];
        std::pair<int, int> new_sensor = std::make_pair((coord.second*-1), coord.first);

        mRemoveOutputSensors[sensor] = new_sensor;
      }

    }

    if (direction == "right")
    {// turning right
      // update facing position
      mFacing = (mFacing+90)%360;

      // update grid sensor coordinates
      for(int side = 0; side < mGridSensor.size(); side++){
        for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){
          std::pair<int, int> coord = mGridSensor[side][sensor];
          std::pair<int, int> newSensor = std::make_pair(coord.second, (coord.first*-1));

          mGridSensor[side][sensor] = newSensor;
        }
      }

      // update remove output sensor coordinates
      for(int sensor = 0; sensor < mRemoveOutputSensors.size(); sensor++)
      {
        std::pair<int, int> coord = mRemoveOutputSensors[sensor];
        std::pair<int, int> new_sensor = std::make_pair(coord.second, (coord.first*-1));

        mRemoveOutputSensors[sensor] = new_sensor;
      }

    }

  }

/**************************************************************************************
*
*                                OBSTACLE FUNCTIONS
*
***************************************************************************************/