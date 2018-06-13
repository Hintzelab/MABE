//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "LexicaseOptimizer.h"

std::shared_ptr<ParameterLink<std::string>> LexicaseOptimizer::optimizeFormulasPL =
    Parameters::register_parameter("OPTIMIZER_LEXICASE-optimizeFormulas",
	(std::string) "DM_AVE[score]",
    "values to optimize with lexicase selection (list of MTrees)\n"
	"example for BerryWorld: [DM_AVE[food1],DM_AVE[food2],(0-DM_AVE[switches])]");

std::shared_ptr<ParameterLink<std::string>> LexicaseOptimizer::optimizeFormulaNamesPL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-optimizeFormulaNames",
	(std::string) "default",
	"column names to associate with optimize formulas in data files."
	"\n'default' will auto generate names as optimizeValue_1, optimizeValue_2, ...");

std::shared_ptr<ParameterLink<std::string>> LexicaseOptimizer::nextPopSizePL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-nextPopSize", (std::string) "-1",
	"size of population after optimization(MTree). -1 indicates use current population size");

std::shared_ptr<ParameterLink<int>> LexicaseOptimizer::numberParentsPL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-numberParents", 1,
	"number of parents used to produce offspring");

std::shared_ptr<ParameterLink<double>> LexicaseOptimizer::epsilonPL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-epsilon", .1,
	"cutoff when conducting per formula selection.\n"
	"e.g. 0.1 = organisms in the top 90% are kept. use 0.0 for classic Lexicase.");
std::shared_ptr<ParameterLink<bool>> LexicaseOptimizer::epsilonByRangePL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-epsilonByRange", false,
	"if true epsilon will be relative to min and max score"
	"\n  i.e. keep orgs with (score > maxScore - (maxScore-minScore) * epsilon)"
	"\nif false, epsilon will be relative to organism ranks"
	"\n  i.e. keep (best current keepers * epsilon) organisms");

std::shared_ptr<ParameterLink<int>> LexicaseOptimizer::tournamentSizePL = 
Parameters::register_parameter("OPTIMIZER_LEXICASE-poolSize", -1,
	"number of organisms used when selecting parent(s) in the lexicase algorithm, -1 indicates to use entire population");

std::shared_ptr<ParameterLink<bool>> LexicaseOptimizer::recordOptimizeValuesPL = 
Parameters::register_parameter("OPTIMIZER_LEXICASE-recordOptimizeValues", true,
	"record optimize values to data files using optimizeFormulaNames");


LexicaseOptimizer::LexicaseOptimizer(std::shared_ptr<ParametersTable> PT_)
    : AbstractOptimizer(PT_) {

	std::vector<std::string> optimizeFormulasStrings;
	convertCSVListToVector(optimizeFormulasPL->get(PT), optimizeFormulasStrings);

	for (auto s : optimizeFormulasStrings) {
		optimizeFormulasMTs.push_back(stringToMTree(s));
	}

	// get names to use with scores
	if (optimizeFormulaNamesPL->get(PT) == "default") {
		// user has not defined names, auto generate names
		for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
			scoreNames.push_back("optimizeValue_" + std::to_string(fIndex));
		}
	}
	else {
		// user has defined names, use those
		convertCSVListToVector(optimizeFormulaNamesPL->get(PT), scoreNames);
	}


	epsilon = epsilonPL->get(PT);
	epsilonByRange = epsilonByRangePL->get(PT);
	tournamentSize = tournamentSizePL->get(PT);
	nextPopSizeFormula = stringToMTree(nextPopSizePL->get(PT));
	numberParents = numberParentsPL->get(PT);
	recordOptimizeValues = recordOptimizeValuesPL->get(PT);
	
	// leave this undefined so that max.csv is not generated
	//optimizeFormula = optimizeValueMT;

	popFileColumns.clear();
	if (recordOptimizeValues){
		for (auto &name : scoreNames) {
			popFileColumns.push_back(name);
		}
	}
}

int LexicaseOptimizer::lexiSelect(const std::vector<int> &tournamentIndexList) {
	if (!scoresHaveDelta) { // if all scores are the same! pick random
		return Random::getIndex(tournamentIndexList.size());
	}

	// generate a vector with formulasSize and fill with values from 0 to formulasSize in a random order.
	std::vector<int> formulasOrder(optimizeFormulasMTs.size());
	iota(formulasOrder.begin(), formulasOrder.end(), 0);
	std::shuffle(formulasOrder.begin(), formulasOrder.end(), Random::getCommonGenerator());
	// now we have a random order in formulasOrder

	// keepers is the current list of indexes into population for orgs which have passed all tests.
	std::vector<int> keepers  = tournamentIndexList;

	while (keepers.size() > 1 && formulasOrder.size() > 0) {
		// while there are still atleast one keeper and there are still formulas
		int formulaIndex = formulasOrder.back();
		formulasOrder.pop_back();

		double scoreCutoff;

		if (epsilonByRange) {
			// get the range of scores
			double maxScoreHere = scores[keepers[0]][formulaIndex];
			double minScoreHere = maxScoreHere;
			for (size_t i = 0; i < keepers.size(); i++) {
				maxScoreHere = std::max(scores[keepers[i]][formulaIndex], maxScoreHere);
				minScoreHere = std::min(scores[keepers[i]][formulaIndex], minScoreHere);
			}
			// get scoreCutoff relivite to min and max
			scoreCutoff = (maxScoreHere - ((maxScoreHere - minScoreHere) * epsilon));
		}
		else { // not epsilonByRange
			// create a vector of remaning scores and sort them
			std::vector<double> keeperScores;
			for (size_t i = 0; i < keepers.size(); i++) {
				keeperScores.push_back(scores[keepers[i]][formulaIndex]);
			}
			
			auto cull_index = std::ceil(std::max((((1.0 - epsilon) * keeperScores.size()) - 1.0), 0.0));

			std::nth_element(std::begin(keeperScores),
				std::begin(keeperScores) + cull_index,
				std::end(keeperScores));
			scoreCutoff = keeperScores[cull_index];

			//std::cout << "cull_index: " << cull_index << "  keeperScores.size(): " << keeperScores.size() << std::endl;
			//std::cout << "scoreCutoff: " << scoreCutoff << std::endl;
		}
		// for each keeper, see if there are still a keeper, i.e. they have score >= scoreCutoff
		for (size_t i = 0; i < keepers.size();) {
			if (scores[keepers[i]][formulaIndex] >= scoreCutoff) {
				i++; // this is a keeper
			}
			else {
				keepers[i] = keepers.back();
				keepers.pop_back(); // this is not a keeper!
			}
		}
	}
	int pickHere = Random::getIndex(keepers.size());
	//std::cout << "    keeping: " << tournamentPopulation[keepers[pickHere]]->ID << std::endl;
	return keepers[pickHere];
}

void LexicaseOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
	int oldPopulationSize = static_cast<int>(population.size());
	int nextPopulationTargetSize = nextPopSizeFormula->eval(PT)[0];
	if (nextPopulationTargetSize == -1) {
		nextPopulationTargetSize = population.size();
	}

	int nextPopulationSize = 0;

	// initialize ave,max and, min vectors
	std::vector<double> aveScores(optimizeFormulasMTs.size());
	std::vector<double> maxScores(optimizeFormulasMTs.size());
	std::vector<double> minScores(optimizeFormulasMTs.size());
	for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
		aveScores[fIndex] = 0;
		maxScores[fIndex] = optimizeFormulasMTs[fIndex]->eval(population[0]->dataMap, PT)[0];
		minScores[fIndex] = maxScores.back();
	}

	killList.clear(); // will hold orgs which will be cleaned up after archive
	scoresHaveDelta = false; // if all scores are the same, then random selection will result

	// get all scores and record to dataMap

	// reset scores each update (population size may have changed...
	scores = std::vector<std::vector<double>>(population.size(), std::vector<double>(optimizeFormulasMTs.size(), 0));

	for (size_t i = 0; i < population.size(); i++) {
		// for each org in population
		killList.insert(population[i]); // add to kill list so that they are deleted in cleanup step
		for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
			scores[i][fIndex] = optimizeFormulasMTs[fIndex]->eval(population[i]->dataMap, PT)[0];
			aveScores[fIndex] += scores[i][fIndex];
			if (recordOptimizeValues){
				population[i]->dataMap.set(scoreNames[fIndex], scores.back().back());
			}
			if (scores[i][fIndex] > maxScores[fIndex]) {
				maxScores[fIndex] = scores[i][fIndex];
				scoresHaveDelta = true;
			}
			if (scores[i][fIndex] < minScores[fIndex]) {
				minScores[fIndex] = scores[i][fIndex];
				scoresHaveDelta = true;
			}
		}
	}

	for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
		aveScores[fIndex] /= oldPopulationSize;
	}
	
	// hold newly generated orgs - do not add to population until all have been selected
	std::vector<std::shared_ptr<Organism>> newPopulation;

	// temp vector used to pass tournamentSize randomly selected orgs to lexicase algorithm
	std::vector<int> tournamentIndexList;

	// initialize tournamentIndexList to whole population
	for (size_t i = 0; i < population.size(); i++) {
		tournamentIndexList.push_back(i);
	}

	std::shared_ptr<Organism> parent; // for asexual
	std::vector<std::shared_ptr<Organism>> parents; // for sexual

	while (nextPopulationSize < nextPopulationTargetSize) {
		if (numberParents == 1) { // asexual reproduction
			if (tournamentSize == -1) {
				parent = population[lexiSelect(tournamentIndexList)];
			}
			else {
				tournamentIndexList.clear();
				for (int i = 0; i < tournamentSize; i++) {
					tournamentIndexList.push_back(Random::getIndex(population.size()));
				}
				parent = population[lexiSelect(tournamentIndexList)];
			}
			newPopulation.push_back(parent->makeMutatedOffspringFrom(parent)); // push to population
		}
		else { // sexual reproduction
			parents.clear();
			while (static_cast<int>(parents.size()) < numberParents) {
				if (tournamentSize == -1) {
					parents.push_back(population[lexiSelect(tournamentIndexList)]);
				}
				else {
					tournamentIndexList.clear();
					for (int i = 0; i < tournamentSize; i++) {
						tournamentIndexList.push_back(Random::getIndex(population.size()));
					}
					parents.push_back(population[lexiSelect(tournamentIndexList)]);
				}
			}
			newPopulation.push_back(parents[0]->makeMutatedOffspringFromMany(parents));
		}
		nextPopulationSize++;
	}
	
	for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
		std::cout << std::endl << "   " << scoreNames[fIndex] << ":  max = " << std::to_string(maxScores[fIndex])
			<< "   ave = " << std::to_string(aveScores[fIndex]) << std::flush;
	}

	// add new orgs to population so they are visable to the archivist
	for (auto org : newPopulation) {
		population.push_back(org);
	}
}

