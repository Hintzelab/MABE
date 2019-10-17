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

class XorWorld : public AbstractWorld {

public:
  static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
  static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
  static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
  static std::shared_ptr<ParameterLink<int>> brainUpdatesPL;
  int brainUpdates;
  std::string groupName;
  std::string brainName;

  XorWorld(std::shared_ptr<ParametersTable> PT_ = nullptr);
  virtual ~XorWorld() = default;
  void evaluateSolo(std::shared_ptr<Organism> org, int analyze,
                            int visualize, int debug);
  void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
                          int analyze, int visualize, int debug) override;



  std::unordered_map<std::string, std::unordered_set<std::string>>
    requiredGroups() override;
};
