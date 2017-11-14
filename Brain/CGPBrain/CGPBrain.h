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
#include "../../Genome/AbstractGenome.h"

#include "../AbstractBrain.h"


using namespace std;

class CGPBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<int>> hiddenNodesPL;
	//int nrHiddenValues;

	static shared_ptr<ParameterLink<string>> genomeNamePL;
	//string genomeName;

	static shared_ptr<ParameterLink<string>> availableOperatorsPL;
	vector<string> availableOperators;

	static shared_ptr<ParameterLink<double>> magnitudeMaxPL;
	static shared_ptr<ParameterLink<double>> magnitudeMinPL;
	//double magnitudeMax;
	//double magnitudeMin;

	static shared_ptr<ParameterLink<int>> numOpsPreVectorPL;
	//int numOpsPreVector;

	static shared_ptr<ParameterLink<string>> buildModePL;
	//string buildMode;

	static shared_ptr<ParameterLink<int>> codonMaxPL;
	//int codonMax;

	static shared_ptr<ParameterLink<bool>> readFromOutputsPL;
	//bool readFromOutputs;
	
	vector<double> readFromValues; // list of values that can be read from (inputs, outputs, hidden)
	vector<double> writeToValues; // list of values that can be written to (there will be this number of trees) (outputs, hidden)

	int nrInputTotal; // inputs + last outputs (maybe) + hidden
	int nrOutputTotal; // outputs + hidden

	map<string, int> allOps;
	vector<int> availableOps;
	int availableOpsCount;

	vector<vector<int>> brainVectors; // instruction sets (op,in1,in2)


	CGPBrain() = delete;

	CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	CGPBrain(int _nrInNodes, int _nrOutNodes, unordered_map<string, shared_ptr<AbstractGenome>>& _genomes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~CGPBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override {
		shared_ptr<CGPBrain> newBrain = make_shared<CGPBrain>(nrInputValues, nrOutputValues, _genomes, PT);
		return newBrain;
	}

	virtual unordered_set<string> requiredGenomes() override {
		return {genomeNamePL->get(PT) };
	}


	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "CGP";
	}

	virtual void resetBrain() override;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes);

};

inline shared_ptr<AbstractBrain> CGPBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<CGPBrain>(ins, outs, PT);
}
