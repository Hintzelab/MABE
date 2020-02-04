#pragma once

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


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
  // @return: destination position pair
  int mapSize() { return mGeoGrid.size(); } 
  // @return: destination position pair  
  std::pair<int, int> getDestPos() { return mDestination; } 
  // @return: character from mGeoGrid  
  std::string getCoordString(std::pair<int, int> pos) { 
    return mGeoGrid[pos.second][pos.first]; }   
  // @return: half of possible distance
  int getStartDist() { return ((mPossiblePositions.size()-1)/2); }  
  // @return: character from mGeoGrid 
  std::vector< std::pair<int, int> > getStartPositions() { return mStartPositions; }   

  
  // Helper functions
  void resetMap();
  void pickDestCoord();
  void manhattanDistance();
  void clear();
  void printStarting();

  void setStartPositions(int);

  // visualizer
  std::string dimensions();
  std::string walls();
  
  //PRINTING
  std::string stringGeo(std::pair<int,int> pos = std::make_pair(-1,-1), int facing = -1);

};
