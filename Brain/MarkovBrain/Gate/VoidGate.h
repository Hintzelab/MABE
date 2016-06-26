//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__VoidGate__
#define __BasicMarkovBrainTemplate__VoidGate__

#include "DeterministicGate.h"

using namespace std;

class VoidGate: public DeterministicGate {
public:

	static shared_ptr<ParameterLink<double>> voidGate_ProbabilityPL;
	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(shared_ptr<ParametersTable> _PT = nullptr) :
		DeterministicGate(_PT) {
		epsilon = 0; 
	}
	VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
			return "Void";
		}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

#endif /* defined(__BasicMarkovBrainTemplate__VoidGate__) */
