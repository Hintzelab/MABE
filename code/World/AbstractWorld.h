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

#include <cstdlib>
#include <thread>
#include <vector>

#include <Group/Group.h>
#include <Utilities/Utilities.h>
#include <Utilities/Data.h>
#include <Utilities/Parameters.h>

class AbstractWorld {
public:
  static std::shared_ptr<ParameterLink<bool>> debugPL;
  static std::shared_ptr<ParameterLink<std::string>> worldTypePL;

  const std::shared_ptr<ParametersTable> PT;

  std::vector<std::string> popFileColumns;

  AbstractWorld(std::shared_ptr<ParametersTable> PT_) : PT(PT_) {}
  virtual ~AbstractWorld() = default;

  virtual std::unordered_map<std::string, std::unordered_set<std::string>>
  requiredGroups() = 0; // {
                        //	string groupName = "root::";
                        //	string brainName = "root::";
                        //	return { { groupName,{"B:"+
  // brainName+","+to_string(requiredInputs)+","+to_string(requiredOutputs)}} };
  //// default requires a root group and a brain (in root namespace) and no
  // genome
  //}

  virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups,
	  int analyze = 0, int visualize = 0, int debug = 0) = 0;
};
