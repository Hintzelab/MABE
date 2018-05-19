//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AbstractOptimizer.h"

/*
#include <algorithm>
#include <math.h>
#include <stdlib.h> // for atoi

#include "../Utilities/Random.h"
*/

////// OPTIMIZER-optimizer is actually set by Modules.h //////
std::shared_ptr<ParameterLink<std::string>>
    AbstractOptimizer::Optimizer_MethodStrPL = Parameters::register_parameter(
        "OPTIMIZER-optimizer", std::string("This_string_is_set_by_modules.h"),
        "This_string_is_set_by_modules.h"); // string parameter for
                                            // outputMethod;
////// OPTIMIZER-optimizer is actually set by Modules.h //////

/*
 * Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * place holder function, copies population to make new population
 * no selection and no mutation
 */
// void BaseOptimizer::makeNextGeneration(vector<shared_ptr<Organism>>
// &population) {
//	vector<shared_ptr<Organism>> nextPopulation;
//	for (size_t i = 0; i < population.size(); i++) {
//		shared_ptr<Organism> newOrg = make_shared<Organism>(population[i],
//population[i]->genome);
//		nextPopulation.push_back(newOrg);
//	}
//	for (size_t i = 0; i < population.size(); i++) {
//		population[i]->kill();  // set org.alive = 0 and delete the
//organism if it has no offspring
//	}
//	population = nextPopulation;
//}
