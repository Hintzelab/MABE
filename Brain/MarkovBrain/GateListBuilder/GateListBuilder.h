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

#include "../GateBuilder/GateBuilder.h"
#include "../../../Genome/AbstractGenome.h"

#include "../../../Utilities/Parameters.h"

using namespace std;
class AbstractGateListBuilder {

 public:
	const shared_ptr<ParametersTable> PT;
	Gate_Builder gateBuilder;


	AbstractGateListBuilder(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT), gateBuilder(PT) {
	}

//	AbstractGateListBuilder(shared_ptr<ParametersTable> _PT) : PT(_PT), AbstractGateListBuilder() {
//		gateBuilder.PT = PT;
//	}

	virtual ~AbstractGateListBuilder() = default;
//	virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
//	                                               int maxValue, shared_ptr<vector<int>> genomeHeadValues, int genomeHeadValuesCount,
//	                                               shared_ptr<vector<vector<int>>> genomePerGateValues, int genomePerGateValuesCount) = 0;

	virtual set<string> getInUseGateNames(){
		return gateBuilder.inUseGateNames;
	}
	virtual vector<shared_ptr<AbstractGate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, shared_ptr<ParametersTable> gatePT){
		vector<int> temp1;
		vector<vector<int>> temp2;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, 0, temp1, 0, temp2, 0, gatePT);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetHeadValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount, shared_ptr<ParametersTable> gatePT){
		vector<vector<int>> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, genomeHeadValues, genomeHeadValuesCount, temp, 0, gatePT);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetPerGateValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount, shared_ptr<ParametersTable> gatePT){
		vector<int> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, temp, 0, genomePerGateValues, genomePerGateValuesCount, gatePT);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetAllValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
            int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount,
            vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount, shared_ptr<ParametersTable> gatePT) = 0;

};

class ClassicGateListBuilder : public AbstractGateListBuilder {
 public:

//	ClassicGateListBuilder() {
//		gateBuilder.setupGates();
//	}
	ClassicGateListBuilder(shared_ptr<ParametersTable> _PT = nullptr) : AbstractGateListBuilder(_PT){
	}

	virtual ~ClassicGateListBuilder() = default;

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetAllValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
	                                               int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount,
	                                               vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount, shared_ptr<ParametersTable> gatePT);
};

