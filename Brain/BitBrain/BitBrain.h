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

#include <algorithm>
#include <iostream>
#include <math.h>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

using namespace std;

class BitBrain : public AbstractBrain
{
public:
  class Gate
  {
  public:
    vector<int> ins;
    vector<int> output;

    Gate(shared_ptr<AbstractGenome::Handler> handler, int range, int nrOfIns)
    {
      for (int i = 0; i < nrOfIns; i++) {
        ins.push_back(handler->readInt(0, range - 1));
      }

      for (int i = 0; i < (1 << nrOfIns); i++) {
        output.push_back(handler->readInt(0, 1));
      }
    }

    double update(vector<double>& I)
    {
      int theI = 0;

      for (auto i : ins) {
        theI = (theI << 1) + Bit(I[i]);
      }
      return output[theI];
    }
  };

  static shared_ptr<ParameterLink<string>> genomeNamePL;
  static shared_ptr<ParameterLink<int>> nrOfHiddenNodesPL;
  static shared_ptr<ParameterLink<int>> nrOfLayersPL;
  static shared_ptr<ParameterLink<int>> nrOfGateInsPL;

  vector<vector<double>> nodes;
  vector<vector<shared_ptr<Gate>>> gates;

  int I, O, H, nrOfLayers, nrOfGateIns;

  string genomeName;

  BitBrain() = delete;

  BitBrain(int nrInNodes, int nrOutNodes, shared_ptr<ParametersTable> PT);

  virtual ~BitBrain() = default;

  virtual void update() override;

  virtual shared_ptr<AbstractBrain> makeBrain(
    unordered_map<string, shared_ptr<AbstractGenome>>& genomes) override;

  virtual string description() override;
  virtual DataMap getStats(string& prefix) override;
  virtual string getType() override { return "BIT"; }

  virtual void resetBrain() override;
  virtual void resetOutputs() override;

  vector<int> getHiddenNodes();
  
  virtual void initializeGenomes(
    std::unordered_map<string, std::shared_ptr<AbstractGenome>>& genomes)
    override;

  virtual shared_ptr<AbstractBrain> makeCopy(
    shared_ptr<ParametersTable> PT_ = nullptr) override;

  virtual unordered_set<string> requiredGenomes() override
  {
    return { genomeName };
  }

  inline void setInput(const int& inputAddress, const double& value) override;
  inline double readInput(const int& inputAddress) override;
  inline void setOutput(const int& outputAddress, const double& value) override;
  inline double readOutput(const int& outputAddress) override;

  void showBrain();
};

inline shared_ptr<AbstractBrain>
BitBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT)
{
  return make_shared<BitBrain>(ins, outs, PT);
}