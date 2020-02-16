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
  Parameters::register_parameter("WORLD_MAP-turnOffSensorsPL", (std::string) "-1,1:1,1",
                                 "ass cooredinates to turn off");

std::shared_ptr<ParameterLink<int>> MapWorld::insertWallsPL =
  Parameters::register_parameter("WORLD_MAP-insertWallsPL", 30,
                                 "ass cooredinates to turn off");

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
  cCar car = cCar(sensorInputRange, sensorOutputRange, removeOutputSensors);
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

    
    car.setGeo(currGeo);

    int startDist = car.getGeo()->getStartDist(); 
    aveStartDist += startDist;
    int movesPerOrganism = startDist*movesMultiplier;


    // adding wall and destination to visualizer
    if (visualize)
    {
      // std::string dimension = "dimension\n" + currGeo->dimensions();
      // FileManager::writeToFile("MapWorldData.csv", dimension);

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
          car.configureCompass();
     
          // adding coordinates to visualizer organism position (x,y,radians rotation)
          if (visualize)
          {
            FileManager::writeToFile("MapWorldData.csv", car.getPosString());
          }

          if (debug)
          {
            std::cout << car.stringGeo() << std::endl;
          }
      

          // INPUT to brain 0-: grid sensors
          int counter_input = 0; // input node number
          for(auto grid_sensor : car.getSensors()){
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
          for(auto grid_sensor : car.getSensorsOutput()){
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
            int moved = car.move();
            if (!moved)
            {
              hit = 1;
              if (debug)
                std::cout << "DEDUCTION: Hit wall" << std::endl;
            }
          }
          
          moves += 1;

          //** scoring valid moves **//
          std::pair<int, int> dPos = car.getGeo()->getDestPos();
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
          int currDist = std::stoi(car.getGeo()->getCoordString(car.getPos()));
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


/****************** getter FUNCTIONS ****************************/
  /**
  * @return: destination position pair
  **/
  int cGeo::mapSize()
  {
    return mGeoGrid.size();
  }

  /**
  * @return: destination position pair
  **/
  std::pair<int, int> cGeo::getDestPos()
  {
    return mDestination;
  }

  /**
  * Gets characerter at pos from mGeoGrid
  *
  * @param: pair position (x,y)
  * @return: character from mGeoGrid
  **/
  std::string cGeo::getCoordString(std::pair<int, int> pos)
  {
    return mGeoGrid[pos.second][pos.first];
  }

  int cGeo::getStartDist()
  {
    return ((mPossiblePositions.size()-1)/2);
  }

  /**
  * Gets characerter at pos from mGeoGrid
  *
  * @param: pair position (x,y)
  * @return: character from mGeoGrid
  **/

    std::vector< std::pair<int, int> > cGeo::getStartPositions()
  {
    return mStartPositions;
  }

  


/************************  HELPER FUNCTIONS ************************/
  /**
  * Pick random open spot
  *
  * @return: coordinate pair
  **/
  void cGeo::resetMap(int walls)
  {
    clear();
    for (int i = 0; i < walls; i++)
    {


    }
    pickDestCoord();
    manhattanDistance();
  }

  void cGeo::pickDestCoord()
  {
    std::string open = ".";
    int x = Random::getInt(0, mGeoGrid[0].size()-1);
    int y = Random::getInt(0, mGeoGrid.size()-1);

    mDestination = std::make_pair(x,y);

    // if at wall, repick coordinate
    while (getCoordString(mDestination) != open){
      x = Random::getInt(0, mGeoGrid[0].size()-1);
      y = Random::getInt(0, mGeoGrid.size()-1);

      mDestination = std::make_pair(x, y);
    }
  }



  /**
  * Manhattan distance on geo grid
  *
  **/
  void cGeo::manhattanDistance()
  {
    std::string open = ".";

    std::vector<std::pair<int, int> > curr_arr = {mDestination};
    std::vector<std::pair<int, int> > next_arr;
    int counter = 0;

    // loop until the next_arr has the coordinates of all the possible starting poitns
    while (curr_arr.size() != 0)
    {
      // add possible starting position list to array
      mPossiblePositions.push_back(curr_arr);

      // iterate through all of current positions
      for (int i = 0; i < curr_arr.size(); i ++)
      {
        // label on grid
        std::pair<int,int> pos = curr_arr[i];
        int x = pos.first;
        int y = pos.second;
        mGeoGrid[y][x] = std::to_string(counter);

        std::pair<int,int> front = std::make_pair(x+1, y);
        std::pair<int,int> back = std::make_pair(x-1, y);
        std::pair<int,int> right = std::make_pair(x, y+1);
        std::pair<int,int> left = std::make_pair(x, y-1);

        if ((getCoordString(front) == open) && !(findPos(next_arr, front)))
          next_arr.push_back(front);

        if ((getCoordString(back) == open) && !(findPos(next_arr, back)))
          next_arr.push_back(back);

        if ((getCoordString(right) == open) && !(findPos(next_arr, right)))
          next_arr.push_back(right);

        if ((getCoordString(left) == open) && !(findPos(next_arr, left)))
          next_arr.push_back(left);

      }

      curr_arr = next_arr;
      next_arr.clear();
      counter += 1;
    }

    // set mStart Positions
    try{ 
      setStartPositions(3);
    }
    catch (std::out_of_range& invalid){
      // pass
    }

    // printStarting();
  }

  /**
  * clears manhattan distance from geo grid
  *
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
  * clears manhattan distance from geo grid
  *
  **/
  bool cGeo::findPos(std::vector<std::pair<int,int>> arr, std::pair<int,int> ele)
  {
    for (int i = 0; i < arr.size(); i++){
      if ((arr[i].first == ele.first) and (arr[i].second == ele.second))
        return true;
    }

    return false;
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
  std::string cGeo::stringGeo(std::pair<int,int> pos, int facing)
  {
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

  // std::pair<double, double> cGeo::get_mid(){
  //   /**
  //   * returns: pair for size of grid
  //   **/
  //
  //   return std::make_pair((double)mGeoGrid[0].size()/2, (double)mGeoGrid.size()/2);
  // }
  //
  // std::string cGeo::get_offset(int x, int y){
  //   /**
  //   * returns the offset from the center
  //   *
  //   * @return: coordinate pair
  //   **/
  //   double x_len = get_mid().first-x;
  //   double y_len = get_mid().second-y;
  //   return std::to_string(x_len) + "," + std::to_string(y_len);
  // }
  //
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
*                                ORGANISM FUNCTIONS
*
***************************************************************************************/

  /**
  * parameters:
    - pair<int,int> curr_pos: current position of ORGANISM
    - int facing: facing direction in degrees
    - geo curr_grid: current grid the organism is on
    - vector< vector< pair<int,int> > > grid_sensor: sensors on the mGeoGrid
    - vector< int> mCompass: mCompass directions of where the destination is
  **/
  // cCar::cCar(int sensorInputRange, int sensorOutputRange, std::vector<std::pair<int,int>> removeOutputSensors)
  // {

  //   // // getting mCompass sensors
  //   // configure_mCompass();
  // }

/******************** GETTER FUNCTIONS *****************/
  /**
  * @return: current position pair
  **/
  std::shared_ptr<cGeo> cCar::getGeo()
  {
    return mGeo;
  }

  /**
  * @return: current position pair
  **/
  std::string cCar::stringGeo()
  {
    return mGeo->stringGeo(mPos, mFacing);
  }

  /**
  * @return: current position pair
  **/
  std::pair<int, int> cCar::getPos()
  {
    return mPos;
  }

  /**
  * @return: facing direction
  **/
  int cCar::getFacing(){
    return mFacing;
  }

  std::vector<int> cCar::getCompass()
  {
    /**
    * @return: mCompass variable
    **/
    // std::cout << "mCompass";
    // for (int i = 0; i < mCompass.size(); i++){
    //   std::cout << std::to_string(mCompass[i]) << ", ";
    // }
    // std::cout << std::endl;
    return mCompass;
  }

  std::vector<int> cCar::getSensors()
  {
    /*
    *
    * returns all positions in grid_sensors
    *
    * @return: vector of ints cooresponding to if coordinate is open or not
    * has object = 1, open = 0
    **/
    // std::cout << "sensors: ";
    std::vector< int > retSensors;

    for(int side = 0; side < mGridSensor.size(); side++){ // for grid sensor side
      for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){ // for every sensor in the side

        std::pair<int, int> offset = mGridSensor[side][sensor]; // offset from current position
        std::pair<int, int> actual_location(mPos.first+offset.first, mPos.second+offset.second);

        if (std::abs(offset.first) <= mSensorInputRange && std::abs(offset.second) <= mSensorInputRange)
        {
          // std::cout << std::to_string(offset.first) << ", " << std::to_string(offset.second) << std::endl;

          if (mGeo->getCoordString(actual_location) != "w")  // if spot is open
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

  std::vector<int> cCar::getSensorsOutput()
  {
    /*
    *
    * returns all positions in grid_sensors
    *
    * @return: vector of ints cooresponding to if coordinate is open or not
    * has object = 1, open = 0
    **/
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

          if (mGeo->getCoordString(actual_location) != "w")  // if spot is open
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

  std::vector<int> cCar::getSideSensors(int sensorColumns)
  {
    /**
    * returns if the side sensor has object or not
    *
    * @return: vecotr of if left, front, or right have objects
    **/
    // std::cout << "side sensors: ";
    std::vector<int> ret_sensors;

    for(int side = 0; side < mGridSensor.size(); side++){ // for grid sensor side
      int object = 1;

      for(int sensor = 0; sensor < mGridSensor[side].size(); sensor++){ // for every sensor in the side
        std::pair<int, int> offset = mGridSensor[side][sensor];// offset from current position
        std::pair<int, int> actual_location(mPos.first+offset.first, mPos.second+offset.second);

        if (offset.first <= sensorColumns && mGeo->getCoordString(actual_location) == "w")
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
* Sets member variable pointer mGeo to geoAddr
*/
void cCar::setGeo(std::shared_ptr<cGeo> geoAddr)
{
  mGeo = geoAddr;
}

/*
* Sets member variable current position to pos
*/
void cCar::setPos(std::pair<int,int> pos)
{
  mPos = pos;
}

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
  void cCar::configureCompass()
  {
    /**
    * Resets the mCompass for current position
    *
    **/
    // clear mCompass
    mCompass.clear();

    int front, right, back, left;
    std::pair<int,int> dest = mGeo->getDestPos();
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

  void cCar::turn(std::string direction)
  {
    /**
    * turns organism
    * UPDATES: grid_sensors, mCompass, and facing
    *
    * @parameter: direction (left or right) to turn
    **/

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


  int cCar::move()
  {
    /**
    * moves organism
    * UPDATES: current position
    *
    * @return: 1 if valid move, 0 if not
    **/
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

    if (std::isdigit(mGeo->getCoordString(newPos)[0])){
      mPos = newPos;
      return 1;
    }
    else
      return 0;
    
  }




/******************* visualizer information ****************/

std::string cCar::getPosString()
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


  