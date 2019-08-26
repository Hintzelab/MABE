//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "MarkovBrain.h"

std::shared_ptr<ParameterLink<bool>> MarkovBrain::recordIOMapPL=
    Parameters::register_parameter(
        "BRAIN_MARKOV_ADVANCED-recordIOMap", false,
        "if true, all inoput output and hidden nodes will be recorderd on every brain update");
std::shared_ptr<ParameterLink<std::string>> MarkovBrain::IOMapFileNamePL=
    Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-recordIOMap_fileName",
                                   (std::string) "markov_IO_map.csv",
                                   "Name of file where IO mappings are saved");
std::shared_ptr<ParameterLink<bool>> MarkovBrain::randomizeUnconnectedOutputsPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputs", false,
        "output nodes with no connections will be set randomly (default : "
        "false, behavior set to 0)");
std::shared_ptr<ParameterLink<int>> MarkovBrain::randomizeUnconnectedOutputsTypePL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsType", 0,
        "determines type of values resulting from randomizeUnconnectedOutput "
        "0 = int, 1 = double");
std::shared_ptr<ParameterLink<double>>
    MarkovBrain::randomizeUnconnectedOutputsMinPL =
        Parameters::register_parameter(
            "BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsMin", 0.0,
            "random values resulting from randomizeUnconnectedOutput will be "
            "in the range of randomizeUnconnectedOutputsMin to "
            "randomizeUnconnectedOutputsMax");
std::shared_ptr<ParameterLink<double>>
    MarkovBrain::randomizeUnconnectedOutputsMaxPL =
        Parameters::register_parameter(
            "BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsMax", 1.0,
            "random values resulting from randomizeUnconnectedOutput will be "
            "in the range of randomizeUnconnectedOutputsMin to "
            "randomizeUnconnectedOutputsMax");
std::shared_ptr<ParameterLink<int>> MarkovBrain::hiddenNodesPL =
    Parameters::register_parameter("BRAIN_MARKOV-hiddenNodes", 8,
                                   "number of hidden nodes");
std::shared_ptr<ParameterLink<std::string>> MarkovBrain::genomeNamePL =
    Parameters::register_parameter("BRAIN_MARKOV-genomeNameSpace",
                                   (std::string) "root::",
                                   "namespace used to set parameters for "
                                   "genome used to encode this brain");

void MarkovBrain::readParameters() {
  randomizeUnconnectedOutputs = randomizeUnconnectedOutputsPL->get(PT);
  randomizeUnconnectedOutputsType = randomizeUnconnectedOutputsTypePL->get(PT);
  randomizeUnconnectedOutputsMin = randomizeUnconnectedOutputsMinPL->get(PT);
  randomizeUnconnectedOutputsMax = randomizeUnconnectedOutputsMaxPL->get(PT);
  hiddenNodes = hiddenNodesPL->get(PT);

  genomeName = genomeNamePL->get(PT);

  nrNodes = nrInputValues + nrOutputValues + hiddenNodes;
  nodes.resize(nrNodes, 0);
  nextNodes.resize(nrNodes, 0);
}

