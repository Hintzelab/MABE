//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__DeterministicGate__
#define __BasicMarkovBrainTemplate__DeterministicGate__

#include "AbstractGate.h"

using namespace std;


class DeterministicGate: public AbstractGate {
public:
	
	static shared_ptr<ParameterLink<string>> IO_RangesPL;

	vector<vector<int>> table;
	DeterministicGate() = delete;
	DeterministicGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {}; 
	}
	DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);
	virtual string gateType() override{
		return "Deterministic";
	}
};


#endif /* defined(__BasicMarkovBrainTemplate__DeterministicGate__) */
