//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldSOF__
#define __BasicMarkovBrainTemplate__WorldSOF__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class SOFWorld : public AbstractWorld {

public:
	
	//parameters
	static shared_ptr<ParameterLink<string>> scoreMapFilenamePL;

	
	vector<int> scoreMatrix;
	int x, y;

	
	SOFWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~SOFWorld() = default;
	virtual void runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) override;

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;
	virtual int maxOrgsAllowed() override;
	virtual int minOrgsAllowed() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSOF__) */
