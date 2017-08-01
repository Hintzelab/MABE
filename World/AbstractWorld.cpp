//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AbstractWorld.h"

#include <math.h>

#include "../Utilities/Data.h"

shared_ptr<ParameterLink<bool>> AbstractWorld::debugPL = Parameters::register_parameter("WORLD-debug", false, "run world in debug mode (if available)");

////// WORLD-worldType is actually set by Modules.h //////
shared_ptr<ParameterLink<string>> AbstractWorld::worldTypePL = Parameters::register_parameter("WORLD-worldType", (string) "This_string_is_set_by_modules.h", "This_string_is_set_by_modules.h");
////// WORLD-worldType is actually set by Modules.h //////

void AbstractWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	int popSize = groups[groupName]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups[groupName]->population[i], analyse, visualize, debug);
	}
}
