//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "SimpleOptimizer.h"

std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::selectionMethodPL =
    Parameters::register_parameter(
        "OPTIMIZER_SIMPLE-selectionMethod", (std::string) "Roulette()",
        "how are parents selected? options: Roulette(),Tournament(size=VAL)");
std::shared_ptr<ParameterLink<int>> SimpleOptimizer::numberParentsPL =
    Parameters::register_parameter(
        "OPTIMIZER_SIMPLE-numberParents", 1,
        "number of parents used to produce offspring");

std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::optimizeValuePL =
    Parameters::register_parameter("OPTIMIZER_SIMPLE-optimizeValue",
                                   (std::string) "DM_AVE[score]",
                                   "value to optimize (MTree)");
std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::surviveRatePL =
    Parameters::register_parameter("OPTIMIZER_SIMPLE-surviveRate",
                                   (std::string) "0",
                                   "value between 0 and 1, probability that an "
                                   "organism will survive (MTree)");
std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::selfRatePL =
    Parameters::register_parameter("OPTIMIZER_SIMPLE-selfRate",
                                   (std::string) "0",
                                   "value between 0 and 1, probability that an "
                                   "organism will self (ignored if "
                                   "numberParents = 1) (MTree)");
std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::elitismCountPL =
    Parameters::register_parameter("OPTIMIZER_SIMPLE-elitismCount",
                                   (std::string) "1",
                                   "number of mutated offspring "
                                   "added to next population for "
                                   "each elite organism (MTree)");
std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::elitismRangePL =
    Parameters::register_parameter(
        "OPTIMIZER_SIMPLE-elitismRange", (std::string) "0",
        "number of elite organisms (i.e. if 5, then best 5) (MTree)");

std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::nextPopSizePL =
    Parameters::register_parameter(
        "OPTIMIZER_SIMPLE-nextPopSize", (std::string) "-1",
        "size of population after optimization(MTree). -1 indicates use "
        "current population size");

std::shared_ptr<ParameterLink<double>> SimpleOptimizer::cullBelowPL =
Parameters::register_parameter(
	"OPTIMIZER_SIMPLE-cullBelow", -1.0,
	"cull organisms with score less then (((maxScore - minScore) * cullBelow) + minScore)\nif -1, no culling.");
std::shared_ptr<ParameterLink<double>> SimpleOptimizer::cullRemapPL =
Parameters::register_parameter(
	"OPTIMIZER_SIMPLE-cullRemap", -1.0,
	"if cullBelow is being used (not -1) then remap scores between cullRemap and 1.0 so that the minimum score in the culled population is remapped to cullRemap and the high score is remapped to 1.0\nThe effect will be that the lowest score after culling will have a cullRemap % chance to kept if selected using the Roulette selection method");

std::shared_ptr<ParameterLink<bool>> SimpleOptimizer::cullByRangePL =
Parameters::register_parameter("OPTIMIZER_SIMPLE-cullRangeByRange", false,
	"if true cull will be relative to min and max score"
	"\n  i.e. cull organisms with score less then (((maxScore - minScore) * cullBelow) + minScore)"
	"\nif false, cull will be relative to organism ranks"
	"\n  i.e. find score of cullBelow*populaiton size best, and discard all orgs with lower score.");

