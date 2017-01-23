//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__World__
#define __BasicMarkovBrainTemplate__World__

#include <stdlib.h>
#include <thread>
#include <vector>

#include "../Group/Group.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class AbstractWorld {
public:
	static shared_ptr<ParameterLink<bool>> debugPL;
	static shared_ptr<ParameterLink<string>> worldTypePL;
	
	const shared_ptr<ParametersTable> PT;

	vector<string> aveFileColumns;

	AbstractWorld(shared_ptr<ParametersTable> _PT = nullptr) :
			PT(_PT) {
	}
	virtual ~AbstractWorld() = default;
	virtual int requiredInputs() = 0;
	virtual int requiredOutputs() = 0;
	virtual bool requireGenome(){
		return false;
	}
	virtual bool requireBrain(){
		return true;
	}

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0);
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
		cout << "  chosen world does not define evaluateSolo()! Exiting." << endl;
		exit(1);
	};
};

#endif /* defined(__BasicMarkovBrainTemplate__World__) */
