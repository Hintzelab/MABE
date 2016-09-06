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

void Tournament2Optimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	vector<shared_ptr<Organism>> nextPopulation;
	set<shared_ptr<Organism>> survivors;
	int p1, p2;  // parent1 and 2
	int challanger;  // used when picking best of
	double surviveChance = 0;
	bool orgSurvived = 0;

	vector<double> Scores;
	for (auto org : population) {
		Scores.push_back(org->score);
	}
	int best = findGreatestInVector(Scores);
	maxFitness = Scores[best];

//	{  // check to make sure there are at least 2 genders.
//		bool same = true;
//		for (int index = 0; index < (int) population.size() - 2; index++) {
//			if (population[index]->gender != population[index + 1]->gender) {
//				same = false;
//			}
//		}
//		if (same == true) {  // if there are not atleast 2 genders, make population[0] have a diffrent gender
//			(population[0]->gender == 1) ? population[0]->gender = 0 : population[0]->gender = 1;
//		}
//	}

	while (nextPopulation.size() < population.size()) {  // while we have not filled up the next generation
		//cout << nextPopulation.size() << " < " << population.size() << endl;
		// chance for each pick that this org survives to the next population
		//cout << "picking p1..." << flush;
		if ((int) nextPopulation.size() < elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
			//cout << "E" << endl;
			p1 = best;
		} else {  // otherwise, p1 is the best of tournamentSize random picks
			//cout << "R" << endl;
			p1 = Random::getIndex(population.size());
			for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
				challanger = Random::getIndex(population.size());
				if (Scores[challanger] > Scores[p1]) {
					p1 = challanger;
				}
			}
		}
		//cout << p1 << flush;
		//cout << "  picking p2..." << flush;

		orgSurvived = 0;  // clear orgSurvived. if population[p1] survives this will become 1 and we will not pick a mate
		if (Random::P(surviveChance)) {  // if this org survives
			if (find(nextPopulation.begin(), nextPopulation.end(), population[p1]) == nextPopulation.end()) {  // if they have not already survived
				nextPopulation.push_back(population[p1]);  // push them to the next population
				orgSurvived = 1;
				survivors.insert(population[p1]);
			}
		}
		if (!orgSurvived) {
			p2 = p1;  // make these the same to prime the while loop
			//while ((p1 == p2) || (population[p1]->gender == population[p2]->gender)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
			if ((int)nextPopulation.size() >= elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
				//cout << "R" << endl;
				while ((p1 == p2)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
					//cout << p2 << " " << p1 << endl;
					p2 = Random::getIndex(population.size());
					//cout << "  " << p2 << " " << p1 << endl;

					for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
						challanger = Random::getIndex(population.size());
						//cout << "  ch = " << challanger << "\n";
						if (Scores[challanger] > Scores[p2]) {
							//cout << "p2 assigned" << endl;
							p2 = challanger;
						}
					}
				}
			}
			//cout << "\n " << nextPopulation.size() << " " << population.size() << endl;
			nextPopulation.push_back(population[p1]->makeMutatedOffspringFromMany( { population[p1], population[p2] }));
			//cout << p1 << " " << p2 << endl;
			//nextPopulation.push_back(population[p1]->makeMutatedOffspring(population[p1]));
			//cout << " " << nextPopulation.size() << " " << population.size() << endl;
		}

	}
	for (auto org : population) {
		if (!survivors.count(org)) {  // if this org did not survive to the next generation, it must be killed (sad.)
			org->kill();
		}
	}
	population = nextPopulation;
}

