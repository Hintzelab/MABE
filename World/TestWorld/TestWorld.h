//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include "../AbstractWorld.h"

#include <cstdlib>
#include <thread>
#include <vector>

class TestWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<int>> modePL;
  static std::shared_ptr<ParameterLink<int>> numberOfOutputsPL;
  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

  // int mode;
  // int numberOfOutputs;
  // int evaluationsPerGeneration;

  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  // string groupName;
  // string brainName;

  TestWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~TestWorld() = default;

  void evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                            int visualize, int debug);
  virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                        int analyze, int visualize, int debug) {
    int popSize = groups[groupNamePL->get(PT)]->population.size();
    for (int i = 0; i < popSize; i++) {
      evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze,
                   visualize, debug);
    }
  }

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() override {
    return {{groupNamePL->get(PT),
             {"B:" + brainNamePL->get(PT) + ",1," +
              std::to_string(numberOfOutputsPL->get(PT))}}};
    // requires a root group and a brain (in root namespace) and no addtional
    // genome,
    // the brain must have 1 input, and the variable numberOfOutputs outputs
  }
};

