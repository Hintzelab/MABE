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

shared_ptr<ParameterLink<string>> TournamentOptimizer::optimizeFormulaPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-optimizeValue", (string) "DM[score]", "value to optimize");
shared_ptr<ParameterLink<int>> TournamentOptimizer::elitismPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-elitism", 0, "The highest scoring organism will be included in the next generation this many times (0 = no elitism)?");
shared_ptr<ParameterLink<int>> TournamentOptimizer::tournamentSizePL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT-tournamentSize", 5, "number of organisms considered in each tournament?");

/*
 * Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time
 * for each next population genome, randomly select (with replacement) n genomes (where n = Optimizer::tournamentSize)
 * copy to the next generation and mutate the copy.
 */
void TournamentOptimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	//cout << "In TournamentOptimizer::makeNextGeneration\n";
	vector<shared_ptr<Organism>> nextPopulation;

	vector<double> Scores;
	double aveScore = 0;

	for (auto org : population) {
		Scores.push_back(optimizeFormula->eval(org->dataMap, PT)[0]);
		aveScore += Scores.back();
	}

	aveScore /= population.size();

	int best = findGreatestInVector(Scores);
	double maxScore = Scores[best];

	while (nextPopulation.size() < population.size()) {
		int winner, challanger;
		if ((int) nextPopulation.size() < elitismLPL->lookup()) {
			winner = best;
		} else {
			winner = Random::getIndex(population.size());
			for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
				challanger = Random::getIndex(population.size());
				if (Scores[challanger] > Scores[winner]) {
					winner = challanger;
				}
			}
		}
		nextPopulation.push_back(population[winner]->makeMutatedOffspringFrom(population[winner]));
		//nextPopulation.push_back(population[winner]);
	}
	for (size_t i = 0; i < population.size(); i++) {
		population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
	}
	//population = nextPopulation;
	//population.resize(0);
//	while (population.size() > 0){
//		cout << population.size() << " " << flush;
//		auto keys = population[population.size()-1]->dataMap.getKeys();
//		for (auto key:keys){
//			cout << key << " = " << flush << population[population.size()-1]->dataMap.Get(key) << "   " << flush;
//
//		}
//		cout << "   done keys" << endl;
//		population.pop_back();
//	}
	population = nextPopulation;
	cout << "max = " << maxScore << "\tave = " << aveScore;
	//cout << "Leaving TournamentOptimizer::makeNextGeneration\n";


}
