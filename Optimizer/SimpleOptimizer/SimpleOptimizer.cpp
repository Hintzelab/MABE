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
        "how are parents selected? options: Roullette(),Tounament(size=VAL)");
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
                                   "value between 0 and 1, likelyhood that an "
                                   "organism will survive (MTree)");
std::shared_ptr<ParameterLink<std::string>> SimpleOptimizer::selfRatePL =
    Parameters::register_parameter("OPTIMIZER_SIMPLE-selfRate",
                                   (std::string) "0",
                                   "value between 0 and 1, likelyhood that an "
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
        "OPTIMIZER_SIMPLE-nextPopSize", (std::string) "0-1",
        "size of population after optimization(MTree). -1 indicates use "
        "current population size");

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

void SimpleOptimizer::optimize(
    std::vector<std::shared_ptr<Organism>> &population) {
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

  elites.clear();
  scores.clear();
  killList.clear();

  for (int i = 0; i < (int)population.size(); i++) {
    if (Random::P(surviveRateMT->eval(population[i]->dataMap, PT)[0])) {
      surviveCount++;
      nextPopulationSize++;
    } else {
      killList.insert(population[i]);
    }
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

  auto tempScores = scores;
  int elitismRange = (int)elitismRangeMT->eval(PT)[0];
  int elitismCount = (int)elitismCountMT->eval(PT)[0];
  for (int i = 0; i < elitismRange; i++) { // get handles for elite orgs
    elites.push_back(findGreatestInVector(tempScores));
    tempScores[elites.back()] = minScore;
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
         (currentElite < std::min(elitismRange, oldPopulationSize))) {
    currentCopy = 0;
    while ((nextPopulationSize < nextPopulationTargetSize) &&
           (currentCopy < elitismCount)) {
      population.push_back(
          population[elites[currentElite]]->makeMutatedOffspringFrom(
              population[elites[currentElite]]));
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
      auto parent = population[selector->select()];
      population.push_back(parent->makeMutatedOffspringFrom(parent));
    } else {
      parents.clear();
      parents.push_back(population[selector->select()]);
      if (Random::P(selfRateMT->eval(parents[0]->dataMap, PT)[0])) {
        population.push_back(parents[0]->makeMutatedOffspringFrom(parents[0]));
      } else {
        while ((int)parents.size() < numberParents) {
          parents.push_back(population[selector->select()]);
        }
        population.push_back(parents[0]->makeMutatedOffspringFromMany(parents));
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

