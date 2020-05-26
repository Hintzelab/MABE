//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "TournamentOptimizer.h"

std::shared_ptr<ParameterLink<int>> TournamentOptimizer::tournamentSizePL =
	Parameters::register_parameter("OPTIMIZER_TOURNAMENT-tournamentSize", 5, "number of organisims compaired in each tournament");

std::shared_ptr<ParameterLink<int>> TournamentOptimizer::numberParentsPL =
	Parameters::register_parameter("OPTIMIZER_TOURNAMENT-numberParents", 1, "number of parents used to produce offspring (each parent will be selected by a unique tournament)");

std::shared_ptr<ParameterLink<bool>> TournamentOptimizer::minimizeErrorPL =
Parameters::register_parameter("OPTIMIZER_TOURNAMENT-minimizeError", false, "if true, Tournament Optimizer will select lower optimizeValues");

std::shared_ptr<ParameterLink<std::string>> TournamentOptimizer::optimizeValuePL =
	Parameters::register_parameter("OPTIMIZER_TOURNAMENT-optimizeValue", (std::string) "DM_AVE[score]", "value to optimize (MTree)");


int TournamentOptimizer::selectParent(int tournamentSize, bool minimizeError, std::vector<double> scores, int popSize){
	int winner, challanger;
	winner = Random::getIndex(popSize);
	for (int i = 0; i < tournamentSize - 1; i++) {
		challanger = Random::getIndex(popSize);
		if (minimizeError?scores[challanger] < scores[winner]:scores[challanger] > scores[winner]) {
			winner = challanger;
		}
	}
	return winner;
}


TournamentOptimizer::TournamentOptimizer(std::shared_ptr<ParametersTable> PT_)
	: AbstractOptimizer(PT_) {

	tournamentSize = tournamentSizePL->get(PT);
	numberParents = numberParentsPL->get(PT);
	minimizeError = minimizeErrorPL->get(PT);

	optimizeValueMT = stringToMTree(optimizeValuePL->get(PT));

	if (!minimizeError) {
		optimizeFormula = optimizeValueMT; // set this so Archivist knows which org is max
	}
	else {
		optimizeFormula = stringToMTree("0-("+optimizeValuePL->get(PT)+")");; // set this so Archivist knows which org is max
	}

	popFileColumns.clear();
	popFileColumns.push_back("optimizeValue");
}

void TournamentOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
	auto popSize = population.size();

	std::vector<double> scores(popSize, 0);
	double aveScore = 0;
	double maxScore = optimizeValueMT->eval(population[0]->dataMap, PT)[0];
	double minScore = maxScore;

	killList.clear();

	for (size_t i = 0; i < popSize; i++) {
		killList.insert(population[i]);
		double opVal = optimizeValueMT->eval(population[i]->dataMap, PT)[0];
		scores[i] = opVal;
		aveScore += opVal;
		population[i]->dataMap.set("optimizeValue", opVal);
		maxScore = std::max(maxScore, opVal);
		minScore = std::min(minScore, opVal);
	}
	
	aveScore /= popSize;

	std::vector<std::shared_ptr<Organism>> parents;

	for (int i = 0; i < popSize; i++) {
		if (numberParents == 1) {
			auto parent = population[selectParent(tournamentSize, minimizeError, scores, popSize)];
			population.push_back(parent->makeMutatedOffspringFrom(parent)); // add to population
		}
		else {
			parents.clear();
			parents.push_back(population[selectParent(tournamentSize, minimizeError, scores, popSize)]);
			while (static_cast<int>(parents.size()) < numberParents) {
				parents.push_back(population[selectParent(tournamentSize, minimizeError, scores, popSize)]); // select from culled
			}
			population.push_back(parents[0]->makeMutatedOffspringFromMany(parents)); // push to population
		}
	}

	for (int i = 0; i < popSize; i++) {
		population[i]->dataMap.set("tournament_numOffspring", population[i]->offspringCount);
	}

	if (!minimizeError) {
		std::cout << "max = " << std::to_string(maxScore) << "   ave = " << std::to_string(aveScore);
	}
	else {
		std::cout << "min = " << std::to_string(minScore) << "   ave = " << std::to_string(aveScore);
	}
}

