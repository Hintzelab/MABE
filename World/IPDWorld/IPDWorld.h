//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__IPDWorld__
#define __BasicMarkovBrainTemplate__IPDWorld__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "../AbstractWorld.h"

using namespace std;

class IPDWorld : public AbstractWorld {
private:
	int outputNodesCount, inputNodesCount;
public:
	// Parameters
	//static shared_ptr<ParameterLink<double>> TSKPL;
	// end parameters

	static shared_ptr<ParameterLink<int>> roundsMinPL;
	static shared_ptr<ParameterLink<int>> roundsMaxPL;
	static shared_ptr<ParameterLink<double>> R_payOffPL; // Reward
	static shared_ptr<ParameterLink<double>> S_payOffPL; // sucker
	static shared_ptr<ParameterLink<double>> T_payOffPL; // Teptation
	static shared_ptr<ParameterLink<double>> P_payOffPL; // Punishment

	static shared_ptr<ParameterLink<bool>> CPL;

	static shared_ptr<ParameterLink<int>> numCompetitorsPL;

	int roundsMin;
	int roundsMax;
	double R_payOff; // Reward
	double S_payOff; // sucker
	double T_payOff; // Temptation
	double P_payOff; // Punishment
	int numCompetitors;

	bool C;
	bool D;

	IPDWorld(shared_ptr<ParametersTable> _PT = nullptr);

	virtual void runWorld(shared_ptr<Group> group, bool analyse, bool visualize, bool debug) override;
	virtual void runWorldDuel(shared_ptr<Organism> player1, shared_ptr<Organism> player2, bool analyse, bool visualize, bool debug);

	virtual int requiredInputs() override{
		return inputNodesCount;
	}
	virtual int requiredOutputs() override {
		return outputNodesCount;
	}

	virtual int maxOrgsAllowed() override {
		return -1;
	}

	virtual int minOrgsAllowed() override {
		return 1;
	}

	//void SaveWorldState(string fileName, vector<int> grid, vector<pair<int, int>> currentLocation, vector<int> facing);
};

#endif /* defined(__BasicMarkovBrainTemplate__IPDWorld__) */
