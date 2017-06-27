//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "Tournament2Optimizer.h"

using namespace std;

shared_ptr<ParameterLink<string>> Tournament2Optimizer::optimizeFormulaPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT2-optimizeValue", (string) "DM_AVE[score]", "value to optimize");
shared_ptr<ParameterLink<string>> Tournament2Optimizer::surviveFormulaPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT2-surviveFormula", (string) "0", "chance that an organism will survive");
shared_ptr<ParameterLink<int>> Tournament2Optimizer::elitismPL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT2-elitism", 0, "The highest scoring organism will be included in the next generation this many times (0 = no elitism)?");
shared_ptr<ParameterLink<int>> Tournament2Optimizer::tournamentSizePL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT2-tournamentSize", 5, "number of organisms considered in each tournament?");
shared_ptr<ParameterLink<double>> Tournament2Optimizer::selfRatePL = Parameters::register_parameter("OPTIMIZER_TOURNAMENT2-selfRate", 0.0, "prabablity that organsim will not choose mate, and rather will cross with self");

void Tournament2Optimizer::optimize(vector<shared_ptr<Organism>> &population) {

	int oldPopulationSize = (int)population.size();

	int nextPopulationTargetSize = popSizeLPL->lookup();
	int nextPopulationSize = 0;

	set<shared_ptr<Organism>> survivors;
	int p1, p2;  // parent 1 and 2
	int challanger;  // used when picking best of
	double surviveChance = 0;
	bool orgSurvived = 0;
	int selfCount = 0;
	int otherCount = 0;
	int surviveCount = 0;

	vector<double> Scores;
	double aveScore = 0;
	vector<double> surviveRates;

	for (int i = 0; i < (int)population.size(); i++) {
		Scores.push_back(optimizeFormula->eval(population[i]->dataMap, PT)[0]);
		aveScore += Scores.back();
		population[i]->dataMap.Append("optimizeValue", Scores.back());

		surviveRates.push_back(surviveFormula->eval(population[i]->dataMap, PT)[0]);

		killList.insert(population[i]);
	}

	aveScore /= population.size();

	int best = findGreatestInVector(Scores);
	double maxScore = Scores[best];

	while (nextPopulationSize < nextPopulationTargetSize) {  // while we have not filled up the next generation
		if (nextPopulationSize < elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
			p1 = best;
		}
		else {  // otherwise, p1 is the best of tournamentSize random picks
			p1 = Random::getIndex(oldPopulationSize);
			for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
				challanger = Random::getIndex(oldPopulationSize);
				if (Scores[challanger] > Scores[p1]) {
					p1 = challanger;
				}
			}
		}

		orgSurvived = 0;  // clear orgSurvived. if population[p1] survives this will become 1 and we will not pick a mate
		if (Random::P(surviveRates[p1]) && survivors.find(population[p1]) == survivors.end()) {  // if this org survives and has not already survived
			surviveCount++;
			population.push_back(population[p1]);  // push them to the next population
			orgSurvived = 1;
			survivors.insert(population[p1]);
			killList.erase(population[p1]);
			nextPopulationSize++;
		}

		if (!orgSurvived) {
			p2 = p1;  // make these the same to prime the while loop
			//while ((p1 == p2) || (population[p1]->gender == population[p2]->gender)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
			if (nextPopulationSize >= elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
				if (Random::P(selfRateLPL->lookup())) {
					selfCount++;
					p2 = p1;
				}
				else { // mate with other
					otherCount++;
					while ((p1 == p2)) {  // keep picking until you have 2 diffrent parents
						p2 = Random::getIndex(oldPopulationSize);
						//cout << "  " << p2 << " " << p1 << endl;

						for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
							challanger = Random::getIndex(oldPopulationSize);
							while ((p1 == challanger)) {  // if we pick p1, pick again (it is possible that p2 was select from less then tournament size, it the same challenger/p2 is picked twice
								challanger = Random::getIndex(oldPopulationSize);
							}
							if (Scores[challanger] > Scores[p2]) {
								p2 = challanger;
							}
						}
					}
				}
			}
			population.push_back(population[p1]->makeMutatedOffspringFromMany({ population[p1], population[p2] }));
			nextPopulationSize++;
		}

	}
	cout << "max = " << to_string(maxScore) << "   ave = " << to_string(aveScore) << "  survive,other,self: " << surviveCount << "," << otherCount << "," << selfCount;
	for (auto org : population) {
		org->dataMap.Set("Tournament2_numOffspring", org->offspringCount);
	}
}

