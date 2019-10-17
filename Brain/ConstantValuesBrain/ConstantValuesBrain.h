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

class ConstantValuesBrain : public AbstractBrain {
public:
  static std::shared_ptr<ParameterLink<double>> valueMinPL;
  static std::shared_ptr<ParameterLink<double>> valueMaxPL;
  static std::shared_ptr<ParameterLink<int>> valueTypePL;
  static std::shared_ptr<ParameterLink<int>> samplesPerValuePL;

  static std::shared_ptr<ParameterLink<bool>> initializeUniformPL;
  static std::shared_ptr<ParameterLink<bool>> initializeConstantPL;
  static std::shared_ptr<ParameterLink<double>> initializeConstantValuePL;

  static std::shared_ptr<ParameterLink<std::string>> genomeNamePL;

  ConstantValuesBrain() = delete;

  ConstantValuesBrain(int _nrInNodes, int _nrOutNodes,
                      std::shared_ptr<ParametersTable> PT_ = nullptr);

  virtual ~ConstantValuesBrain() = default;

  virtual void update() override;

  virtual std::shared_ptr<AbstractBrain>
  makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
                &_genomes) override;

  virtual std::unordered_set<std::string> requiredGenomes() override {
    return {genomeNamePL->get(PT)};
  }

  virtual std::string description() override;
  virtual DataMap getStats(std::string &prefix) override;
  virtual std::string getType() override { return "ConstantValues"; }

  virtual void resetBrain() override;
  virtual void resetOutputs() override;

  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;

  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes);
};

inline std::shared_ptr<AbstractBrain>
ConstantValuesBrain_brainFactory(int ins, int outs,
                                 std::shared_ptr<ParametersTable> PT) {
  return std::make_shared<ConstantValuesBrain>(ins, outs, PT);
}

