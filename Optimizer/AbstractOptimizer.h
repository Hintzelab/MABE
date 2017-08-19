//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../Genome/AbstractGenome.h"
#include "../Organism/Organism.h"

#include "../Utilities/Utilities.h"
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
	vector<string> popFileColumns;

	unordered_set <shared_ptr<Organism>> killList; // set of organisms to be killed after archive 

	AbstractOptimizer(shared_ptr<ParametersTable> _PT) : PT(_PT) {

	}

	virtual ~AbstractOptimizer() = default;
	//virtual vector<shared_ptr<Organism>> makeNextGeneration(vector<shared_ptr<Organism>> &population) = 0;
	virtual void optimize(vector<shared_ptr<Organism>> &population) = 0;

	virtual void cleanup(vector<shared_ptr<Organism>> &population) {
		vector<shared_ptr<Organism>> newPopulation;
		for (auto org : population) {
			if (killList.find(org) == killList.end()) { // if not in kill list
				newPopulation.push_back(org); // move into new population
			}
			else {
				org->kill(); // if in kill list, call kill
			}
		}

		population = newPopulation;
		killList.clear();
	}

	//virtual string maxValueName() {
	//	return("score");
	//}

	virtual bool requireGenome() {
		return false;
	}
	virtual bool requireBrain() {
		return false;
	}


	virtual unordered_set<string> requiredGenomes() {
		return {};
		// "root" = use empty name space
		// "GROUP::" = use group name space
		// "blah" = use "blah namespace at root level
		// "Group::blah" = use "blah" name space inside of group name space
	}

};

