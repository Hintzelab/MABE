//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "AbstractWorld.h"

#include <math.h>

#include "../Utilities/Data.h"

shared_ptr<ParameterLink<bool>> AbstractWorld::debugPL = Parameters::register_parameter("WORLD-debug", false, "run world in debug mode (if available)");
shared_ptr<ParameterLink<string>> AbstractWorld::worldTypePL = Parameters::register_parameter("WORLD-worldType", (string) "Berry", "type of world to use in evolution loop");

void AbstractWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	int popSize = groups["default"]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups["default"]->population[i], analyse, visualize, debug);
	}
}
