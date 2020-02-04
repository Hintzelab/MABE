#pragma once

#include "Object.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class cCar : public cObject{
private:
  // variables
  int mSensorInputRange;
  int mSensorOutputRange;

  std::vector< std::vector<std::pair<int, int>> > mGridSensor;
  std::vector< int > mCompass;

  std::vector<std::pair<int,int>> mRemoveOutputSensors = {};
  const std::vector<std::pair<int,int>> mRemoveOutputSensorsCopy;


public:
  // constructors
  /**
    - vector< vector< pair<int,int> > > grid_sensor: sensors on the mGeoGrid
    - vector< int> mCompass: mCompass directions of where the destination is
  **/

  cCar(int sensorInputRange, int sensorOutputRange, std::vector<std::pair<int,int>> originalSensor) 
  : mSensorInputRange(sensorInputRange), mSensorOutputRange(sensorOutputRange),
  mRemoveOutputSensors(originalSensor), mRemoveOutputSensorsCopy(originalSensor){
    resetOffsets();
  }

  // getter functions
  std::string stringGeo();

  std::vector<int> getCompass();
  std::vector<int> getSensors();
  std::vector<int> getSensorsOutput();
  std::vector<int> getSideSensors(int);


  // setter functions
  void setFacing(int);
  void resetOffsets();
  

  // updating FUNCTIONS organism
  void configureCompass();
  void turn(std::string);
  int move();
};
