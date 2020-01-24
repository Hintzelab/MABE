#pragma once

#include "../AbstractWorld.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
// #include <memory>



/**************************************************************************************
*
*                                      GEO
*
***************************************************************************************/
class cGeo{
private:
  // grid vairables
  std::vector< std::vector<std::string> > mGeoGrid;
  std::vector< std::vector<std::pair<int, int> > > mPossiblePositions;
  std::vector< std::pair<int, int> > mStartPositions;
  std::pair<int, int> mDestination;

  std::string mWallList = "";
  int mWallNum = 0;

  const std::string mDestString = "X";
  const std::string mWallString = "w";

/*************** geo functions *****************/
public:
  // constructions
  cGeo(std::string);

  // Getter functions
  int mapSize();
  std::pair<int, int> getDestPos();
  std::string getCoordString(std::pair<int, int>);
  int getStartDist();
  std::vector< std::pair<int, int> > getStartPositions();


  // Helper functions
  void resetMap();
  void pickDestCoord();
  void manhattanDistance();
  void clear();
  bool findPos(std::vector<std::pair<int,int>>, std::pair<int,int>);
  void printStarting();

  void setStartPositions(int);


  // // visualizer
  // std::pair<double, double> get_mid();
  // std::string get_offset(int, int);
  std::string dimensions();
  std::string walls();
  
  
  // //PRINTING
  std::string stringGeo(std::pair<int,int> pos = std::make_pair(-1,-1), int facing = -1);
  // void print_starting();

};


/**************************************************************************************
*
*                                      ORGANISM
*
***************************************************************************************/
class cCar{
private:
  //variables
  std::pair<int,int> mPos;
  int mFacing;

  std::shared_ptr<cGeo> mGeo;
  std::vector< std::vector<std::pair<int, int>> > mGridSensor;
  std::vector< int > mCompass;

public:
  // constructors
  cCar();

  // getter functions
  std::shared_ptr<cGeo> getGeo();
  std::string stringGeo();

  std::pair<int, int> getPos();
  int getFacing();
  std::vector<int> getCompass();
  std::vector<int> getSensors();
  std::vector<int> getSideSensors();

  // setter functions
  void setGeo(std::shared_ptr<cGeo>);
  void setPos(std::pair<int,int>);
  void setFacing(int);
  

  // updating FUNCTIONS organism
  void configureCompass();
  void turn(std::string);
  int move();

  // visualizer functions
  std::string getPosString();
};


/**************************************************************************************
*
*                                      MAPWORLD
*
***************************************************************************************/

class MapWorld : public AbstractWorld {
  public:
  // shared parameters
    // geo info
    static std::shared_ptr<ParameterLink<std::string>> inputFilesPL;

    // evaluation info
    static std::shared_ptr<ParameterLink<int>> movesPerOrganismPL;

    // organism info
    static std::shared_ptr<ParameterLink<double>> rewardForSensorPL;
    static std::shared_ptr<ParameterLink<double>> rewardForDirectionPL;
    static std::shared_ptr<ParameterLink<int>> rewardForGoalPL;

    // mandatory info
    static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
    static std::shared_ptr<ParameterLink<std::string>> brainNamePL;

  // global variables
    std::vector<cGeo> mMapList;

    // geo variables
    // string openChar;
    std::string wallChar;
    std::string carChar;
    std::string destinationChar;

    //  registered parameters
    int movesPerOrganism;
    double rewardForSensor;
    double rewardForDirection;
    int rewardForGoal;

    // brain input, output counters
    int input_num;
    int output_num;


  // main functions
    MapWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
    virtual ~MapWorld() = default;

    void evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                              int visualize, int debug);
    void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                  int analyze, int visualize, int debug);

    virtual std::unordered_map<std::string, std::unordered_set<std::string>>
      requiredGroups() override;

    // helper functions



};
