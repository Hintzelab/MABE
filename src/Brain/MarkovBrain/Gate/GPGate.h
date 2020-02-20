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

class GPGate : public AbstractGate {
 private:

	int operation; // <link> stores the kind of GP operation (Add, Sub, Mult...)
	vector<double> constValues; // list of constant values
 public:

	static shared_ptr<ParameterLink<double>> constValueMinPL;
	static shared_ptr<ParameterLink<double>> constValueMaxPL;
	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	GPGate() = delete;
	GPGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		operation = 0;
	}
	//GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~GPGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
	virtual string gateType() override{
		return "GeneticPrograming";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

