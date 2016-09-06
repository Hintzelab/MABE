//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "AbstractOptimizer.h"

#include <algorithm>
#include <math.h>
#include <stdlib.h>     // for atoi

#include "../Utilities/Random.h"

using namespace std;

shared_ptr<ParameterLink<string>> AbstractOptimizer::Optimizer_MethodStrPL = Parameters::register_parameter("OPTIMIZER-optimizer", (string) "GA", "optimizer to be used in evolution loop, [GA, Tournament, Tournament2]");  // string parameter for outputMethod;

shared_ptr<ParameterLink<int>> AbstractOptimizer::elitismPL = Parameters::register_parameter("OPTIMIZER-elitism", 0, "if the chosen optimizer allows for elitism, The highest scoring brain will be included in the next generation this many times (0 = no elitism)?");
shared_ptr<ParameterLink<int>> AbstractOptimizer::tournamentSizePL = Parameters::register_parameter("OPTIMIZER-tournamentSize", 5, "how many genomes to consider when doing Tournament selection? 1 will result in random selection.");

/*
 * Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * place holder function, copies population to make new population
 * no selection and no mutation
 */
//void BaseOptimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
//	vector<shared_ptr<Organism>> nextPopulation;
//	for (size_t i = 0; i < population.size(); i++) {
//		shared_ptr<Organism> newOrg = make_shared<Organism>(population[i], population[i]->genome);
//		nextPopulation.push_back(newOrg);
//	}
//	for (size_t i = 0; i < population.size(); i++) {
//		population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
//	}
//	population = nextPopulation;
//}
