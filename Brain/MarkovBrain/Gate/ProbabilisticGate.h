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

#include "AbstractGate.h"

using namespace std;


class ProbabilisticGate: public AbstractGate {  //conventional probabilistic gate
public:

	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<vector<double>> table;
	ProbabilisticGate() = delete;
	ProbabilisticGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	ProbabilisticGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _rawTable, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~ProbabilisticGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Probabilistic";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
};
