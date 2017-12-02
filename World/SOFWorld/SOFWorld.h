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

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class SOFWorld : public AbstractWorld {

public:
	
	//parameters
	static shared_ptr<ParameterLink<string>> scoreMapFilenamePL;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;

	vector<double> scoreMatrix;
	int x, y;

	
	SOFWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~SOFWorld() = default;
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse, visualize, debug);
		}
	}
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",1,2" } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}


};

