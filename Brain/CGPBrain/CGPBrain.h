//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__CGPBrain__
#define __BasicMarkovBrainTemplate__CGPBrain__

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
	vector<double> readFromValues; // list of values that can be read from (inputs, outputs, hidden)
	vector<double> writeToValues; // list of values that can be written to (there will be this number of trees) (outputs, hidden)

	int nrHiddenValues;
	bool readFromOutputs;

	int nrInputTotal; // inputs + last outputs (maybe) + hidden
	int nrOutputTotal; // outputs + hidden

	map<string, int> allOps;
	vector<int> availableOps;
	int availableOpsCount;

	vector<vector<int>> brainVectors; // instruction sets (op,in1,in2)
	int numOpsPreVector = 10;

	double magnitudeMax;
	double magnitudeMin;

	CGPBrain() = delete;

	CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);
	CGPBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<AbstractGenome> _genome, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~CGPBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override {
		shared_ptr<CGPBrain> newBrain = make_shared<CGPBrain>(nrInputValues, nrOutputValues, _genome, PT);
		return newBrain;
	}

	virtual bool requireGenome() {
		return true;
	}

	virtual string description() override;
	virtual DataMap getStats() override;

	virtual void resetBrain() override;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);

};

inline shared_ptr<AbstractBrain> CGPBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<CGPBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__CGPBrain__) */
