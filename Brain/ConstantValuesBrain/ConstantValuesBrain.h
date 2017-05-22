//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__ConstantValuesBrain__
#define __BasicMarkovBrainTemplate__ConstantValuesBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"


using namespace std;

class ConstantValuesBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<double>> valueMinPL;
	static shared_ptr<ParameterLink<double>> valueMaxPL;
	static shared_ptr<ParameterLink<int>> valueTypePL;
	static shared_ptr<ParameterLink<int>> samplesPerValuePL;

	static shared_ptr<ParameterLink<bool>> initializeUniformPL;
	static shared_ptr<ParameterLink<bool>> initializeConstantPL;
	static shared_ptr<ParameterLink<int>> initializeConstantValuePL;

	static shared_ptr<ParameterLink<string>> genomeNamePL;
	string genomeName;

	double valueMin;
	double valueMax;
	double valueType;
	int samplesPerValue;

	bool initializeUniform;
	bool initializeConstant;
	int initializeConstantValue;

	ConstantValuesBrain() = delete;

	ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~ConstantValuesBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;

	virtual unordered_set<string> requiredGenomes() override {
		return { genomeName };
	}

	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;

	virtual void resetBrain() override;
	virtual void resetOutputs() override;

	virtual void initalizeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes);
};

inline shared_ptr<AbstractBrain> ConstantValuesBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<ConstantValuesBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__ConstantValuesBrain__) */