SimpleOptimizer::SimpleOptimizer(std::shared_ptr<ParametersTable> PT_)
    : AbstractOptimizer(PT_) {

  selectionMethod = selectionMethodPL->get(PT);
  numberParents = numberParentsPL->get(PT);

  optimizeValueMT = stringToMTree(optimizeValuePL->get(PT));
  surviveRateMT = stringToMTree(surviveRatePL->get(PT));
  selfRateMT = stringToMTree(selfRatePL->get(PT));
  elitismCountMT = stringToMTree(elitismCountPL->get(PT));
  elitismRangeMT = stringToMTree(elitismRangePL->get(PT));
  nextPopSizeMT = stringToMTree(nextPopSizePL->get(PT));

  cullBelow = cullBelowPL->get(PT); // -1 or [0,1] orgs who ((opVal - min) / (max - min)) < cullBelow are culled before selection
									// culled orgs will not be automatically not be allowed to survive
									// if -1 (default) then cullBelowScore = 0
  cullRemap = cullRemapPL->get(PT); // -1 or [0,1] scores will be normalized between min and cullBelowScore score and then adjusted
									// such that min score is the value
									// if -1, no normalization will occur

  if (cullBelow != -1 && !(cullBelow >= 0 && cullBelow <= 1.0)) {
	  std::cout << "  in SimpleOptimizer constructor, found cullBelow value "
		  << cullBelow << " but cullBelow must be either -1 or in the range [0,1].\n  exiting." << std::endl;
	  exit(1);
  }
  if (cullRemap != -1 && !(cullRemap >= 0 && cullRemap <= 1.0)) {
	  std::cout << "  in SimpleOptimizer constructor, found cullRemap value "
		  << cullRemap << " but cullRemap must be either -1 or in the range [0,1].\n  exiting." << std::endl;
	  exit(1);
  }

  cullByRange = cullByRangePL->get(PT);;
  
  optimizeFormula = optimizeValueMT;

  std::vector<std::string> selectorArgs;
  std::stringstream ss(selectionMethod); // Turn the string into a stream.
  std::string tok;

  while (getline(ss, tok, '(')) {
    selectorArgs.push_back(tok);
  }

  selectorArgs[1].pop_back();
  // cout << "SimpleOptimizer method: " << selectorArgs[0] << "  " <<
  // selectorArgs[1] << endl;
  // cout << selectorArgs.size() << endl;
  if (selectorArgs[0] == "Tournament") {
    selector = std::make_shared<TournamentSelector>(selectorArgs[1], this);
  } else if (selectorArgs[0] == "Roulette") {
    selector = std::make_shared<RouletteSelector>(selectorArgs[1], this);
  } else {
    std::cout << "  in SimpleOptimizer constructor, selector method \""
              << selectionMethod << "\" is not known.\n  exiting..."
              << std::endl;
    exit(1);
  }

  popFileColumns.clear();
  popFileColumns.push_back("optimizeValue");
}

void SimpleOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
  oldPopulationSize = static_cast<int>(population.size());
  /////// MUST update to MTREE
  nextPopulationTargetSize = static_cast<int>(nextPopSizeMT->eval(PT)[0]);

  if (nextPopulationTargetSize == -1) {
    nextPopulationTargetSize = population.size();
  }
  /////// MUST update to MTREE

  nextPopulationSize = 0;

  selfCount = 0;
  eliteCount = 0;
  surviveCount = 0;

  aveScore = 0;
  maxScore = optimizeValueMT->eval(population[0]->dataMap, PT)[0];
  minScore = maxScore;
  auto scoresHaveDelta = false;

  double deltaScore; // maxScore - cullBelow
  
  double cullBelowScore;

  std::vector<std::shared_ptr<Organism>> populationAfterCull;
  scoresAfterCull.clear();

  elites.clear();
  scores.clear();
  killList.clear();

  // get all scores

  for (size_t i = 0; i < population.size(); i++) {
	  double opVal = optimizeValueMT->eval(population[i]->dataMap, PT)[0];
	  scores.push_back(opVal);
	  aveScore += opVal;
	  population[i]->dataMap.set("optimizeValue", opVal);
	  if (opVal > maxScore) {
		  maxScore = opVal;
		  scoresHaveDelta = true;
	  }
	  if (opVal < minScore) {
		  minScore = opVal;
		  scoresHaveDelta = true;
	  }
  }
  aveScore /= oldPopulationSize;
  
  if (cullBelow > -.5 && scoresHaveDelta){ // cull and normalize scores if min == max then all scores are the same, do nothing!
	culledMinScore = maxScore;
	culledMaxScore = maxScore;
	auto culledScoresHaveDetla = false;
	if (cullByRange) {
		cullBelowScore = minScore + ((maxScore - minScore) * cullBelow);
	} else { // cull not by range, but by rank position
		auto sortedScores = scores;
        auto cull_index = cullBelow * sortedScores.size() - 1;
        std::nth_element(std::begin(sortedScores),
                       std::begin(sortedScores) + cull_index,
                       std::end(sortedScores));
        cullBelowScore = sortedScores[cull_index];
		
		/* uncomment to see all scores
		for (auto ss : sortedScores) {
			std::cout << ss << " ";
		}
		std::cout << std::endl;
		*/

	}
	deltaScore = maxScore - cullBelowScore;
	//std::cout << "\n\nmax: " << maxScore << "   min: " << minScore;
	//std::cout << "  cullBelowScore: " << cullBelowScore << "  deltaScore: " << deltaScore << std::endl;
	for (size_t i = 0; i < population.size(); i++) {
		//std::cout << scores[i];
		if (scores[i] >= cullBelowScore) { // if not culled, add to culledPopulation
			populationAfterCull.push_back(population[i]);
			scoresAfterCull.push_back(scores[i]);
			if (scores[i] < culledMinScore) {
				culledMinScore = scores[i];
				culledScoresHaveDetla = true;
			}
			//std::cout << " ->  " << scoresAfterCull.back() << "   min: " << culledMinScore;
		}
		else { // if culled, add to kill list and DO NOT add to culled population
			killList.insert(population[i]);
			//std::cout << " ->  culled";
		}
		//std::cout << std::endl;
	}
	culledPopulationSize = static_cast<int>(populationAfterCull.size());
	if ((cullRemap > -.5) && (culledScoresHaveDetla)) { // normaization
		for (int i = 0; i < culledPopulationSize; i++) {
			//std::cout << "  remap: " << scoresAfterCull[i] << " ";
			scoresAfterCull[i] = (((scoresAfterCull[i] - culledMinScore) / (culledMaxScore - culledMinScore)) * (1.0 - cullRemap)) + cullRemap;
			//std::cout << scoresAfterCull[i] << std::endl;
		}
		culledMaxScore = 1; // all scores will be between 0 and 1
		culledMinScore = 0;
	}
  }
  else { // if not culling, don't worry, we are using population and scores as is.
	  populationAfterCull = population;
	  culledPopulationSize = oldPopulationSize;
	  scoresAfterCull = scores;
	  culledMinScore = minScore;
	  culledMaxScore = maxScore;
  }

  // figure out if an orgs survive
  for (int i = 0; i < culledPopulationSize; i++) {
	  if (Random::P(surviveRateMT->eval(populationAfterCull[i]->dataMap, PT)[0])) {
		  surviveCount++;
		  nextPopulationSize++;
	  }
	  else {
		  killList.insert(populationAfterCull[i]);
	  }
  }

  auto tempScores = scoresAfterCull;
  int elitismRange = static_cast<int>(elitismRangeMT->eval(PT)[0]);
  int elitismCount = static_cast<int>(elitismCountMT->eval(PT)[0]);
  for (int i = 0; i < elitismRange; i++) { // get handles for elite orgs
    elites.push_back(findGreatestInVector(tempScores));
    tempScores[elites.back()] = culledMinScore;
  }

  /*
  for (auto p : population) {
          cout << optimizeValueMT->eval(p->dataMap, PT)[0] << ", ";
  }
  cout << endl;
  for (auto elite : elites) {
          cout << population[elite]->ID << ":" <<
  optimizeValueMT->eval(population[elite]->dataMap, PT)[0] << "  ..." << endl;
  }
  */

  // first add elitism offspring for each of the best elitismRange organisms
  // (assuming there is room)
  int currentElite = 0;
  int currentCopy = 0;
  while ((nextPopulationSize < nextPopulationTargetSize) &&
         (currentElite < std::min(elitismRange, culledPopulationSize))) {
    currentCopy = 0;
    while ((nextPopulationSize < nextPopulationTargetSize) &&
           (currentCopy < elitismCount)) {
      population.push_back(
          populationAfterCull[elites[currentElite]]->makeMutatedOffspringFrom(
			  populationAfterCull[elites[currentElite]]));
      nextPopulationSize++;
      eliteCount++;
      currentCopy++;
    }
    currentElite++;
  }

  // now select parents for remainder of population
  std::vector<std::shared_ptr<Organism>> parents;
  while (nextPopulationSize < nextPopulationTargetSize) { // while we have not
                                                          // filled up the next
                                                          // generation
    if (numberParents == 1) {
      auto parent = populationAfterCull[selector->select()]; // select from culled
      population.push_back(parent->makeMutatedOffspringFrom(parent)); // add to population
    } else {
      parents.clear();
      parents.push_back(populationAfterCull[selector->select()]); // select from culled
      if (Random::P(selfRateMT->eval(parents[0]->dataMap, PT)[0])) {
        population.push_back(parents[0]->makeMutatedOffspringFrom(parents[0])); // push to population
      } else {
        while (static_cast<int>(parents.size()) < numberParents) {
          parents.push_back(populationAfterCull[selector->select()]); // select from culled
        }
        population.push_back(parents[0]->makeMutatedOffspringFromMany(parents)); // push to population
      }
    }
    nextPopulationSize++;
  }
  std::cout << "max = " << std::to_string(maxScore)
            << "   ave = " << std::to_string(aveScore);
  for (auto org : population) {
    // org->dataMap.Set("Simple_numOffspring", org->offspringCount);
  }
}

