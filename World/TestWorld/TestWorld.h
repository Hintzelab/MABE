//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__WorldTest__
#define __BasicMarkovBrainTemplate__WorldTest__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class TestWorld : public AbstractWorld {

public:

	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;

	int mode;
	int numberOfOutputs;


	TestWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~TestWorld() = default;
	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;
	virtual int maxOrgsAllowed() override;
	virtual int minOrgsAllowed() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
