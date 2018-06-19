//  MABE is a product of The Hintze Lab @ MSU

//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../BitBrain/BitBrain.h"

shared_ptr<ParameterLink<string>> BitBrain::genomeNamePL =
  Parameters::register_parameter("BRAIN_BIT_NAMES-genomeName",
                                 (string) "root::",
                                 "root:: is default value");
shared_ptr<ParameterLink<int>> BitBrain::nrOfRecurringNodesPL =
  Parameters::register_parameter("BRAIN_BIT-nrOfRecurringNodes",
                                 0,
                                 "number of recurring nodes");
shared_ptr<ParameterLink<int>> BitBrain::nrOfLayersPL =
  Parameters::register_parameter("BRAIN_BIT-nrOfLayers", 0, "number of layers");
shared_ptr<ParameterLink<int>> BitBrain::nrOfGateInsPL =
  Parameters::register_parameter("BRAIN_BIT-nrOfGateIns",
                                 2,
                                 "number of inputs each gate has");

BitBrain::BitBrain(int nrInNodes,
                   int nrOutNodes,
                   shared_ptr<ParametersTable> PT)
  : AbstractBrain(nrInNodes, nrOutNodes, PT)
{
  genomeName = genomeNamePL->get(PT);
  nrOfLayers = nrOfLayersPL->get(PT);
  nrOfGateIns = nrOfGateInsPL->get(PT);
  H = nrOfRecurringNodesPL->get(PT);
  I = nrInNodes;
  O = nrOutNodes;
}

shared_ptr<AbstractBrain>
BitBrain::makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes)
{
  shared_ptr<BitBrain> newBrain =
    make_shared<BitBrain>(nrInputValues, nrOutputValues, PT);
  auto genomeHandler =
    _genomes[genomeName]->newHandler(_genomes[genomeName], true);
  newBrain->H = H;
  newBrain->I = I;
  newBrain->O = O;
  newBrain->nrOfLayers = nrOfLayers;

  newBrain->nodes.resize(nrOfLayers + 2);

  for (int i = 0; i < nrOfLayers + 2; i++) {
    newBrain->nodes[i].resize(I + O + H);
  }

  newBrain->gates.resize(nrOfLayers + 1);

  for (int i = 0; i < nrOfLayers + 1; i++) {
    newBrain->gates[i].resize(I + O + H);

    for (int j = 0; j < (I + O + H); j++) {
      newBrain->gates[i][j] =
        make_shared<Gate>(genomeHandler, I + O + H, nrOfGateIns);
    }
  }
  return newBrain;
}

void
BitBrain::resetBrain()
{
  for (auto N : nodes) {
    for (int i = 0; i < (int)N.size(); i++) {
      N[i] = 0.0;
    }
  }
}

void
BitBrain::setInput(const int& inputAddress, const double& value)
{
  nodes[0][inputAddress] = value;
}

double
BitBrain::readInput(const int& inputAddress)
{
  return nodes[0][inputAddress];
}

void
BitBrain::setOutput(const int& outputAddress, const double& value)
{
  nodes[(int)nodes.size() - 1][outputAddress] = value;
}

double
BitBrain::readOutput(const int& outputAddress)
{
  return nodes[(int)nodes.size() - 1][outputAddress];
}

void
BitBrain::update()
{
  for (int i = 0; i < nrOfLayers + 1; i++) {
    for (int j = 0; j < gates[i].size(); j++) {
      nodes[i + 1][j] = gates[i][j]->update(nodes[i]);
    }
  }

  for (int i = 0; i < H; i++) {
    nodes[0][_I + _O + i] = nodes[nodes.size() - 1][I + O + i];
  }
}

void inline BitBrain::resetOutputs()
{
  for (int o = 0; o < _O; o++) {
    nodes[(int)nodes.size() - 1][o] = 0.0;
  }
}

string
BitBrain::description()
{
  string S = "Bit Brain";

  return S;
}

DataMap
BitBrain::getStats(string& prefix)
{
  DataMap dataMap;

  return dataMap;
}

void
BitBrain::initializeGenomes(
  unordered_map<string, shared_ptr<AbstractGenome>>& _genomes)
{
  _genomes[genomeName]->fillRandom();
}

shared_ptr<AbstractBrain>
BitBrain::makeCopy(shared_ptr<ParametersTable> PT_)
{
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  auto newBrain = make_shared<BitBrain>(nrInputValues, nrOutputValues, PT_);
  newBrain->H = H;
  newBrain->I = I;
  newBrain->O = O;
  newBrain->nodes = nodes;
  newBrain->gates = gates;
  return newBrain;
}

void
BitBrain::showBrain()
{
  // EMPTY
}