#pragma once

#include "Geo.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class cObject{
protected:
  //variables
  std::pair<int,int> mPos;
  int mFacing = 0;
  std::shared_ptr<cGeo> mGeo;

public:
  // // getter functions
  
  // @return: current position pair
  std::shared_ptr<cGeo> getGeo() { return mGeo; }
  // @return: current position pair
  std::pair<int, int> getPos() { return mPos; }
  // @return: facing direction
  int getFacing() { return mFacing; }


  // setter functions
  // Sets member variable pointer mGeo to geoAddr
  void setGeo(std::shared_ptr<cGeo> geoAddr) { mGeo = geoAddr; }
  // Sets member variable current position to pos
  void setPos(std::pair<int,int> pos) { mPos = pos; }
  // Sets facing direction
  virtual void setFacing(int) = 0;
  
  // updating FUNCTIONS organism
  virtual void turn(std::string) = 0;
  virtual int move() = 0;

  // visualizer functions
  std::string getPosString();
};
