//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "RouletteOptimizer.h"

std::shared_ptr<ParameterLink<int>> RouletteOptimizer::numberParentsPL =
	Parameters::register_parameter("OPTIMIZER_ROULETTE-numberParents", 1, "number of parents used to produce offspring (each parent will be selected by a unique tournament)");

std::shared_ptr<ParameterLink<std::string>> RouletteOptimizer::optimizeValuePL =
Parameters::register_parameter("OPTIMIZER_ROULETTE-optimizeValue", (std::string) "POW[1.05,DM_AVE[score]]", "value to optimize (MTree)");
std::shared_ptr<ParameterLink<std::string>> RouletteOptimizer::remapFunctionPL =
Parameters::register_parameter("OPTIMIZER_ROULETTE-remapFunction", (std::string) "NONE", "remap optimizeValue to affect strength of selection\n"
	"uses MTree, but adds the following options that can be used in place of MTree functions:\n"
	"$optVal$ = score of current organism\n"
	"$maxOptVal$ = maximum score in population\n"
	"$minOptVal$ = minimum score in population\n"
    "$aveOptVal$ = average score in population\n"
	"if NONE, no remap is preformed.");


int RouletteOptimizer::selectParent(std::vector<double>& scores, double maxScore, double minScore, int popSize){

	if (maxScore <= 0 || maxScore == minScore) {
		return Random::getIndex(popSize);
	}
	int parent;
	int countTries = 0;
	do {
		parent = Random::getIndex(popSize);
	} while (!Random::P(scores[parent] / maxScore));
	return parent;
}

void RouletteOptimizer::stringReplace(std::string& s, const std::string& search, const std::string& replace) {
	for (size_t pos = 0; ; pos += replace.length()) {
		// Locate the substd::string to replace
		pos = s.find(search, pos);
		if (pos == std::string::npos) break;
		// Replace by erasing and inserting
		s.erase(pos, search.length());
		s.insert(pos, replace);
	}
}

RouletteOptimizer::RouletteOptimizer(std::shared_ptr<ParametersTable> PT_)
	: AbstractOptimizer(PT_) {

	numberParents = numberParentsPL->get(PT);

	optimizeValueMT = stringToMTree(optimizeValuePL->get(PT));

	if (remapFunctionPL->get(PT) == "NONE") {
		doRemap = false;
	}
	else {
		doRemap = true;
		std::string remapString = remapFunctionPL->get(PT);
		stringReplace(remapString, "$minOptVal$", "VECT[0,0]");
		stringReplace(remapString, "$aveOptVal$", "VECT[0,1]");
		stringReplace(remapString, "$maxOptVal$", "VECT[0,2]");
		stringReplace(remapString, "$optVal$", "VECT[0,3]");
		remapFunctionMT = stringToMTree(remapString);
	}
	popFileColumns.clear();
	popFileColumns.push_back("optimizeValue");
	if (doRemap) {
		popFileColumns.push_back("remappedOptimizeValue");
	}
}

void RouletteOptimizer::optimize(std::vector<std::shared_ptr<Organism>>& population) {
	auto popSize = population.size();

	std::vector<double> scores(popSize, 0);
	std::vector<double> remappedScores(popSize, 0);
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

	std::vector<std::vector<double>> remapVect({ { minScore, aveScore, maxScore, 0 } });
	if (doRemap) {
		for (size_t i = 0; i < popSize; i++) {
			remapVect[0][3] = scores[i];
			remappedScores[i] = remapFunctionMT->eval(population[i]->dataMap, PT, remapVect)[0];
			population[i]->dataMap.set("remappedOptimizeValue", remappedScores[i]);
		}
	}
	else {
		remappedScores = scores;

	}

	double remappedScoresMax = *std::max_element(remappedScores.begin(), remappedScores.end());
	double remappedScoresMin = *std::min_element(remappedScores.begin(), remappedScores.end());

	std::vector<std::shared_ptr<Organism>> parents;

	for (int i = 0; i < popSize; i++) {
		if (numberParents == 1) {
			auto parent = population[selectParent(remappedScores, remappedScoresMax, remappedScoresMin, popSize)];
			population.push_back(parent->makeMutatedOffspringFrom(parent)); // add to population
		}
		else {
			parents.clear();
			do {
				parents.push_back(population[selectParent(remappedScores, remappedScoresMax, remappedScoresMin, popSize)]); // select from culled
			} while (static_cast<int>(parents.size()) < numberParents);
			population.push_back(parents[0]->makeMutatedOffspringFromMany(parents)); // push to population
		}

	}
	for (int i = 0; i < popSize; i++) {
		population[i]->dataMap.set("roulette_numOffspring", population[i]->offspringCount);
	}
	std::cout << "max = " << std::to_string(maxScore) << "   ave = " << std::to_string(aveScore) << "   min = " << std::to_string(minScore);
}
