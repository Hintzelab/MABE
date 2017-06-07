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

vector<shared_ptr<Organism>> Tournament2Optimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	vector<shared_ptr<Organism>> nextPopulation;
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

	for (auto org : population) {
		Scores.push_back(optimizeFormula->eval(org->dataMap, PT)[0]);

		surviveRates.push_back(surviveFormula->eval(org->dataMap, PT)[0]);
		aveScore += Scores.back();
	}


	/*
	map<string,pair<double, double>> valRanges;
	map<string, double> valAves;

	for (auto key : population[0]->dataMap.getKeys()) {
		valRanges[key] = { population[0]->dataMap.GetAverage(key) ,population[0]->dataMap.GetAverage(key) };
		valAves[key] = 0;
		for (auto org : population) {
			double temp = org->dataMap.GetAverage(key);
			valRanges[key].first = min(valRanges[key].first, temp);
			valRanges[key].second = max(valRanges[key].second, temp);
			valAves[key] += temp;
		}
		valAves[key] /= (double)(population.size());
	}

	for (auto key : population[0]->dataMap.getKeys()) {
		cout << key << "_ranges = " << valRanges[key].first << " , " << valRanges[key].second << "   ave: " << valAves[key] << endl;
	}

	// now we know all values and ranges.

	// pick 2, lets say NUM_CLASS_BRAIN_aveFormulaLength and CGP_GENOME_genomeLength_ranges
	int XSIZE = 30;
	int YSIZE = 30;
	Vector2d<int> bestOrgs(XSIZE, YSIZE); // best 2 ids in each box

	for (int x = 0; x < XSIZE; x++) {
		for (int y = 0; y < YSIZE; y++) {
			bestOrgs(x, y) = -1;
		}
	}
	for (int index = 0; index < population.size(); index++) {
		//cout << "index: " << index << flush;
		// find box for this org
		string key = "repeated";
		
		//cout << "\n" << population[index]->dataMap.GetAverage(key) << "  " << valRanges[key].first << "   " << valRanges[key].second << endl;
		int orgX = 0;
		int orgY = 0;
		
		if (valRanges[key].second - valRanges[key].first == 0) {
			orgX = 0;
		}
		else {
			orgX = (population[index]->dataMap.GetAverage(key) - valRanges[key].first) / (valRanges[key].second - valRanges[key].first) * XSIZE;
		}
		key = "food1";
		//cout << "\n" << population[index]->dataMap.GetAverage(key) << "  " << valRanges[key].first << "   " << valRanges[key].second << endl;;
		if (valRanges[key].second - valRanges[key].first == 0) {
			orgY = 0;
		}
		else {
			orgY = (population[index]->dataMap.GetAverage(key) - valRanges[key].first) / (valRanges[key].second - valRanges[key].first) * YSIZE;
		}
		if (orgX == XSIZE) {
			orgX = XSIZE - 1;
		}
		if (orgY == YSIZE) {
			orgY = YSIZE - 1;
		}
		int compareIndex = bestOrgs((int)orgX, (int)orgY);
		if (compareIndex == -1) {
			bestOrgs((int)orgX, (int)orgY) = index;
		} else if (population[index]->dataMap.GetAverage("score") > population[compareIndex]->dataMap.GetAverage("score")) {
			//cout << "   replaces " << bestOrgs((int)orgX, (int)orgY);
			bestOrgs((int)orgX, (int)orgY) = index;
		}
		//cout << endl;

	}
	cout << endl;
	bestOrgs.showGrid();

	// make a list of keepers!
	vector<int> keepers;

	for (int x = 0; x < XSIZE; x++) {
		for (int y = 0; y < YSIZE; y++) {
			if (bestOrgs(x, y) != -1) {
				keepers.push_back(bestOrgs(x, y));
			}
		}
	}

	//exit(1);
	*/

	aveScore /= population.size();

	int best = findGreatestInVector(Scores);
	double maxScore = Scores[best];
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
		/*if (keepers.size() > 0) {
			p1 = keepers[keepers.size() - 1];
			keepers.pop_back();
		}
		else {*/
			//cout << nextPopulation.size() << " < " << population.size() << endl;
			// chance for each pick that this org survives to the next population
			//cout << "picking p1..." << flush;
			if ((int)nextPopulation.size() < elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
				//cout << "E" << endl;
				p1 = best;
			}
			else {  // otherwise, p1 is the best of tournamentSize random picks
			 //cout << "R" << endl;
				p1 = Random::getIndex(population.size());
				for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
					challanger = Random::getIndex(population.size());
					if (Scores[challanger] > Scores[p1]) {
						p1 = challanger;
					}
				}
			}
		//}


		//cout << p1 << flush;
		//cout << "  picking p2..." << flush;

		orgSurvived = 0;  // clear orgSurvived. if population[p1] survives this will become 1 and we will not pick a mate
		if (Random::P(surviveRates[p1]) && survivors.find(population[p1]) == survivors.end()) {  // if this org survives and has not already survived
			surviveCount++;
			nextPopulation.push_back(population[p1]);  // push them to the next population
			orgSurvived = 1;
			survivors.insert(population[p1]);
		}
		
		if (!orgSurvived) {
			p2 = p1;  // make these the same to prime the while loop
			//while ((p1 == p2) || (population[p1]->gender == population[p2]->gender)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
			if ((int)nextPopulation.size() >= elitismLPL->lookup()) {  // if next population has less members then elitism, then p1 is best.
				//cout << "R" << endl;
				if (Random::P(selfRateLPL->lookup())) {
					// self!
					selfCount++;
					p2 = p1;
				}
				else { // mate wit other
					otherCount++;
					while ((p1 == p2)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
						//cout << p2 << " " << p1 << endl;
						p2 = Random::getIndex(population.size());
						//cout << "  " << p2 << " " << p1 << endl;

						for (int i = 0; i < tournamentSizeLPL->lookup() - 1; i++) {
							challanger = Random::getIndex(population.size());
							while ((p1 == challanger)) {  // if we pick p1, pick again (it is possible that p2 was select from less then tournament size, it the same challenger/p2 is picked twice
								challanger = Random::getIndex(population.size());
							}
							//cout << "  ch = " << challanger << "\n";
							if (Scores[challanger] > Scores[p2]) {
								//cout << "p2 assigned" << endl;
								p2 = challanger;
							}
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
	//for (auto org : population) {
	//	if (!survivors.count(org)) {  // if this org did not survive to the next generation, it must be killed (sad.)
	//		org->kill();
	//	}
	//}
	//population = nextPopulation;
	cout << "max = " << to_string(maxScore) << "   ave = " << to_string(aveScore) << "  survive,other,self: " << surviveCount << "," << otherCount << "," << selfCount;
	for (auto org : population) {
		org->dataMap.Set("Tournament2_numOffspring", org->offspringCount);
	}
	return nextPopulation;
}

