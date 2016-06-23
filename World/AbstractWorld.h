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
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class AbstractWorld {
public:
	static shared_ptr<ParameterLink<int>> repeatsPL;
	static shared_ptr<ParameterLink<bool>> debugPL;

	static shared_ptr<ParameterLink<bool>> groupEvaluationPL;

	static shared_ptr<ParameterLink<string>> worldTypePL;

	const shared_ptr<ParametersTable> PT;

	vector<string> aveFileColumns;

	AbstractWorld(shared_ptr<ParametersTable> _PT = nullptr) :
			PT(_PT) {
	}
	virtual ~AbstractWorld() = default;
	virtual int requiredInputs() = 0;
	virtual int requiredOutputs() = 0;

	// new stuff
	virtual void evaluate(shared_ptr<Group> group, bool groupEvaluation = 0, bool analyse = 0, bool visualize = 0, bool debug = 0);
	virtual void runWorld(shared_ptr<Group> group, bool analyse, bool visualize, bool debug) {
		runWorldSolo(group->population[0], analyse, visualize, debug);
	}
	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
		cout << "  AbstractWorld::runWorldSolo : a call was made to the abstract function runWorldSolo. Your world must impliment runWorldSolo.\n  Exiting.";
		exit(1);
	}

	virtual int maxOrgsAllowed() = 0;
	virtual int minOrgsAllowed() = 0;

};

#endif /* defined(__BasicMarkovBrainTemplate__World__) */
