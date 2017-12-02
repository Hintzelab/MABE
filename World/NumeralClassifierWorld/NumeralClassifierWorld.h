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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "../AbstractWorld.h"

using namespace std;

class NumeralClassifierWorld : public AbstractWorld {
private:
	int outputNodesCount, inputNodesCount;
public:
	const int numberOfDirections = 4;

	const int BLACK = 0;
	const int WHITE = 1;

	// Parameters
	static shared_ptr<ParameterLink<int>> defaulttestsPreWorldEvalPL;
	static shared_ptr<ParameterLink<int>> defaultWorldUpdatesPL;
	static shared_ptr<ParameterLink<int>> defaultRetinaTypePL;
	static shared_ptr<ParameterLink<string>> numeralDataFileNamePL;

	// end parameters

	int worldUpdates;
	int testsPreWorldEval;
	int retinaType;
	string numeralDataFileName;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;

	vector<pair<int, int>> retinalOffsets = { { 0, 0 }, { -1, 0 }, { 1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 }, { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { -2, -1 }, { 2, -1 }, { -2, 0 }, { 2, 0 }, { -2, 1 }, { 2, 1 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, { 0, -3 }, { 1, -3 }, { 2, -3 }, { 3, -3 }, { -3, 3 }, { -2, 3 }, { -1, 3 }, { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 }, { -3, -2 }, { 3, -2 }, { -3, -1 }, { 3, -1 }, { -3, 0 }, { 3, 0 }, { -3, 1 }, { 3, 1 }, { -3, 2 }, { 3, 2 } };
	// retina is a list of offsets defining input sensor array to brain
	// 25 26 27 28 29 30 31
	// 39  9 10 11 12 13 40
	// 41 19  3  4  5 20 42
	// 43 21  1  0  2 22 44
	// 45 32  6  7  8 24 46
	// 47 14 15 16 17 18 48
	// 32 33 34 35 36 37 38
	int retinaSensors, stepSize;

	vector<vector<bool>>numeralData;


	NumeralClassifierWorld(shared_ptr<ParametersTable> _PT = nullptr);

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse, visualize, debug);
		}
	}

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + "," + to_string(inputNodesCount) + "," + to_string(outputNodesCount) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

};
