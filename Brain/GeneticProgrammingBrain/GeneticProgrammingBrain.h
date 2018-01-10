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

#include "../../Utilities/Random.h"
#include "../../Utilities/MTree.h"

#include "../AbstractBrain.h"


using namespace std;

class GeneticProgrammingBrain: public AbstractBrain {
public:

	//static shared_ptr<ParameterLink<double>> valueMinPL;
	//static shared_ptr<ParameterLink<double>> valueMaxPL;
	//static shared_ptr<ParameterLink<int>> valueTypePL;
	//static shared_ptr<ParameterLink<int>> samplesPerValuePL;

	//static shared_ptr<ParameterLink<bool>> initializeUniformPL;
	//static shared_ptr<ParameterLink<bool>> initializeConstantPL;
	//static shared_ptr<ParameterLink<int>> initializeConstantValuePL;

	//double valueMin;
	//double valueMax;
	//double valueType;
	//int samplesPerValue;

	//bool initializeUniform;
	//bool initializeConstant;
	//int initializeConstantValue;

	vector<shared_ptr<Abstract_MTree>> trees; // one tree for each output and hidden value
	vector<double> readFromValues; // list of values that can be read from (inputs, outputs, hidden)
	vector<double> writeToValues; // list of values that can be written to (there will be this number of trees) (outputs, hidden)

	int nrHiddenValues;
	bool readFromOutputs;
	bool useHidden;

	int initialTreeDepth;
	vector<string> nodeTypes;

	double magnitudeMax;
	double magnitudeMin;

	GeneticProgrammingBrain() = delete;

	GeneticProgrammingBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~GeneticProgrammingBrain() = default;

	shared_ptr<Abstract_MTree> makeTree(vector<string> nodeTypes, int depth, int maxDepth);

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual shared_ptr<AbstractBrain> makeBrainFrom(shared_ptr<AbstractBrain> parent, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;
	virtual shared_ptr<AbstractBrain> makeBrainFromMany(vector<shared_ptr<AbstractBrain>> parents, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "GeneticProgramming";
	}

	virtual DataMap serialize(string& name) override;
	virtual void deserialize(shared_ptr<ParametersTable> PT, unordered_map<string, string>& orgData, string& name) override;

	virtual void resetBrain() override;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	virtual unordered_set<string> requiredGenomes() override {
		return {};
	}

};

inline shared_ptr<AbstractBrain> GeneticProgrammingBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<GeneticProgrammingBrain>(ins, outs, PT);
}
