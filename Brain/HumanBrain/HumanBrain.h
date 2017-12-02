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

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

using namespace std;

class HumanBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<bool>> useActionMapPL;
	static shared_ptr<ParameterLink<string>> actionMapFileNamePL;

	bool useActionMap;
	string actionMapFileName;

	map<char, vector<double>> actionMap;
	map<char, string> actionNames;

	HumanBrain() = delete;

	HumanBrain(int _nrInNodes, int _nrOutNodes, shared_ptr<ParametersTable> _PT = Parameters::root);

	virtual ~HumanBrain() = default;

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrain(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes) override;
	virtual shared_ptr<AbstractBrain> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	//virtual shared_ptr<AbstractBrain> makeMutatedBrainFrom(shared_ptr<AbstractBrain> parent) override {
	//	//cout << "  in makeMutatedBrainFrom" << endl;
	//	return make_shared<HumanBrain>(nrInputValues, nrOutputValues, PT);
	//}

	//virtual shared_ptr<AbstractBrain> makeMutatedBrainFromMany(vector<shared_ptr<AbstractBrain>> parents) override {
	//	//cout << "  in makeMutatedBrainFromMany" << endl;
	//	return make_shared<HumanBrain>(nrInputValues, nrOutputValues, PT);
	//}

	virtual string description() override;
	virtual DataMap getStats(string& prefix) override;
	virtual string getType() override {
		return "Human";
	}

	virtual void resetBrain() override;

	virtual void initializeGenomes(unordered_map<string, shared_ptr<AbstractGenome>>& _genomes);
	
	virtual unordered_set<string> requiredGenomes() override {
		return {};
	}

};

inline shared_ptr<AbstractBrain> HumanBrain_brainFactory(int ins, int outs, shared_ptr<ParametersTable> PT = Parameters::root) {
	return make_shared<HumanBrain>(ins, outs, PT);
}
