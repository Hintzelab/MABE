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

class RPPWorld : public AbstractWorld {

public:

	//static shared_ptr<ParameterLink<int>> modePL;
	//static shared_ptr<ParameterLink<int>> numberOfOutputsPL;
	static shared_ptr<ParameterLink<int>> numMatchesPL;
	static shared_ptr<ParameterLink<int>> numCompetitorsPL;

	//int mode;
	//int numberOfOutputs;
	int numMatches;
	int numCompetitors;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainANamePL;
	static shared_ptr<ParameterLink<string>> brainBNamePL;
	//string groupName;
	string brainAName;
	string brainBName;
	static shared_ptr<ParameterLink<bool>> randomBPL;
	bool randomB;

	RPPWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~RPPWorld() = default;

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) override;

	void evaluateMatch(shared_ptr<Organism> orgA, shared_ptr<Organism> orgB, int analyse, int visualize, int debug);
		//void RPPWorld::evaluateMatch(shared_ptr<Organism> orgA, shared_ptr<Organism> orgB, int analyse, int visualize, int debug);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupName,{ "B:" + brainAName + ",9,3","B:" + brainBName + ",9,3" } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}


};
