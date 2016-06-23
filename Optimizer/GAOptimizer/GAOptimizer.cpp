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

/*
 * GA::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time for each next population genome, select a random number
 * then select (with replacement) a random genome. If the selected genomes fitness beats (there is some
 * normalizing of genomes fitness to max fitness and math, but that's really it) the random value, copy
 * to the next generation and mutate the copy. If it is too low, keep drawing genomes till you get one
 * which is good enough.
 */
void GAOptimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	vector<shared_ptr<Organism>> nextPopulation;

	vector<double> Scores;
	for (auto org : population) {
		Scores.push_back(org->score);
	}

	int best = findGreatestInVector(Scores);
	maxFitness = Scores[best];

	//now to roulette wheel selection:
	while (nextPopulation.size() < population.size()) {
		int who;
		if ((int) nextPopulation.size() < elitismLPL->lookup()) {
			who = best;
		} else {
			if (maxFitness > 0.0) {  // if anyone has fitness > 0
				do {
					who = Random::getIndex(population.size());  //keep choosing a random genome from population until we get one that's good enough
				} while (pow(1.05, Random::getDouble(1)) > pow(1.05, (Scores[who] / maxFitness)));
			} else {
				who = Random::getIndex(population.size());  // otherwise, just pick a random genome from population
			}
		}
		nextPopulation.push_back(population[who]->makeMutatedOffspringFrom(population[who]));

	}
	for (size_t i = 0; i < population.size(); i++) {
		population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
	}
	population = nextPopulation;
}

