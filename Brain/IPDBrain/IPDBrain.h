//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__IPDBrain__
#define __BasicMarkovBrainTemplate__IPDBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"


using namespace std;

class IPDBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<string>> availableStrategiesPL;
//	static shared_ptr<ParameterLink<double>> valueMaxPL;
//	static shared_ptr<ParameterLink<int>> valueTypePL;
//	static shared_ptr<ParameterLink<int>> samplesPerValuePL;
//
//	static shared_ptr<ParameterLink<bool>> initializeUniformPL;
//	static shared_ptr<ParameterLink<bool>> initializeConstantPL;
//	static shared_ptr<ParameterLink<int>> initializeConstantValuePL;
//
//	double valueMin;
//	double valueMax;
//	double valueType;
//	int samplesPerValue;
//
//	bool initializeUniform;
//	bool initializeConstant;
//	int initializeConstantValue;

	vector<string> availableStrategies;
	string strategy;
	vector<double> movesSelf;
	vector<double> movesOther;
	vector<double> internalValues; // can be used by individual strategies as needed

	IPDBrain() = delete;

	IPDBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~IPDBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	virtual DataMap getStats() override;

	virtual void resetBrain() override;
	//virtual void resetOutputs() override;

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);
};

inline shared_ptr<AbstractBrain> IPDBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<IPDBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__IPDBrain__) */
