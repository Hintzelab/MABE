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

class XorWorld : public AbstractWorld {

public:
	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
	static shared_ptr<ParameterLink<int>> brainUpdatesPL;
    int brainUpdates;
	string groupName;
	string brainName;
	
	XorWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~XorWorld() = default;
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyze, int visualize, int debug) override;
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze, visualize, debug);
		}
	}

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		// agents in this world will need 2 inputs, and 1 output:
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",2," + to_string(1) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}
};
