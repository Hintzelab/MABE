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

#include <Brain/MarkovBrain/GateBuilder/GateBuilder.h>
#include <Genome/AbstractGenome.h>
#include <Utilities/Parameters.h>

class AbstractGateListBuilder {

 public:
	const std::shared_ptr<ParametersTable> PT;
	Gate_Builder gateBuilder;


	AbstractGateListBuilder(std::shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT), gateBuilder(PT) {
	}

//	AbstractGateListBuilder(shared_ptr<ParametersTable> _PT) : PT(_PT), AbstractGateListBuilder() {
//		gateBuilder.PT = PT;
//	}

	virtual ~AbstractGateListBuilder() = default;
//	virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
//	                                               int maxValue, shared_ptr<vector<int>> genomeHeadValues, int genomeHeadValuesCount,
//	                                               shared_ptr<vector<vector<int>>> genomePerGateValues, int genomePerGateValuesCount) = 0;

	virtual std::set<std::string> getInUseGateNames(){
		return gateBuilder.inUseGateNames;
	}
	virtual std::vector<std::shared_ptr<AbstractGate>> buildGateList(std::shared_ptr<AbstractGenome> genome, int nrOfBrainStates, std::shared_ptr<ParametersTable> gatePT){
		std::vector<int> temp1;
		std::vector<std::vector<int>> temp2;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, 0, temp1, 0, temp2, 0, gatePT);
	}

	virtual std::vector<std::shared_ptr<AbstractGate>> buildGateListAndGetHeadValues(std::shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, std::vector<int> &genomeHeadValues, int genomeHeadValuesCount, std::shared_ptr<ParametersTable> gatePT){
		std::vector<std::vector<int>> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, genomeHeadValues, genomeHeadValuesCount, temp, 0, gatePT);
	}

	virtual std::vector<std::shared_ptr<AbstractGate>> buildGateListAndGetPerGateValues(std::shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, std::vector<std::vector<int>> &genomePerGateValues, int genomePerGateValuesCount, std::shared_ptr<ParametersTable> gatePT){
		std::vector<int> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, temp, 0, genomePerGateValues, genomePerGateValuesCount, gatePT);
	}

	virtual std::vector<std::shared_ptr<AbstractGate>> buildGateListAndGetAllValues(std::shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
            int maxValue, std::vector<int> &genomeHeadValues, int genomeHeadValuesCount,
            std::vector<std::vector<int>> &genomePerGateValues, int genomePerGateValuesCount, std::shared_ptr<ParametersTable> gatePT) = 0;

};

class ClassicGateListBuilder : public AbstractGateListBuilder {
 public:

//	ClassicGateListBuilder() {
//		gateBuilder.setupGates();
//	}
	ClassicGateListBuilder(std::shared_ptr<ParametersTable> _PT = nullptr) : AbstractGateListBuilder(_PT){
	}

	virtual ~ClassicGateListBuilder() = default;

	virtual std::vector<std::shared_ptr<AbstractGate>> buildGateListAndGetAllValues(std::shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
	                                               int maxValue, std::vector<int> &genomeHeadValues, int genomeHeadValuesCount,
	                                               std::vector<std::vector<int>> &genomePerGateValues, int genomePerGateValuesCount, std::shared_ptr<ParametersTable> gatePT);
};

