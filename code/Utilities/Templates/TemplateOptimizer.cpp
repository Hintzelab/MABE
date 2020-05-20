//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "{{MODULE_NAME}}Optimizer.h"

std::shared_ptr<ParameterLink<std::string>> {{MODULE_NAME}}Optimizer::optimizeValuePL =
	Parameters::register_parameter("OPTIMIZER_{{MODULE_NAME}}-optimizeValue", (std::string) "DM_AVE[score]", "value to optimize (MTree)");

{{MODULE_NAME}}Optimizer::{{MODULE_NAME}}Optimizer(std::shared_ptr<ParametersTable> PT_)
	: AbstractOptimizer(PT_) {

    // convert the optimizeValuePL to an mtree
	optimizeValueMT = stringToMTree(optimizeValuePL->get(PT));

    // set this so Archivist knows which org is max
	optimizeFormula = optimizeValueMT;

    // popFileColumns sets what data will be saved to pop.csv
    // make sure you add these values to every organsims dataMap
	popFileColumns.clear();
	popFileColumns.push_back("optimizeValue");
}

void {{MODULE_NAME}}Optimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
	auto popSize = population.size();

	std::vector<double> scores(popSize, 0);
	double aveScore = 0;
    
    // set maxScore to some score in the population
	double maxScore = optimizeValueMT->eval(population[0]->dataMap, PT)[0];
	double minScore = maxScore;

    // kill list is used in clean up (see below) do determine with organaims will be removed from the population
	killList.clear();

	for (size_t i = 0; i < popSize; i++) {
        // this will kill every organism in the current generation
		killList.insert(population[i]);
        
        // calculate the max value for all organisms
		double opVal = optimizeValueMT->eval(population[i]->dataMap, PT)[0];
        
        // add to scores vector, i is index in population and scores
		scores[i] = opVal;
		aveScore += opVal;
        // set score on dataMap
		population[i]->dataMap.set("optimizeValue", opVal);
		maxScore = std::max(maxScore, opVal);
		minScore = std::min(minScore, opVal);
	}
	
	aveScore /= popSize;

	for (int i = 0; i < popSize; i++) {
		auto parent = population[Random::getIndex(popSize)];
		population.push_back(parent->makeMutatedOffspringFrom(parent));// add to population
	}
    
    // the population is currently double the size (all old orgs and new orgs)

	for (int i = 0; i < popSize; i++) {
        // record number of offspring each org produced
		population[i]->dataMap.set("tournament_numOffspring", population[i]->offspringCount);
	}

	std::cout << "max = " << std::to_string(maxScore) << "   ave = " << std::to_string(aveScore);
}


// by defaut, the cleanup function defined in AbstractOptimizer is being used.
// cleanup removes orgnaisms in killList.

