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

class LSTMBrain : public AbstractBrain {
public:
  static std::shared_ptr<ParameterLink<std::string>> genomeNamePL;

  std::string genomeName;

  std::vector<std::vector<double>> Wf, Wi, Wc, Wo;
  std::vector<double> ft, it, Ct, Ot, dt;
  std::vector<double> bt, bi, bC, bO;
  int I_, O_;
  std::vector<double> C, X, H;

  LSTMBrain() = delete;

  LSTMBrain(int _nrInNodes, int _nrOutNodes,
            std::shared_ptr<ParametersTable> PT_);

  virtual ~LSTMBrain() = default;

  virtual void update() override;

  virtual std::shared_ptr<AbstractBrain>
  makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
                &_genomes) override;

  virtual std::string description() override;
  virtual DataMap getStats(std::string &prefix) override;
  virtual std::string getType() override { return "LSTM"; }

  virtual void resetBrain() override;
  virtual void resetOutputs() override;

  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
          &_genomes) override;

  double fastSigmoid(double value) { return value / (1.0 + fabs(value)); }
  void singleLayerUpdate(std::vector<double> &IN, std::vector<double> &out,
                         std::vector<std::vector<double>> &W);
  void vectorMathSigmoid(std::vector<double> &V);
  void vectorMathTanh(std::vector<double> &V);
  void vectorMathElementalPlus(std::vector<double> &A, std::vector<double> &B,
                               std::vector<double> &result);
  void vectorMathElementalMultiply(std::vector<double> &A,
                                   std::vector<double> &B,
                                   std::vector<double> &result);
  void showVector(std::vector<double> &V);

  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) override;

  virtual std::unordered_set<std::string> requiredGenomes() override {
    return {genomeNamePL->get(PT)};
  }
};

inline std::shared_ptr<AbstractBrain>
LSTMBrain_brainFactory(int ins, int outs, std::shared_ptr<ParametersTable> PT) {
  return std::make_shared<LSTMBrain>(ins, outs, PT);
}

