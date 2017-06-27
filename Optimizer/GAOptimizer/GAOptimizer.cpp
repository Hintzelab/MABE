//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "GAOptimizer.h"

#include <algorithm>
#include <math.h>
#include <stdlib.h>     // for atoi

using namespace std;

shared_ptr<ParameterLink<string>> GAOptimizer::optimizeFormulaPL = Parameters::register_parameter("OPTIMIZER_GA-optimizeValue", (string) "DM_AVE[score]", "value to optimize");
shared_ptr<ParameterLink<int>> GAOptimizer::elitismPL = Parameters::register_parameter("OPTIMIZER_GA-elitism", 0, "The highest scoring organism will be included in the next generation this many times (0 = no elitism)?");

/*
 * GA::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time for each next population genome, select a random number
 * then select (with replacement) a random genome. If the selected genomes fitness beats (there is some
 * normalizing of genomes fitness to max fitness and math, but that's really it) the random value, copy
 * to the next generation and mutate the copy. If it is too low, keep drawing genomes till you get one
 * which is good enough.
 */
void GAOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
	
	int oldPopulationSize = (int)population.size();
	
	int nextPopulationTargetSize = popSizeLPL->lookup();
	int nextPopulationSize = 0;
	vector<double> Scores;
	double aveScore = 0;

	killList.clear();

	for (auto org : population) {
		Scores.push_back(optimizeFormula->eval(org->dataMap,PT)[0]);
		aveScore += Scores.back();
		org->dataMap.Append("optimizeValue", Scores.back());
		killList.insert(org);
	}

	aveScore /= population.size();

	int best = findGreatestInVector(Scores);
	double maxScore = Scores[best];

	//now to roulette wheel selection:
	while (nextPopulationSize < nextPopulationTargetSize) {
		int p1;
		if (nextPopulationSize < elitismLPL->lookup()) {
			p1 = best;
		} else {
			if (maxScore > 0.0) {  // if anyone has fitness > 0
				do {
					p1 = Random::getIndex(oldPopulationSize);  //keep choosing a random genome from population until we get one that's good enough
				} while (pow(1.05, Random::getDouble(1)) > pow(1.05, (Scores[p1] / maxScore)));
			} else {
				p1 = Random::getIndex(oldPopulationSize);  // otherwise, just pick a random genome from population
			}
		}
		population.push_back(population[p1]->makeMutatedOffspringFrom(population[p1]));
		nextPopulationSize++;
	}
	//for (size_t i = 0; i < population.size(); i++) {
	//	population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
	//}
	cout << "max = " << to_string(maxScore) << "   ave = " << to_string(aveScore);
}

