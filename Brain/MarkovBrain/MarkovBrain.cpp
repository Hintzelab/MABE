//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "MarkovBrain.h"

shared_ptr<ParameterLink<bool>> MarkovBrain::randomizeUnconnectedOutputsPL = Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputs", false, "output nodes with no connections will be set randomly (default : false, behavior set to 0)");
shared_ptr<ParameterLink<int>> MarkovBrain::randomizeUnconnectedOutputsTypePL = Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsType", 0, "determines type of values resulting from randomizeUnconnectedOutput [0 = int, 1 = double]");
shared_ptr<ParameterLink<double>> MarkovBrain::randomizeUnconnectedOutputsMaxPL = Parameters::register_parameter("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsMax", 1.0, "random values resulting from randomizeUnconnectedOutput will be in the range of 0 to randomizeUnconnectedOutputsMax");

MarkovBrain::MarkovBrain(vector<shared_ptr<AbstractGate>> _gates, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes, _PT) {

	randomizeUnconnectedOutputs = (PT == nullptr) ? randomizeUnconnectedOutputsPL->lookup() : PT->lookupBool("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputs");
	randomizeUnconnectedOutputsType = (PT == nullptr) ? randomizeUnconnectedOutputsTypePL->lookup() : PT->lookupInt("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsType");
	randomizeUnconnectedOutputsMax = (PT == nullptr) ? randomizeUnconnectedOutputsMaxPL->lookup() : PT->lookupDouble("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsMax");

	GLB = nullptr;
	gates = _gates;
	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("gates");
//	for (auto name : Gate_Builder::inUseGateNames) {
	for (auto name : GLB->getInUseGateNames()) {
		aveFileColumns.push_back(name + "Gates");
	}
	nodesConnections.resize(nrOfBrainNodes, 0);
	nextNodesConnections.resize(nrOfBrainNodes, 0);
	fillInConnectionsLists();
}

MarkovBrain::MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes, _PT) {
	GLB = _GLB;
	//make a node map to handle genome value to brain state address look up.

	randomizeUnconnectedOutputs = (PT == nullptr) ? randomizeUnconnectedOutputsPL->lookup() : PT->lookupBool("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputs");
	randomizeUnconnectedOutputsType = (PT == nullptr) ? randomizeUnconnectedOutputsTypePL->lookup() : PT->lookupInt("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsType");
	randomizeUnconnectedOutputsMax = (PT == nullptr) ? randomizeUnconnectedOutputsMaxPL->lookup() : PT->lookupDouble("BRAIN_MARKOV_ADVANCED-randomizeUnconnectedOutputsMax");

	makeNodeMap(nodeMap, Global::bitsPerBrainAddressPL->lookup(), nrOfBrainNodes);

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("gates");
	for (auto name : GLB->getInUseGateNames()) {
		aveFileColumns.push_back(name + "Gates");
	}
	nodesConnections.resize(nrOfBrainNodes, 0);
	nextNodesConnections.resize(nrOfBrainNodes, 0);
}

MarkovBrain::MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT) :
		MarkovBrain(_GLB, _nrInNodes, _nrOutNodes, _nrHiddenNodes, _PT) {
	//cout << "in MarkovBrain::MarkovBrain(shared_ptr<Base_GateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrOfBrainStates)\n\tabout to - gates = GLB->buildGateList(genome, nrOfBrainStates);" << endl;
	gates = GLB->buildGateList(genome, nrOfBrainNodes, _PT);
	inOutReMap();  // map ins and outs from genome values to brain states
	fillInConnectionsLists();
}

shared_ptr<AbstractBrain> MarkovBrain::makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
	shared_ptr<MarkovBrain> newBrain = make_shared<MarkovBrain>(GLB, _genome, nrInNodes, nrOutNodes, nrHiddenNodes, PT);
	return newBrain;
}

void MarkovBrain::resetBrain() {
	AbstractBrain::resetBrain();
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->resetGate();
	}
}

void MarkovBrain::update() {
	nextNodes.assign(nrOfBrainNodes, 0.0);
	for (size_t i = 0; i < gates.size(); i++) {  //update each gate
		gates[i]->update(nodes, nextNodes);
	}
	if (randomizeUnconnectedOutputs) {
		if (randomizeUnconnectedOutputsType == 0) {
			for (int i = 0; i < nrOutNodes; i++) {
				if (nextNodesConnections[outputNodesList[i]] == 0) {
					nextNodes[outputNodesList[i]] = Random::getInt(0, (int) randomizeUnconnectedOutputsMax);
				}
			}
		} else if (randomizeUnconnectedOutputsType == 1) {
			for (int i = 0; i < nrOutNodes; i++) {
				if (nextNodesConnections[outputNodesList[i]] == 0) {
					nextNodes[outputNodesList[i]] = Random::getDouble(0, randomizeUnconnectedOutputsMax);
				}
			}
		} else {
			cout << "  ERROR! BRAIN_MARKOV_ADVANCED::randomizeUnconnectedOutputsType is invalid. current value: " << randomizeUnconnectedOutputsType << endl;
			exit(1);
		}
	}
	swap(nodes, nextNodes);
}

