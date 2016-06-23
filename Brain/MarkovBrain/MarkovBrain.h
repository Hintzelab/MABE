//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__MarkovBrain__
#define __BasicMarkovBrainTemplate__MarkovBrain__

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
 protected:
	vector<shared_ptr<AbstractGate>> gates;

 public:
	static shared_ptr<ParameterLink<bool>> randomizeUnconnectedOutputsPL;
	static shared_ptr<ParameterLink<int>> randomizeUnconnectedOutputsTypePL;
	static shared_ptr<ParameterLink<double>> randomizeUnconnectedOutputsMaxPL;

	bool randomizeUnconnectedOutputs;
	bool randomizeUnconnectedOutputsType;
	double randomizeUnconnectedOutputsMax;

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

	MarkovBrain(vector<shared_ptr<AbstractGate>> _gates, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT = nullptr);

	MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT = nullptr);
	MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT = nullptr);
	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~MarkovBrain() = default;

	virtual void update() override;

	void inOutReMap();

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	void fillInConnectionsLists();
	virtual vector<string> getStats() override;

	virtual void resetBrain() override;
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();
	int numGates();

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);

};

inline shared_ptr<AbstractBrain> MarkovBrain_brainFactory(int ins, int outs, int hidden, shared_ptr<ParametersTable> PT) {
	return make_shared<MarkovBrain>(make_shared<ClassicGateListBuilder>(PT), ins, outs, hidden, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__MarkovBrain__) */
