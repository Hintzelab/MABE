//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../Genome/AbstractGenome.h"
#include "../Organism/Organism.h"

#include "../Utilities/MTree.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"

using namespace std;

class AbstractOptimizer {
 public:
	shared_ptr<Abstract_MTree> optimizeFormula;


	static shared_ptr<ParameterLink<string>> Optimizer_MethodStrPL;
	static shared_ptr<ParameterLink<string>> reportNamesPL;

 public:
	const shared_ptr<ParametersTable> PT;

	AbstractOptimizer(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {

	}

	virtual ~AbstractOptimizer() = default;
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) = 0;
	virtual string maxValueName() {
		return("score");
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
