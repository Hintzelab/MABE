//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldTest__
#define __BasicMarkovBrainTemplate__WorldTest__

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

	int mode;
	int numberOfOutputs;
	int evaluationsPerGeneration;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;

	TestWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~TestWorld() = default;

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupName,{ "B:" + brainName + ",1," + to_string(numberOfOutputs) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}


};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
