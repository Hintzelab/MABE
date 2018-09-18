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

#include "../../Utilities/Random.h"
#include "../../Genome/AbstractGenome.h"
#include "../../Utilities/Utilities.h"

#include "../AbstractBrain.h"

class CGPBrain : public AbstractBrain {
public:
  static std::shared_ptr<ParameterLink<int>> hiddenNodesPL;
  // int nrHiddenValues;

  static std::shared_ptr<ParameterLink<std::string>> genomeNamePL;
  // std::string genomeName;

  static std::shared_ptr<ParameterLink<std::string>> availableOperatorsPL;
  std::vector<std::string> availableOperators;

  static std::shared_ptr<ParameterLink<double>> magnitudeMaxPL;
  static std::shared_ptr<ParameterLink<double>> magnitudeMinPL;
  // double magnitudeMax;
  // double magnitudeMin;

  static std::shared_ptr<ParameterLink<int>> numOpsPreVectorPL;
  // int numOpsPreVector;

  static std::shared_ptr<ParameterLink<std::string>> buildModePL;
  // string buildMode;

  static std::shared_ptr<ParameterLink<int>> codonMaxPL;
  // int codonMax;

  static std::shared_ptr<ParameterLink<bool>> readFromOutputsPL;
  // bool readFromOutputs;

  std::vector<double> readFromValues; // list of values that can be read from
                                 // (inputs, outputs, hidden)
  std::vector<double> writeToValues;  // list of values that can be written to (there
                                 // will be this number of trees) (outputs,
                                 // hidden)

  int nrInputTotal;  // inputs + last outputs (maybe) + hidden
  int nrOutputTotal; // outputs + hidden

  std::map<std::string, int> allOps;
  std::vector<int> availableOps;
  int availableOpsCount;

  std::vector<std::vector<int>> brainVectors; // instruction sets (op,in1,in2)

  CGPBrain() = delete;

  CGPBrain(int _nrInNodes, int _nrOutNodes,
           std::shared_ptr<ParametersTable> PT_ = nullptr);
  CGPBrain(int _nrInNodes, int _nrOutNodes,
           std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes,
           std::shared_ptr<ParametersTable> PT_ = nullptr);

  virtual ~CGPBrain() = default;

  virtual void update() override;

  virtual std::shared_ptr<AbstractBrain> makeBrain(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) override {
    std::shared_ptr<CGPBrain> newBrain =
        std::make_shared<CGPBrain>(nrInputValues, nrOutputValues, _genomes, PT);
    return newBrain;
  }

  virtual std::unordered_set<std::string> requiredGenomes() override {
    return {genomeNamePL->get(PT)};
  }

  virtual std::string description() override;
  virtual DataMap getStats(std::string &prefix) override;
  virtual std::string getType() override { return "CGP"; }

  virtual void resetBrain() override;

  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;
  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes);
};

inline std::shared_ptr<AbstractBrain>
CGPBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
  return std::make_shared<CGPBrain>(ins, outs, PT);
}
