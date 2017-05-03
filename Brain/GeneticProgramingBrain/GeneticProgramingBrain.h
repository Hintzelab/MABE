//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__GeneticProgrammingBrain__
#define __BasicMarkovBrainTemplate__GeneticProgrammingBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Utilities/Random.h"
#include "../../Utilities/MTree.h"

#include "../AbstractBrain.h"


using namespace std;

class GeneticProgramingBrain: public AbstractBrain {
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

	GeneticProgramingBrain() = delete;

	GeneticProgramingBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~GeneticProgramingBrain() = default;

	shared_ptr<Abstract_MTree> makeTree(vector<string> nodeTypes, int depth, int maxDepth);

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override {
		cout << "  Attempt to makeBrainFromGenome for GeneticProgramming Brain. GeneticProgrammingBrains are not built from Genomes!\n  Exiting." << endl;
		exit(1);
	}

	virtual bool requireGenome() {
		return false;
	}

	virtual shared_ptr<AbstractBrain> makeMutatedBrainFrom(shared_ptr<AbstractBrain> parent) override;
	virtual shared_ptr<AbstractBrain> makeMutatedBrainFromMany(vector<shared_ptr<AbstractBrain>> parents) override;

	virtual string description() override;
	virtual DataMap getStats() override;

	virtual void resetBrain() override;

	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

inline shared_ptr<AbstractBrain> GeneticProgramingBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT) {
	return make_shared<GeneticProgramingBrain>(ins, outs, PT);
}

#endif /* defined(__BasicMarkovBrainTemplate__cwGeneticProgramingBrain__) */
