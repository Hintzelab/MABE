#pragma once

#include "../AbstractWorld.h"
#include "SourceFiles/Geo.h"
#include "SourceFiles/Object.h"
#include "SourceFiles/Car.h"
#include "SourceFiles/Obstacle.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


class MapWorld : public AbstractWorld {
  public:
  // shared parameters
    // geo info
    static std::shared_ptr<ParameterLink<std::string>> inputFilesPL;

    // evaluation info
    static std::shared_ptr<ParameterLink<int>> movesMultiplierPL;

    // organism info
    static std::shared_ptr<ParameterLink<int>> sensorInputRangePL;
    static std::shared_ptr<ParameterLink<int>> sensorOutputRangePL;
    static std::shared_ptr<ParameterLink<int>> outputCompassSensorPL;
    static std::shared_ptr<ParameterLink<std::string>> turnOffSensorsPL;

    static std::shared_ptr<ParameterLink<double>> rewardForSensorPL;
    static std::shared_ptr<ParameterLink<double>> rewardForDirectionPL;
    static std::shared_ptr<ParameterLink<int>> rewardForGoalPL;
    static std::shared_ptr<ParameterLink<double>> hitWallDeductionPL;

    // obstacle info
    static std::shared_ptr<ParameterLink<std::string>> obstaclesPL;

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


    std::vector<std::pair<int,int>> removeOutputSensors;

    //  registered parameters
    int sensorInputRange;
    int sensorOutputRange;
    int outputCompassSensor;

    int movesMultiplier;
    double rewardForSensor;
    double rewardForDirection;
    int rewardForGoal;
    double hitWallDeduction;

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

};
