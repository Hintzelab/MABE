//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "TournamentOptimizer.h"

#include <algorithm>
#include <math.h>
#include <stdlib.h>     // for atoi

using namespace std;

shared_ptr<ParameterLink<string>> TournamentOptimizer::optimizeFormulaPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-optimizeValue", (string) "DM_AVE[score]", "value to optimize");
shared_ptr<ParameterLink<int>> TournamentOptimizer::elitismPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-elitism", 0, "The highest scoring organism will be included in the next generation this many times (0 = no elitism)?");
shared_ptr<ParameterLink<int>> TournamentOptimizer::tournamentSizePL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-tournamentSize", 5, "number of organisms considered in each tournament?");

/*
 * Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time
 * for each next population genome, randomly select (with replacement) n genomes (where n = Optimizer::tournamentSize)
 * copy to the next generation and mutate the copy.
 */
void TournamentOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
	int oldPopulationSize = (int)population.size();

	int nextPopulationTargetSize = popSizeLPL->lookup();
	int nextPopulationSize = 0;
	vector<double> Scores;
	double aveScore = 0;

	killList.clear();

	for (auto org : population) {
		Scores.push_back(optimizeFormula->eval(org->dataMap, PT)[0]);
		aveScore += Scores.back();
		org->dataMap.Set("optimizeValue", Scores.back());
		killList.insert(org);
	}

	aveScore /= population.size();

	int best = findGreatestInVector(Scores);
	double maxScore = Scores[best];

	while (nextPopulationSize < nextPopulationTargetSize) {
		int winner, challanger;
		if ((int) nextPopulationSize < elitismLPL->lookup()) {
			winner = best;
		} else {
			winner = Random::getIndex(oldPopulationSize);
			for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
				challanger = Random::getIndex(oldPopulationSize);
				if (Scores[challanger] > Scores[winner]) {
					winner = challanger;
				}
			}
		}
		population.push_back(population[winner]->makeMutatedOffspringFrom(population[winner]));
		nextPopulationSize++;
	}
	cout << "max = " << to_string(maxScore) << "   ave = " << to_string(aveScore);
}
