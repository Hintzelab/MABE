//
//  DayNightWorld.h
//  BasicMarkovBrainTemplate
//
//	author: cigien
//
#pragma once
#include <iostream>
#include <fstream>
//#include <stdio.h>
//#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>

#include "../AbstractWorld.h"

using std::cout;
using std::endl;

class DayNightWorld : public AbstractWorld {
private:
  enum class cell { empty, food, wall };

  // mixed-Case names only for variables that are read from Parameter Links
  int worldX;
  int worldY;
  int worldUpdates;
  int evolCycleLength;
  double foodDensity;
  bool startWithDay;
  int evaluationsPerGeneration;

  std::ofstream vis_file;

  std::vector<std::vector<cell>> MakeGrid(int, int);

  inline std::pair<int, int> Facing(const std::pair<int, int> &loc,
                                    int dir) { // preserves toroidal invariance
    return {dir % 2 ? loc.first : (loc.first + (dir - 1) + worldX) % worldX,
            dir % 2 ? (loc.second + (2 - dir) + worldY) % worldY : loc.second};
  }

  std::vector<std::vector<std::vector<int>>>
  MakeSmellsAndSights(const std::vector<std::vector<cell>> &);

  void InputEnvIntoBrain(std::shared_ptr<AbstractBrain>,
                         const std::vector<std::vector<std::vector<int>>> &,
                         std::pair<int, int>, int, bool);
  void ChangeEnvFromBrainOutput(std::shared_ptr<AbstractBrain>,
                                std::vector<std::vector<cell>> &,
                                std::vector<std::vector<std::vector<int>>> &,
                                std::pair<int, int> &, int &, double &);
  double EvalInMode(std::shared_ptr<AbstractBrain>, int, int, int, bool);

  //	void printGrid(map<pair<int,int>,int>&, map<pair<int,int>,int>&,
  // pair<int, int>, int);
  //	void saveState(	map<pair<int,int>,int>&, map<pair<int,int>,int>&,
  // pair<int, int>, int,int,int,int,int,int);

public:
  static std::shared_ptr<ParameterLink<int>> WorldXPL;
  static std::shared_ptr<ParameterLink<int>> WorldYPL;
  static std::shared_ptr<ParameterLink<int>> worldUpdatesPL;
  static std::shared_ptr<ParameterLink<int>> evolCycleLengthPL;
  static std::shared_ptr<ParameterLink<double>> foodDensityPL;
  static std::shared_ptr<ParameterLink<bool>> startWithDayPL;
  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

  // these should be automatic
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;

  // methods
  DayNightWorld(std::shared_ptr<ParametersTable> PT = nullptr);
    virtual ~DayNightWorld() = default;

    DayNightWorld(const DayNightWorld &) : AbstractWorld(PT) {}
//    DayNightWorld(const DayNightWorld &) = default;

  virtual void evaluateSolo(std::shared_ptr<Organism> org, int analyse,
                            int visualize, int debug);

  virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                        int analyse, int visualize, int debug) override {
    int popSize = groups[groupNamePL->get(PT)]->population.size();
    for (int i = 0; i < popSize; i++) {
        evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse,
                     visualize, debug);
      
    }


	if (visualize) {

        auto pop = groups[groupNamePL->get(PT)]->population;
    	for (auto org:pop) {

	//	std::cout << "hi" << std::endl;
		org->dataMap.writeToFile("vis.csv");
		
	   }
	}




  }

  virtual void EvaluateWithKnockout(std::shared_ptr<Organism> org, int analyse,
                                    int visualize, int debug);

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override {
    std::string groupName = "root::";
    std::string brainName = "root::";
    return {
        {groupName,
         {"B:" + brainName + "," + std::to_string(requiredInputs) + "," +
          std::to_string(requiredOutputs)}}}; // default requires a root group
                                              // and a brain (in root namespace)
                                              // and no genome
    //    return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",8,2"
    //    } } };
  }

  void testing() {
    cout << " testing ... " << endl;

    auto g = MakeGrid(worldX, worldY);
    auto s = MakeSmellsAndSights(g);
    cout << " Done testing ... " << endl;
  }
};

