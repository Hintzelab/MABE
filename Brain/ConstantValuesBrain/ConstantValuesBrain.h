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

	double valueMin;
	double valueMax;
	double valueType;
	int samplesPerValue;

	bool initializeUniform;
	bool initializeConstant;
	int initializeConstantValue;

	ConstantValuesBrain() = delete;

	ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~ConstantValuesBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	virtual vector<string> getStats() override;

	virtual void resetBrain() override;
	virtual void resetOutputs() override;

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);
};

inline shared_ptr<AbstractBrain> ConstantValuesBrain_brainFactory(int ins, int outs, int hidden, shared_ptr<ParametersTable> PT) {
	return make_shared<ConstantValuesBrain>(ins, outs, hidden, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__ConstantValuesBrain__) */
