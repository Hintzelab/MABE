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

class TestWorld : public AbstractWorld {

public:

	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	//int mode;
	//int numberOfOutputs;
	//int evaluationsPerGeneration;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	//string brainName;

	TestWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~TestWorld() = default;


	virtual void evaluateSolo(shared_ptr<Organism> org, int analyze, int visualize, int debug);
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze, visualize, debug);
		}
	}

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",1," + to_string(numberOfOutputsPL->get(PT)) } } };
		// requires a root group and a brain (in root namespace) and no addtional genome,
		// the brain must have 1 input, and the variable numberOfOutputs outputs
	}


};

