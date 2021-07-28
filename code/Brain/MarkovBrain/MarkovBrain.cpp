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


std::shared_ptr<ParameterLink<bool>> MarkovBrain::recurrentOutputPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-recurrentOutput", false,
    "if true, outputs will be copied like hidden at the end of each brain updata (or evaluation, if evaluationsPerBrainUpdate > 1) giving the brain access to it's prior outputs");

std::shared_ptr<ParameterLink<bool>> MarkovBrain::useGateRegulationPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-useGateRegulation", false,
    "if true, for each gate a pair of values will be read, the first will determin"
    "if the gate is always off, always on, or regulated. If regulated, the second"
    "value will determin the node address to use for regulation.");

std::shared_ptr<ParameterLink<bool>> MarkovBrain::useOutputThresholdPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-outputThresholdSwitch", false,
    "if true, each brain output will be divided by the number of connections to that output and compared to outputThreshold (if < outputThreshold then 0, else 1)");

std::shared_ptr<ParameterLink<double>> MarkovBrain::outputThresholdPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-outputThreshold", .5,
    "if useOutputThreshold is true, this value will be used to compare divided outputs");

std::shared_ptr<ParameterLink<bool>> MarkovBrain::useHiddenThresholdPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-hiddenThresholdSwitch", false,
    "if true, each brain hidden will be divided by the number of connections to that hidden and compared to hiddenThreshold (if < hiddenThreshold then 0, else 1)");

std::shared_ptr<ParameterLink<double>> MarkovBrain::hiddenThresholdPL =
Parameters::register_parameter(
    "BRAIN_MARKOV-hiddenThreshold", .5,
    "if useHiddenThreshold is true, this value will be used to compare divided hidden");

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

std::shared_ptr<ParameterLink<int>> MarkovBrain::evaluationsPreUpdatePL =
Parameters::register_parameter(
    "BRAIN_MARKOV-evaluationsPreUpdate", 1,
    "number of times brain will be evaluated (i.e. have all gates run and hidden cycled) per call to brain update");

std::shared_ptr<ParameterLink<int>> MarkovBrain::hiddenNodesPL =
    Parameters::register_parameter("BRAIN_MARKOV-hiddenNodes", 8,
                                   "number of hidden nodes");
std::shared_ptr<ParameterLink<std::string>> MarkovBrain::genomeNamePL =
    Parameters::register_parameter("BRAIN_MARKOV-genomeNameSpace",
                                   (std::string) "root::",
                                   "namespace used to set parameters for "
                                   "genome used to encode this brain");

void MarkovBrain::readParameters() {
    recurrentOutput = recurrentOutputPL->get(PT);
    useGateRegulation = useGateRegulationPL->get(PT);
  randomizeUnconnectedOutputs = randomizeUnconnectedOutputsPL->get(PT);
  randomizeUnconnectedOutputsType = randomizeUnconnectedOutputsTypePL->get(PT);
  randomizeUnconnectedOutputsMin = randomizeUnconnectedOutputsMinPL->get(PT);
  randomizeUnconnectedOutputsMax = randomizeUnconnectedOutputsMaxPL->get(PT);
  evaluationsPreUpdate = evaluationsPreUpdatePL->get(PT);

  useOutputThreshold = useOutputThresholdPL->get(PT);
  outputThreshold = outputThresholdPL->get(PT);
  useHiddenThreshold = useHiddenThresholdPL->get(PT);
  hiddenThreshold = hiddenThresholdPL->get(PT);

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

    if (!useGateRegulation) {
        gates = GLB->buildGateList(_genomes[genomeName], nrNodes, PT_);
    }
    else { // useGateRegulation
        std::vector<std::vector<int>> genomePerGateValues;
        int genomePerGateValuesCount = 2; // first value: is gate on, off or regulated? second value: address if regulated
        
        gates = GLB->buildGateListAndGetPerGateValues(_genomes[genomeName],
            nrNodes, _genomes[genomeName]->getAlphabetSize(),
            genomePerGateValues, genomePerGateValuesCount,
            PT_);

        // now that gates are constructed, determin which will be off, on, and regulated
        gateRegulationAdresses.clear();
        for (auto v : genomePerGateValues) {
            if (v[0] % 3 == 0){ // gate is always off 
                gateRegulationAdresses.push_back(-2);
                //std::cout << "found:-2 ";
            }
            else if (v[0] % 3 == 1) { // gate is always on 
                gateRegulationAdresses.push_back(-1);
                //std::cout << "found:-1 ";
            }
            else { // v[0] % 3 == 2; this is a regulated gate, pushback an address from inputs
                gateRegulationAdresses.push_back(v[1]%nrNodes);
                //std::cout << "node:"<< v[1] % nrNodes << " ";
            }
        }
        //std::cout << std::endl;
    }
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
    nextNodes.assign(nrNodes, 0.0);
    for (auto& g : gates) {
        g->resetGate();
    }
    /* done in Abstract::resetBrain
    resetInputs();
    resetOutputs();
    if (recordActivity) {
        if (lifeTimes.back() != 0) {
            lifeTimes.push_back(0);
        }
    }
    */
}

