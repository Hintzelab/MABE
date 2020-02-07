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
  int mSensorInputRange = 0;
  int mSensorOutputRange = 0;

  std::vector< std::vector<std::pair<int, int>> > mGridSensor = {};
  std::vector< int > mCompass = {};

  std::vector<std::pair<int,int>> mRemoveOutputSensors = {};
  std::vector<std::pair<int,int>> mRemoveOutputSensorsCopy = {};


public:
  // constructors
  /**
    - vector< vector< pair<int,int> > > grid_sensor: sensors on the mGeoGrid
    - vector< int> mCompass: mCompass directions of where the destination is
  **/

  cCar() = delete;
  cCar(int serialNumber) = delete;

  cCar(int serialNumber, int sensorInputRange, int sensorOutputRange, std::vector<std::pair<int,int>> originalSensor) 
  {
    mSerialNumber = serialNumber;

    mSensorInputRange = sensorInputRange;
    mSensorOutputRange = sensorOutputRange;
    mRemoveOutputSensors = originalSensor;
    mRemoveOutputSensorsCopy = originalSensor;
    resetOffsets();
  }

  // getter functions
  std::vector<int> getCompass();
  std::vector<int> getSensors(std::shared_ptr<cGeo>);
  std::vector<int> getSensorsOutput(std::shared_ptr<cGeo>);
  std::vector<int> getSideSensors(int, std::shared_ptr<cGeo>);


  // setter functions
  void setFacing(int) override;
  void resetOffsets();
  

  // updating FUNCTIONS organism
  void configureCompass(std::shared_ptr<cGeo> geo);
  void turn(std::string) override;
};
