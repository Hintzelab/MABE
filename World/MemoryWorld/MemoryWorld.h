//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldTest__
#define __BasicMarkovBrainTemplate__WorldTest__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include <string>

using namespace std;

class MemoryWorld : public AbstractWorld {

public:

	static shared_ptr<ParameterLink<string>> maskPL;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
	static shared_ptr<ParameterLink<int>> worldUpdatesPL;

	static shared_ptr<ParameterLink<int>> refreshPasswordPL;
	static shared_ptr<ParameterLink<int>> refreshMaskPL;
	int refreshPassword;
	int refreshMask;


	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	string brainName;


    vector<int> rawMask;
	vector<int> mask;
	vector<int> password;
	int currentUpdate = -1;

	int nOut = 0;

	int evaluationsPerGeneration;
    
    int worldUpdates;

	MemoryWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~MemoryWorld() = default;

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;
	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupName,{ "B:" + brainName + "," + to_string(requiredInputs()) + "," + to_string(requiredOutputs()) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
