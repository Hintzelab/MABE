//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#pragma once

#include "AbstractGate.h"

using namespace std;


class DecomposableGate: public AbstractGate {  //conventional probabilistic gate
public:

	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<vector<double>> table;
	DecomposableGate() = delete;
	DecomposableGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	DecomposableGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _rawTable, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DecomposableGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Decomposable";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
};

