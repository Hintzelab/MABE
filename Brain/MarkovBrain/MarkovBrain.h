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

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "GateListBuilder/GateListBuilder.h"
#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

using namespace std;

class MarkovBrain : public AbstractBrain {

 public:
	vector<shared_ptr<AbstractGate>> gates;

//	static shared_ptr<ParameterLink<int>> bitsPerBrainAddressPL;  // how many bits are evaluated to determine the brain addresses.
//	static shared_ptr<ParameterLink<int>> bitsPerCodonPL;

	static shared_ptr<ParameterLink<bool>> randomizeUnconnectedOutputsPL;
	static shared_ptr<ParameterLink<int>> randomizeUnconnectedOutputsTypePL;
	static shared_ptr<ParameterLink<double>> randomizeUnconnectedOutputsMinPL;
	static shared_ptr<ParameterLink<double>> randomizeUnconnectedOutputsMaxPL;
	static shared_ptr<ParameterLink<int>> hiddenNodesPL;
	static shared_ptr<ParameterLink<string>> genomeNamePL;

	bool randomizeUnconnectedOutputs;
	bool randomizeUnconnectedOutputsType;
	double randomizeUnconnectedOutputsMin;
	double randomizeUnconnectedOutputsMax;
	int hiddenNodes;
	string genomeName;

	vector<double> nodes;
	vector<double> nextNodes;

	int nrNodes;

	shared_ptr<AbstractGateListBuilder> GLB;
	vector<int> nodesConnections, nextNodesConnections;

//	static bool& cacheResults;
//	static int& cacheResultsCount;

	//static void initializeParameters();
	vector<int> nodeMap;

	/*
	 * Builds a look up table to convert genome site values into brain state addresses - this is only used when there is a fixed number of brain states
	 * if there is a variable number of brain states, then the node map must be rebuilt.
	 */
	static int makeNodeMap(vector<int> & nodeMap, int sizeInBits, int defaultNrOfBrainStates) {
		for (int i = 0; i < pow(2, (sizeInBits)); i++) {  // each site in the genome has 8 bits so we need to count though (  2 to the (8 * number of sites)  )
			nodeMap.push_back(i % defaultNrOfBrainStates);
		}

		return 1;
	}

	MarkovBrain() = delete;

	MarkovBrain(vector<shared_ptr<AbstractGate>> _gates, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~MarkovBrain() = default;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	void readParameters();

	virtual void update() override;

	void inOutReMap();

	// Make a brain like the brain that called this function, using genomes and initalizing other elements.
	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual string description() override;
	void fillInConnectionsLists();
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "Markov";
	}

	virtual void resetBrain() override;
	virtual void resetOutputs()override;
	virtual void resetInputs() override;

	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();
	int numGates();
	
	vector<int> getHiddenNodes();

	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual unordered_set<string> requiredGenomes() override {
		return {genomeNamePL->get(PT)};
	}

};

inline shared_ptr<AbstractBrain> MarkovBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<MarkovBrain>(make_shared<ClassicGateListBuilder>(PT), ins, outs, PT);
}

