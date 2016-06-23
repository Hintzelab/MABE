//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__FixedEpsilonGate__
#define __BasicMarkovBrainTemplate__FixedEpsilonGate__

#include "DeterministicGate.h"

using namespace std;

class FixedEpsilonGate: public DeterministicGate {
public:
	static shared_ptr<ParameterLink<double>> EpsilonSourcePL;

	vector<int> defaultOutput;
	double epsilon;
	
	FixedEpsilonGate() = delete;
	FixedEpsilonGate(shared_ptr<ParametersTable> _PT = nullptr) :
		DeterministicGate(_PT) {
		epsilon = 0;
	}
	FixedEpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "FixedEpsilon";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

#endif /* defined(__BasicMarkovBrainTemplate__FixedEpsilonGate__) */
