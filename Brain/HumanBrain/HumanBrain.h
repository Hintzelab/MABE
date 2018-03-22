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

#include <cmath>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

class HumanBrain : public AbstractBrain {
public:
  static std::shared_ptr<ParameterLink<bool>> useActionMapPL;
  static std::shared_ptr<ParameterLink<std::string>> actionMapFileNamePL;

  bool useActionMap;
  std::string actionMapFileName;

  std::map<char, std::vector<double>> actionMap;
  std::map<char, std::string> actionNames;

  HumanBrain() = delete;

  HumanBrain(int _nrInNodes, int _nrOutNodes,
             std::shared_ptr<ParametersTable> PT_ = Parameters::root);

  virtual ~HumanBrain() = default;

  virtual void update() override;

  virtual std::shared_ptr<AbstractBrain>
  makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
                &_genomes) override;
  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;

  // virtual shared_ptr<AbstractBrain>
  // makeMutatedBrainFrom(shared_ptr<AbstractBrain> parent) override {
  //	//cout << "  in makeMutatedBrainFrom" << endl;
  //	return make_shared<HumanBrain>(nrInputValues, nrOutputValues, PT);
  //}

  // virtual shared_ptr<AbstractBrain>
  // makeMutatedBrainFromMany(vector<shared_ptr<AbstractBrain>> parents)
  // override {
  //	//cout << "  in makeMutatedBrainFromMany" << endl;
  //	return make_shared<HumanBrain>(nrInputValues, nrOutputValues, PT);
  //}

  virtual std::string description() override;
  virtual DataMap getStats(std::string &prefix) override;
  virtual std::string getType() override { return "Human"; }

  virtual void resetBrain() override;

  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes);

  virtual std::unordered_set<std::string> requiredGenomes() override {
    return {};
  }
};

inline std::shared_ptr<AbstractBrain> HumanBrain_brainFactory(
    int ins, int outs, std::shared_ptr<ParametersTable> PT = Parameters::root) {
  return std::make_shared<HumanBrain>(ins, outs, PT);
}