void MarkovBrain::inOutReMap() {  // remaps genome site values to valid brain state addresses
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->applyNodeMap(nodeMap, nrOfBrainNodes);
	}

}

string MarkovBrain::description() {
	string S = "Markov Briain\n" + gateList();
	return S;
}

void MarkovBrain::fillInConnectionsLists() {
	//nodesConnections.resize(nrOfBrainNodes);
	//nextNodesConnections.resize(nrOfBrainNodes);
	for (auto g : gates) {
		auto gateConnections = g->getConnectionsLists();
		for (auto c : gateConnections.first) {
			nodesConnections[c]++;
		}
		for (auto c : gateConnections.second) {
			nextNodesConnections[c]++;
		}
	}
}
vector<string> MarkovBrain::getStats() {
	vector<string> dataPairs;

	dataPairs.push_back("gates");
	dataPairs.push_back(to_string(gates.size()));

	map<string, int> gatecounts;
	for (auto n : GLB->getInUseGateNames()) {
		gatecounts[n + "Gates"] = 0;
	}
	for (auto g : gates) {
		//auto gateConnections = g->getConnectionsLists();
		//for (auto c : gateConnections.first){
		//	nodesConnections[c]++;
		//}
		//for (auto c : gateConnections.second){
		//	nextNodesConnections[c]++;
		//}
		gatecounts[g->gateType() + "Gates"]++;
	}

	string nodesConnectionsString = "\"[";
	string nextNodesConnectionsString = "\"[";

	for (int i = 0; i < nrOfBrainNodes; i++) {
		nodesConnectionsString += to_string(nodesConnections[i]) + ",";
		nextNodesConnectionsString += to_string(nextNodesConnections[i]) + ",";
	}
	nodesConnectionsString.pop_back();
	nodesConnectionsString += "]\"";
	dataPairs.push_back("nodesConnections");
	dataPairs.push_back(nodesConnectionsString);
	nextNodesConnectionsString.pop_back();
	nextNodesConnectionsString += "]\"";
	dataPairs.push_back("nextNodesConnections");
	dataPairs.push_back(nextNodesConnectionsString);

	for (auto n : GLB->getInUseGateNames()) {
		dataPairs.push_back(n + "Gates");
		dataPairs.push_back(to_string(gatecounts[n + "Gates"]));
	}
	return (dataPairs);
}

string MarkovBrain::gateList() {
	string S = "";
	for (size_t i = 0; i < gates.size(); i++) {
		S = S + gates[i]->description();
	}
	return S;
}

vector<vector<int>> MarkovBrain::getConnectivityMatrix() {
	vector<vector<int>> M;
	M.resize(nrOfBrainNodes);
	for (int i = 0; i < nrOfBrainNodes; i++) {
		M[i].resize(nrOfBrainNodes);
		for (int j = 0; j < nrOfBrainNodes; j++)
			M[i][j] = 0;
	}
	for (auto G : gates) {
		vector<int> I = G->getIns();
		vector<int> O = G->getOuts();
		for (int i : I)
			for (int o : O)
				M[i][o]++;
	}
	return M;
}

int MarkovBrain::brainSize() {
	return (int) gates.size();
}

int MarkovBrain::numGates() {
	return brainSize();
}

void MarkovBrain::initalizeGenome(shared_ptr<AbstractGenome> _genome) {
	int codonMax = (1 << Global::bitsPerCodonPL->lookup()) - 1;
	_genome->fillRandom();

	auto genomeHandler = _genome->newHandler(_genome);

	for (auto gateType : GLB->gateBuilder.inUseGateTypes) {
		//cout << gateType << " : "<<Gate_Builder::intialGateCounts[gateType]<<endl;
		for (int i = 0; i < GLB->gateBuilder.intialGateCounts[gateType]; i++) {
			//cout << "    " << i << " : ";
			genomeHandler->randomize();
			for (auto value : GLB->gateBuilder.gateStartCodes[gateType]) {
				//cout << value << " ";
				genomeHandler->writeInt(value, 0, codonMax);
			}
			//cout << endl;
		}
	}
}

shared_ptr<AbstractBrain> MarkovBrain::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	vector<shared_ptr<AbstractGate>> _gates; 
	for (auto gate : gates) {
		_gates.push_back(gate->makeCopy());
	}
	auto newBrain = make_shared<MarkovBrain>(_gates, nrInNodes, nrOutNodes, nrHiddenNodes,_PT);
	
	return newBrain;
}
