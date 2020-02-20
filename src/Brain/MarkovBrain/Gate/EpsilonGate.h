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

#include "DeterministicGate.h"

using namespace std;

class EpsilonGate: public DeterministicGate {
public:
	static shared_ptr<ParameterLink<double>> EpsilonSourcePL;
	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<int> defaultOutput;
	double epsilon;
	
	EpsilonGate() = delete;
	EpsilonGate(shared_ptr<ParametersTable> _PT = nullptr) :
		DeterministicGate(_PT) {
		epsilon = 0;
	}
	EpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~EpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Epsilon";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};
