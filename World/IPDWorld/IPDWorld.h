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

class IPDWorld : public AbstractWorld {
private:
	int outputNodesCount, inputNodesCount;

	int numRounds;
	int currentUpdate;

public:

	static shared_ptr<ParameterLink<bool>> roundsFixedPerGenerationPL;
	static shared_ptr<ParameterLink<int>> roundsMinPL;
	static shared_ptr<ParameterLink<int>> roundsMaxPL;
	static shared_ptr<ParameterLink<double>> R_payOffPL; // Reward
	static shared_ptr<ParameterLink<double>> S_payOffPL; // sucker
	static shared_ptr<ParameterLink<double>> T_payOffPL; // Temptation
	static shared_ptr<ParameterLink<double>> P_payOffPL; // Punishment

	static shared_ptr<ParameterLink<bool>> CPL;

	static shared_ptr<ParameterLink<int>> numCompetitorsPL;

	static shared_ptr<ParameterLink<bool>> skipFirstMovePL;
	static shared_ptr<ParameterLink<bool>> randomFirstMovePL;
	static shared_ptr<ParameterLink<bool>> saveMovesListPL;

	static shared_ptr<ParameterLink<string>> fixedStrategiesPL;
	static shared_ptr<ParameterLink<int>> playsVsFixedStrategiesPL;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainANamePL;
	static shared_ptr<ParameterLink<string>> brainBNamePL;

	bool roundsFixedPerGeneration;
	int roundsMin;
	int roundsMax;
	double R_payOff; // Reward
	double S_payOff; // sucker
	double T_payOff; // Temptation
	double P_payOff; // Punishment
	int numCompetitors;

	bool skipFirstMove;
	bool randomFirstMove;
	bool saveMovesList;

	int C;
	int D;

	vector<string> fixedStrategies;
	int playsVsFixedStrategies;

	//string groupName;
	string brainAName;
	string brainBName;

	vector<shared_ptr<Organism>> fixedOrgs;


	IPDWorld(shared_ptr<ParametersTable> _PT);

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) override;
	virtual pair<double,double> runDuel(shared_ptr<Organism> player1, shared_ptr<Organism> player2, bool analyse, bool visualize, bool debug);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{
			"B:" + brainANamePL->get(PT) + "," + to_string(inputNodesCount) + "," + to_string(outputNodesCount),
			"B:" + brainBNamePL->get(PT) + "," + to_string(inputNodesCount) + "," + to_string(outputNodesCount) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

};

