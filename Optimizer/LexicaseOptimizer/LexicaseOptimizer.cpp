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
#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <memory>


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
std::shared_ptr<ParameterLink<std::string>> LexicaseOptimizer::epsilonRelativeToPL =
Parameters::register_parameter("OPTIMIZER_LEXICASE-epsilonRelativeTo", (std::string)"rank",
	"determines how epsilon is calculated [rank,score]"
	"\nif rank, epsilon will be relative to organism ranks"
	"\n  i.e. keep (best current keepers count * epsilon) organisms"
	"\nif score epsilon will be relative to min and max score"
	"\n  i.e. keep orgs with score >= maxScore - ( (maxScore-minScore) * epsilon )"
);

std::shared_ptr<ParameterLink<int>> LexicaseOptimizer::poolSizePL = 
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
	if (epsilonRelativeToPL->get(PT) == "score") {
		epsilonRelativeTo = true;
	}
	else if (epsilonRelativeToPL->get(PT) == "rank") {
		epsilonRelativeTo = false;
	}
	else {
		std::cout << "  while setting up LexicaseOptimizer, found epsilonRelativeTo value \"" <<
			epsilonRelativeToPL->get(PT) << "\" but this value must be either \"score\" or \"rank\"."
			"\n  exiting.";
		exit(1);
	}
	poolSize = poolSizePL->get(PT);
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

// return n nonrepeating elements for [0..m)
auto m_choose_n = [](auto const m, auto const n) {
	std::vector<int> v(m);
	std::iota(std::begin(v), std::end(v), 0);
	std::shuffle(std::begin(v), std::end(v), Random::getCommonGenerator());
	std::vector<int> r;
	std::copy_n(std::begin(v), n, std::back_inserter(r));
	return r;
};


int LexicaseOptimizer::lexiSelect(const std::vector<int> &orgIndexList) {
	if (!scoresHaveDelta) { // if all scores are the same! pick random
		return Random::getIndex(orgIndexList.size());
	}

	// generate a vector with formulasSize and fill with values from 0 to formulasSize - 1, in a random order.
	std::vector<int> formulasOrder(optimizeFormulasMTs.size());
	iota(formulasOrder.begin(), formulasOrder.end(), 0);
	std::shuffle(formulasOrder.begin(), formulasOrder.end(), Random::getCommonGenerator());
	// now we have a random order in formulasOrder

	// keepers is the current list of indexes into population for orgs which
	// have passed all tests so far.
	std::vector<int> keepers  = orgIndexList;

	while (keepers.size() > 1 && formulasOrder.size() > 0) {
		// while there are still atleast one keeper and there are still formulas
		int formulaIndex = formulasOrder.back();
		formulasOrder.pop_back();

		double scoreCutoff;

		// create a vector of remaning scores
		std::vector<double> keeperScores;
		for (size_t i = 0; i < keepers.size(); i++) {
			keeperScores.push_back(scores[formulaIndex][keepers[i]]);
		}

		if (epsilonRelativeTo) { // get scoreCutoff relitive to score
			auto scoreRange = std::minmax_element(std::begin(keeperScores), std::end(keeperScores));
			scoreCutoff = (*scoreRange.second - ((*scoreRange.second - *scoreRange.first) * epsilon));
		}
		else { // get scoreCutoff relitive to rank
			// based on the number of keepers, calculate how many to keep.
			size_t cull_index = std::ceil(std::max((((1.0 - epsilon) * keeperScores.size()) - 1.0), 0.0));

			// get score at the cull index position
			std::nth_element(std::begin(keeperScores),
				std::begin(keeperScores) + cull_index,
				std::end(keeperScores));
			scoreCutoff = keeperScores[cull_index];

			//std::cout << "cull_index: " << cull_index << "  keeperScores.size(): " << keeperScores.size() << std::endl;
			//std::cout << "scoreCutoff: " << scoreCutoff << std::endl;
		}

		// for each keeper, see if there are still a keeper, i.e. they have score >= scoreCutoff. remove if not
		keepers.erase(std::remove_if(std::begin(keepers), std::end(keepers), [&](auto k) {
			return scores[formulaIndex][k] < scoreCutoff;
		}), std::end(keepers));
	}
	int pickHere = Random::getIndex(keepers.size()); 
	//std::cout << "    keeping: " << tournamentPopulation[keepers[pickHere]]->ID << std::endl;
	// if there is only one keeper left, return that, otherwise select randomly from keepers.
	return keepers[pickHere];
}


void LexicaseOptimizer::optimize(
    std::vector<std::shared_ptr<Organism>> &population) {

  std::vector<double> aveScores;
  aveScores.reserve(optimizeFormulasMTs.size());
  std::vector<double> maxScores;
  maxScores.reserve(optimizeFormulasMTs.size());
  std::vector<double> minScores;
  minScores.reserve(optimizeFormulasMTs.size());

  scoresHaveDelta = false;

  scores.clear();
  for (auto &opt_formula : optimizeFormulasMTs) {

    std::vector<double> pop_scores;
    pop_scores.reserve(population.size());

    for (auto &org : population)
      pop_scores.push_back(opt_formula->eval(org->dataMap, PT)[0]);

    scores.push_back(pop_scores);

    aveScores.push_back(
        std::accumulate(std::begin(pop_scores), std::end(pop_scores), 0.0) /
        population.size());

    auto const minmax =
        std::minmax_element(std::begin(pop_scores), std::end(pop_scores));

    scoresHaveDelta |= *minmax.first < *minmax.second;

    minScores.push_back(*minmax.first);
    maxScores.push_back(*minmax.second);
  }

  if (recordOptimizeValues)
    for (size_t i = 0; i < population.size(); i++)
      for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++)
        population[i]->dataMap.set(scoreNames[fIndex], scores[fIndex][i]);

  poolSize = poolSize == -1 ? population.size() : poolSize;


  size_t nextPopulationTargetSize = nextPopSizeFormula->eval(PT)[0];
  nextPopulationTargetSize = nextPopulationTargetSize == -1
                                 ? population.size()
                                 : nextPopulationTargetSize;

  // generate new organisms
  // do not add to population until all have been
  // selected
  newPopulation.clear();
  newPopulation.reserve(nextPopulationTargetSize);

  // generate a list of 'nextPopulationTargetSize' new orgs into 'newPopulation'
  // for each, generate a 'parents' vector with 'numberParents' parent orgs
  // parents are selected with lexiSelect( m_choose_n(population.size(), poolSize))
  //   where the m_choose_n command selects 'poolSize' number of population indexes
  std::generate_n(
      std::back_inserter(newPopulation), nextPopulationTargetSize, [&] {
        std::vector<std::shared_ptr<Organism>> parents;
        std::generate_n(std::back_inserter(parents), numberParents, [&] {
          return population[lexiSelect(m_choose_n(population.size(), poolSize))];
        });
        return parents[0]->makeMutatedOffspringFromMany(parents);
      });

  oldPopulation = population;
  population.insert(population.end(), newPopulation.begin(), newPopulation.end());
  for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
    std::cout << std::endl
              << "   " << scoreNames[fIndex]
              << ":  max = " << std::to_string(maxScores[fIndex])
              << "   ave = " << std::to_string(aveScores[fIndex]) << std::flush;
  }
}

void LexicaseOptimizer::cleanup(std::vector<std::shared_ptr<Organism>> &population) {
	for (auto org : oldPopulation) {
		org->kill();
	}
	population.swap(newPopulation);
	newPopulation.clear();
}

