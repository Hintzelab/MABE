//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

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
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	int mode;
	int numberOfOutputs;
	int evaluationsPerGeneration;

	TestWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~TestWorld() = default;

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;


	//virtual int maxOrgsAllowed() override;
	//virtual int minOrgsAllowed() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