void MarkovBrain::resetInputs() {
  for (int i = 0; i < nrInputValues; i++) {
      inputValues[i] = 0.0;
  }
}

void MarkovBrain::resetOutputs() {
  for (int i = 0; i < nrOutputValues; i++) {
      outputValues[i] = 0.0;
  }
}


void MarkovBrain::update() {
    if (checkDet.size() == 0) {
        checkDet.resize(100);
    }
    //std::cout << "  in update..." << std::endl;
    std::vector<int> currentNextNodesConnections = nextNodesConnections;
    for (int eval = 0; eval < evaluationsPreUpdate; eval++) {

        for (int i = 0; i < nrInputValues; i++) { // copy inputs into nodes 
            nodes[i] = inputValues[i];
        }
        if (recurrentOutput) {
            for (int o = 0; o < outputValues.size(); o++) { // copy outputs into nodes if recurrent... else leave alone (i.e. they will be 0)
                nodes[o + nrInputValues] = outputValues[o];
            }
        }
        for (int h = nrInputValues + nrOutputValues; h < nodes.size(); h++) {// copy hidden from prior t+1
            nodes[h] = nextNodes[h];
        }

        nextNodes.assign(nrNodes, 0.0); // clear t+1 so it's ready to get new values

        if (recordActivity) {
            InputStates.push_back(std::vector<double>(nrInputValues)); // add space to the input state TS
            for (int i = 0; i < nrInputValues; i++) {
                InputStates.back()[i] = nodes[i]; // add input values
            }
            if (lifeTimes.back() == 0) { // if it's the first update of a new lifetime we need to add the current hidden and possibly the current output (if recurrent)
                HiddenStates.push_back(std::vector<double>(nodes.size() - (nrInputValues + nrOutputValues)));
                for (int i = 0; i < nodes.size() - (nrInputValues + nrOutputValues); i++) {
                    HiddenStates.back()[i] = nodes[i + nrInputValues + nrOutputValues];
                }
                if (recurrentOutput) {
                    OutputStates.push_back(std::vector<double>(nrOutputValues));
                    for (int i = 0; i < nrOutputValues; i++) {
                        OutputStates.back()[i] = nodes[nrInputValues + i];
                    }
                }
            }
        }

        if (!useGateRegulation) {
            for (auto& g : gates) {// update each gate
                g->update(nodes, nextNodes);
            }
        }
        else { //useGateRegulation
            std::fill(currentNextNodesConnections.begin(), currentNextNodesConnections.end(), 0);
            int gateCount = 0;
            for (auto& g : gates) {// update each gate
                // if -2, don't run the gate
                if (gateRegulationAdresses[gateCount] == -1) { // if -1 "always update"
                    g->update(nodes, nextNodes);
                    //std::cout << gateCount << ":A";
                }
                else if (gateRegulationAdresses[gateCount] >= 0) { // if >= 0 use this value as a node address

                    //std::cout << gateCount << ":V(" << gateRegulationAdresses[gateCount] << "/" << nodes[gateRegulationAdresses[gateCount]] << ")";
                    if (nodes[gateRegulationAdresses[gateCount]] > 0) {
                        g->update(nodes, nextNodes); // run the gate if the connected node is > 0
                        //std::cout << "* ";
                    }
                }
                for (auto out : g->outputs) {
                    currentNextNodesConnections[out]++;
                }
                gateCount++;
            }
            //std::cout << std::endl;

        }

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

        if (useOutputThreshold) {
            for (int i = 0; i < nrOutputValues; i++) {
                if (currentNextNodesConnections[nrInputValues + i] > 0) {
                    nextNodes[nrInputValues + i] = (nextNodes[nrInputValues + i] / currentNextNodesConnections[nrInputValues + i]) >= outputThreshold;
                }
            }
        }
        if (useHiddenThreshold) {
            for (int i = 0; i < hiddenNodes; i++) {
                if (currentNextNodesConnections[nrInputValues + nrOutputValues + i] > 0) {
                    nextNodes[nrInputValues + nrOutputValues + i] = (nextNodes[nrInputValues + nrOutputValues + i] / currentNextNodesConnections[nrInputValues + nrOutputValues + i]) >= hiddenThreshold;
                }
            }
        }

        //swap(nodes, nextNodes); OLD METHOD - here as a reminder of the old ways

        // if recordActivity, add output and hidden states
        if (recordActivity) {
            OutputStates.push_back(std::vector<double>(nrOutputValues));
            for (int i = 0; i < nrOutputValues; i++) {
                OutputStates.back()[i] = nextNodes[nrInputValues + i];
            }

            HiddenStates.push_back(std::vector<double>(nodes.size() - (nrInputValues + nrOutputValues)));
            for (int i = 0; i < nodes.size() - (nrInputValues + nrOutputValues); i++) {
                HiddenStates.back()[i] = nextNodes[i + nrInputValues + nrOutputValues];
            }
            lifeTimes.back()++;
        }

    }

    for (int i = 0; i < nrOutputValues; i++) {
        outputValues[i] = nextNodes[nrInputValues + i];
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
    temp.push_back(Trit(nodes[i]));
  
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