MarkovBrain::MarkovBrain(std::vector<std::shared_ptr<AbstractGate>> _gates,
                         int _nrInNodes, int _nrOutNodes,
                         std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {

  readParameters();

  // GLB = nullptr;
  GLB = std::make_shared<ClassicGateListBuilder>(PT);
  gates = _gates;
  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("markovBrainGates");
  for (auto name : GLB->getInUseGateNames()) {
    popFileColumns.push_back("markovBrain" + name + "Gates");
  }

  fillInConnectionsLists();
}

MarkovBrain::MarkovBrain(std::shared_ptr<AbstractGateListBuilder> GLB_,
                         int _nrInNodes, int _nrOutNodes,
                         std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {
  GLB = GLB_;
  // make a node map to handle genome value to brain state address look up.

  readParameters();

  makeNodeMap(nodeMap, Gate_Builder::bitsPerBrainAddressPL->get(), nrNodes);

  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("markovBrainGates");
  popFileColumns.push_back("markovBrainGates_VAR");
  for (auto name : GLB->getInUseGateNames()) {
    popFileColumns.push_back("markovBrain" + name + "Gates");
  }

  fillInConnectionsLists();
}

MarkovBrain::MarkovBrain(
    std::shared_ptr<AbstractGateListBuilder> GLB_,
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes, int _nrInNodes,
    int _nrOutNodes, std::shared_ptr<ParametersTable> PT_)
    : MarkovBrain(GLB_, _nrInNodes, _nrOutNodes, PT_) {
  // cout << "in MarkovBrain::MarkovBrain(std::shared_ptr<Base_GateListBuilder> GLB_,
  // std::shared_ptr<AbstractGenome> genome, int _nrOfBrainStates)\n\tabout to -
  // gates = GLB->buildGateList(genome, nrOfBrainStates);" << endl;
  gates = GLB->buildGateList(_genomes[genomeName], nrNodes, PT_);
  inOutReMap(); // map ins and outs from genome values to brain states
  fillInConnectionsLists();
}

// Make a brain like the brain that called this function, using genomes and
// initalizing other elements.
std::shared_ptr<AbstractBrain> MarkovBrain::makeBrain(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
  std::shared_ptr<MarkovBrain> newBrain = std::make_shared<MarkovBrain>(
      GLB, _genomes, nrInputValues, nrOutputValues, PT);
  return newBrain;
}

void MarkovBrain::resetBrain() {
  AbstractBrain::resetBrain();
  nodes.assign(nrNodes, 0.0);
  for (auto &g :gates)
	  g->resetGate();
}

void MarkovBrain::resetInputs() {
  AbstractBrain::resetInputs(); 
  for (int i = 0; i < nrInputValues; i++)
    nodes[i] = 0.0;
}

void MarkovBrain::resetOutputs() {
  AbstractBrain::resetOutputs();
  // note nrInputValues+i gets us the index for the node related to each output
  for (int i = 0; i < nrOutputValues; i++) 
    nodes[nrInputValues + i] = 0.0;
}


void MarkovBrain::update() {
  nextNodes.assign(nrNodes, 0.0);
	DataMap IOMap;

  for (int i = 0; i < nrInputValues; i++)  
    nodes[i] = inputValues[i];
  
  if (recordIOMapPL->get())
    for (int i = 0; i < nrInputValues; i++)
     IOMap.append("input", Bit(nodes[i]));

  for (auto &g :gates) // update each gate
	  g->update(nodes, nextNodes);

  if (randomizeUnconnectedOutputs) {
    switch (randomizeUnconnectedOutputsType) {
    case 0:
      for (int i = 0; i < nrOutputValues; i++) 
        if (nextNodesConnections[nrInputValues + i] == 0) 
          nextNodes[nrInputValues + i] =
              Random::getInt((int)randomizeUnconnectedOutputsMin,
                             (int)randomizeUnconnectedOutputsMax);
      break;
    case 1:
      for (int i = 0; i < nrOutputValues; i++) 
        if (nextNodesConnections[nrInputValues + i] == 0) 
          nextNodes[nrInputValues + i] = Random::getDouble(
              randomizeUnconnectedOutputsMin, randomizeUnconnectedOutputsMax);
      //break;
    //default:
      //std::cout
      //    << "  ERROR! BRAIN_MARKOV_ADVANCED::randomizeUnconnectedOutputsType "
      //       "is invalid. current value: "
      //    << randomizeUnconnectedOutputsType << std::endl;
      //exit(1);
    }
  }
  swap(nodes, nextNodes);
  for (int i = 0; i < nrOutputValues; i++) {
    outputValues[i] = nodes[nrInputValues + i];
  }

  if (recordIOMapPL->get()){
   for (int i = 0; i < nrOutputValues; i++ )
      IOMap.append("output", Bit(nodes[nrInputValues + i]));
	
   for (int i = nrInputValues + nrOutputValues ; i < nodes.size() ; i++) 
	   IOMap.append("hidden", Bit(nodes[i]));
   IOMap.setOutputBehavior("input", DataMap::LIST);
   IOMap.setOutputBehavior("output", DataMap::LIST);
   IOMap.setOutputBehavior("hidden", DataMap::LIST);
   IOMap.writeToFile(IOMapFileNamePL->get());
  IOMap.clearMap();
  }
}

void MarkovBrain::inOutReMap() { // remaps genome site values to valid brain
                                 // state addresses
  for (auto &g : gates)
    g->applyNodeMap(nodeMap, nrNodes);
}

std::string MarkovBrain::description() {
  std::string S = "Markov Briain\nins:" + std::to_string(nrInputValues) + " outs:" +
             std::to_string(nrOutputValues) + " hidden:" + std::to_string(hiddenNodes) +
             "\n" + gateList();
  return S;
}

void MarkovBrain::fillInConnectionsLists() {
  nodesConnections.resize(nrNodes);
  nextNodesConnections.resize(nrNodes);
  for (auto &g : gates) {
    auto gateConnections = g->getConnectionsLists();
    for (auto c : gateConnections.first) 
      nodesConnections[c]++;
    for (auto c : gateConnections.second) 
      nextNodesConnections[c]++;
  }
}


DataMap MarkovBrain::getStats(std::string &prefix) {
  DataMap dataMap;
  dataMap.set(prefix + "markovBrainGates", (int)gates.size());
  std::map<std::string, int> gatecounts;
  for (auto &n : GLB->getInUseGateNames()) {
    gatecounts[n + "Gates"] = 0;
  }
  for (auto &g : gates) {
    gatecounts[g->gateType() + "Gates"]++;
  }

  for (auto &n : GLB->getInUseGateNames()) {
    dataMap.set(prefix + "markovBrain" + n + "Gates", gatecounts[n + "Gates"]);
  }

  std::vector<int> nodesConnectionsList;
  std::vector<int> nextNodesConnectionsList;

  for (int i = 0; i < nrNodes; i++) {
    nodesConnectionsList.push_back(nodesConnections[i]);
    nextNodesConnectionsList.push_back(nextNodesConnections[i]);
  }
  dataMap.set(prefix + "markovBrain_nodesConnections", nodesConnectionsList);
  dataMap.setOutputBehavior(prefix + "markovBrain_nodesConnections", DataMap::LIST);
  dataMap.set(prefix + "markovBrain_nextNodesConnections",
              nextNodesConnectionsList);
  dataMap.setOutputBehavior(prefix + "markovBrain_nextNodesConnections", DataMap::LIST);

  return dataMap;
}

std::string MarkovBrain::gateList() {
  std::string S = "";
  for (auto &g : gates)
    S += g->description();
  return S;
}

std::vector<std::vector<int>> MarkovBrain::getConnectivityMatrix() {
  std::vector<std::vector<int>> M(nrNodes,std::vector<int>(nrNodes,0));
  for (auto &g : gates) {
    auto I = g->getIns();
    auto O = g->getOuts();
    for (int i : I)
      for (int o : O)
        M[i][o]++;
  }
  return M;
}

int MarkovBrain::brainSize() { return gates.size(); }

int MarkovBrain::numGates() { return brainSize(); }

std::vector<int> MarkovBrain::getHiddenNodes() {
  std::vector<int> temp ;
  for (size_t i = nrInputValues + nrOutputValues; i < nodes.size(); i++) 
    temp.push_back(Bit(nodes[i]));
  
  return temp;
}

void MarkovBrain::initializeGenomes(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
  int codonMax = (1 << Gate_Builder::bitsPerCodonPL->get()) - 1;
  _genomes[genomeName]->fillRandom();

  auto genomeHandler = _genomes[genomeName]->newHandler(_genomes[genomeName]);

  for (auto gateType : GLB->gateBuilder.inUseGateTypes) {
    for (int i = 0; i < GLB->gateBuilder.intialGateCounts[gateType]; i++) {
      genomeHandler->randomize();
      for (auto value : GLB->gateBuilder.gateStartCodes[gateType]) 
        genomeHandler->writeInt(value, 0, codonMax);
    }
  }
}

std::shared_ptr<AbstractBrain>
MarkovBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  std::vector<std::shared_ptr<AbstractGate>> _gates;
  for (auto gate : gates) {
    _gates.push_back(gate->makeCopy());
  }
  auto newBrain =
      std::make_shared<MarkovBrain>(_gates, nrInputValues, nrOutputValues, PT_);
  return newBrain;
}

std::vector<std::shared_ptr<AbstractBrain>> MarkovBrain::getAllSingleGateKnockouts() {
  std::vector<std::shared_ptr<AbstractBrain>> res;
  auto numg = gates.size();
  if (!numg) std::cout <<"No gates?" << std::endl;
  for (int i = 0; i < numg; i++) {
    std::vector<std::shared_ptr<AbstractGate>> gmut;
    int c = 0;
    for (auto g : gates)
      if (c++ != i)
        gmut.push_back(g->makeCopy());
    auto bmut =
        std::make_shared<MarkovBrain>(gmut, nrInputValues, nrOutputValues, PT);
    res.push_back(bmut);
  }
  return res;
}

