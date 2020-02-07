#pragma once

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Geo.h"

class cObject{
protected:
  //variables
  int mSerialNumber;
  std::pair<int,int> mPos = std::make_pair(0,0);
  int mFacing = 0;

public:
  // @return: current position pair
  std::pair<int, int> getPos() { return mPos; }
  // @return: facing direction
  int getFacing() { return mFacing; }
  // @return: serialNumber
  int getSN() { return mSerialNumber; }


  // setter functions
  // Sets member variable current position to pos
  void setPos(std::pair<int,int> pos) { mPos = pos; }

  // Sets facing direction
  virtual void setFacing(int newFacing) { mFacing = newFacing; };
  
  // updating FUNCTIONS organism
  virtual void turn(std::string);
  virtual int move(std::shared_ptr<cGeo>, std::vector<cObject>);

  // visualizer functions
  std::string getPosString();

  //
  bool checkPosition(std::pair<int,int>, std::vector<cObject>);

  // operators
  bool operator != (cObject object){ return this->getSN() == object.getSN(); }
};
