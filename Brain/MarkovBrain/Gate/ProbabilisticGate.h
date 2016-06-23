//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__ProbabilisticGate__
#define __BasicMarkovBrainTemplate__ProbabilisticGate__

#include "AbstractGate.h"

using namespace std;


class ProbabilisticGate: public AbstractGate {  //conventional probabilistic gate
public:
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

#endif /* defined(__BasicMarkovBrainTemplate__ProbabilisticGate__) */
