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
	"cull organisms with score less then (((maxScore - minScore) * cullBelow) + minScore)\n  if -1, no culling. example, if .25, cull bottom 25%");
std::shared_ptr<ParameterLink<double>> SimpleOptimizer::cullRemapPL =
Parameters::register_parameter(
	"OPTIMIZER_SIMPLE-cullRemap", -1.0,
	"if cullBelow is being used (not -1) then remap scores between cullRemap and 1.0\n  The effect will be that the lowest score after culling will have a cullRemap % chance to be selected in Roulette");

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
  oldPopulationSize = (int)population.size();
  /////// MUST update to MTREE
  nextPopulationTargetSize = (int)nextPopSizeMT->eval(PT)[0];

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
  double deltaScore; // maxScore - cullBelow

  double cullBelow = cullBelowPL->get(PT); // -1 or [0,1] orgs who ((opVal - min) / (max - min)) < cullBelow are culled before selection
						 // culled orgs will not be automatically not be allowed to survive
						 // if -1 (default) then cullBelowScore = 0
  double cullRemap = cullRemapPL->get(PT); // -1 or [0,1] scores will be normalized between min and cullBelowScore score and then adjusted
							   // such that min score is the value
							   // if -1, no normalization will occur
  double cullBelowScore;

  std::vector<std::shared_ptr<Organism>> populationAfterCull;

  elites.clear();
  scores.clear();
  killList.clear();

  // get all scores
  for (int i = 0; i < (int)population.size(); i++) {
	  double opVal = optimizeValueMT->eval(population[i]->dataMap, PT)[0];
	  scores.push_back(opVal);
	  aveScore += opVal;
	  population[i]->dataMap.set("optimizeValue", opVal);
	  if (opVal > maxScore) {
		  maxScore = opVal;
	  }
	  if (opVal < minScore) {
		  minScore = opVal;
	  }
  }
  aveScore /= oldPopulationSize;

  auto checkScores = scores;
  
  if (cullBelow >= 0 && minScore != maxScore){ // cull and normalize scores if min == max then all scores are the same, do nothing!
	cullBelowScore = minScore + ((maxScore-minScore) * cullBelow);
	std::cout << "\n\nmax: " << maxScore << "   min: " << minScore;
	deltaScore = maxScore - cullBelowScore;
	std::cout << "  cullBelowScore: " << cullBelowScore << "  deltaScore: " << deltaScore << std::endl;
	for (int i = 0; i < (int)population.size(); i++) {
		std::cout << checkScores[i];
		if (scores[i] >= cullBelowScore) { // if not culled, nomalize score and add to culledPopulation
			populationAfterCull.push_back(population[i]);
			if (cullRemap == -1) { // no normaization
				scoresAfterCull.push_back(scores[i]);
				culledMinScore = minScore;
				culledMaxScore = maxScore;
			}
			else {
				scoresAfterCull.push_back((((scores[i] - cullBelowScore) / (deltaScore)) * (1.0 - cullRemap)) + cullRemap);
				culledMaxScore = 1; // all scores will be between 0 and 1
				culledMinScore = 0;
			}
			std::cout << " ->  " << scoresAfterCull[scoresAfterCull.size() - 1];
		}
		else { // if culled, add to kill list and DO NOT add to culled population
			killList.insert(population[i]);
			std::cout << " ->  culled";
		}
		std::cout << std::endl;
	}
	culledPopulationSize = populationAfterCull.size();
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
  int elitismRange = (int)elitismRangeMT->eval(PT)[0];
  int elitismCount = (int)elitismCountMT->eval(PT)[0];
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
        while ((int)parents.size() < numberParents) {
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

