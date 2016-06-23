//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../Genome/AbstractGenome.h"
#include "../Organism/Organism.h"

#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"

using namespace std;

class AbstractOptimizer {
 public:


	static shared_ptr<ParameterLink<string>> Optimizer_MethodStrPL;

	static shared_ptr<ParameterLink<int>> elitismPL;
	static shared_ptr<ParameterLink<int>> tournamentSizePL;

 public:
	const shared_ptr<ParametersTable> PT;
	double maxFitness;
	AbstractOptimizer(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {
		maxFitness = 0;
	}
	virtual ~AbstractOptimizer() = default;
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) = 0;
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
