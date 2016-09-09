//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__NumeralClassifierWorld__
#define __BasicMarkovBrainTemplate__NumeralClassifierWorld__

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

	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

	virtual int requiredInputs() override {
		return inputNodesCount;
	}
	virtual int requiredOutputs() override {
		return outputNodesCount;
	}
	virtual int maxOrgsAllowed() override {
		return 1;
	}
	virtual int minOrgsAllowed() override {
		return 1;
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__NumeralClassifierWorld__) */
