//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// A single agent is evaluated to perform the xor logic operation,
// and is tested on each standard 2-bit pattern: 0^0, 0^1, 1^0, 1^1
// it receives 1.0 point for each correct computation.

#include "Logic16World.h"
#include <Utilities/Utilities.h>
#include <cmath>

std::shared_ptr<ParameterLink<std::string>> Logic16World::groupNamePL =
Parameters::register_parameter("WORLD_LOGIC16_NAMES-groupName", (std::string) "root::", "name of group to be evaluated");

std::shared_ptr<ParameterLink<std::string>> Logic16World::brainNamePL =
Parameters::register_parameter("WORLD_LOGIC16_NAMES-brainName", (std::string) "root::", "namespace for parameters used to define brain");

std::shared_ptr<ParameterLink<int>> Logic16World::evaluationsPerGenerationPL =
Parameters::register_parameter("WORLD_LOGIC16-evaluationsPerGeneration", 1,
	"Number of times to test each organism per generation (useful with non-deterministic brains)");

std::shared_ptr<ParameterLink<int>> Logic16World::brainUpdatesPL =
Parameters::register_parameter("WORLD_LOGIC16-brainUpdates", 1,
	"Number of brain updates run for each world input.");

std::shared_ptr<ParameterLink<std::string>> Logic16World::testLogicPL =
Parameters::register_parameter("WORLD_LOGIC16-testLogic", (std::string) "1,2,4,6,7,8,9,11,13,14",
	"comma seperated list of logic used to test each organism. Values may be repeated in this list.\n"
	"Values are:\n"
	"   0: FALSE       1: A NOR B      2: !A AND B    3: !A \n"
	"   4: A AND !B    5: !B           6: A XOR B     7: A NAND B \n"
	"   8: A AND B     9: A NXOR B    10: B          11: !A OR B \n"
	"  12: A          13: A OR !B     14: A OR B     15: TRUE");

std::shared_ptr<ParameterLink<bool>> Logic16World::resetBrainBetweenInputsPL =
Parameters::register_parameter("WORLD_LOGIC16-resetBrainBetweenInputs",
	true, "call brain reset between inputs (each brain is tested with all four possible inputs - 00, 01, 10, 11, is brain reset between each input?)");

std::shared_ptr<ParameterLink<int>> Logic16World::logicShuffleTimePL = Parameters::register_parameter("WORLD_LOGIC16-logicShuffleTime",
	100, "if logicShuffleMethod is not NONE, how often should the environment shuffle?");

std::shared_ptr<ParameterLink<std::string>> Logic16World::logicShuffleMethodPL = Parameters::register_parameter("WORLD_LOGIC16-logicShuffleMethod",
	(std::string)"NONE", "should the logic associated with each output change over time,\n"
	"NONE = no, constent environment\n"
	"RANDOM = randomly reasign which logic is expected by each output\n      (irregularlay changing environment)\n"
	"SLIDE = slide the output expectations one position to the right and wrap the last value\n      (regularlay changing environment)");

std::shared_ptr<ParameterLink<bool>> Logic16World::normalizeScorePL = Parameters::register_parameter("WORLD_LOGIC16-normalizeScore",
	true, "if true, scores will be normalized between 0 and 1. if false, every correct output will add 1 to score - each test logic will add up to 4.");

Logic16World::Logic16World(std::shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {

	groupName = groupNamePL->get(PT);
	brainName = brainNamePL->get(PT);
	brainUpdates = brainUpdatesPL->get(PT);
	resetBrainBetweenInputs = resetBrainBetweenInputsPL->get(PT);
	convertCSVListToVector(Logic16World::testLogicPL->get(PT), testLogic);

	// get a count of the number of each logic being tested. used to normalize per logic scores.
	testLogicCounts.resize(16, 0);
	for (auto l : testLogic) {
		testLogicCounts[l]++;
	}

	// which logics are being tested?
	for (int i = 0; i < testLogicCounts.size(); i++) {
		if (testLogicCounts[i] > 0) {
			testLogicUsed.push_back(i);
		}
	}

	evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);

	normalizeScore = normalizeScorePL->get(PT);

	logicShuffleTime = logicShuffleTimePL->get(PT);
	if (logicShuffleMethodPL->get(PT) == "NONE") {
		logicShuffleMethod = 0;
	}
	else if (logicShuffleMethodPL->get(PT) == "RANDOM") {
		logicShuffleMethod = 1;
	}
	else if (logicShuffleMethodPL->get(PT) == "SLIDE") {
		logicShuffleMethod = 2;
	}
	else {
		std::cout << "  While setting up Logic16 world, found undefined logicShuffleMethod parameter value \"" <<
			logicShuffleMethodPL->get(PT) << "\" this paramter must be NONE, SLIDE or RANDOM./n  exiting.";
		exit(1);
	}

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	for (auto logic : testLogic) {
		popFileColumns.push_back(logicNames[logic]+"_score");
	}
}

void Logic16World::evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug) {
	// at update 1000 mess it all up
	if (logicShuffleMethod > 0 && Global::update%logicShuffleTime == 0 && Global::update!= 0) {
		// if there is a shuffleMethod, and it's a shuffle time and not time 0 shuffle logic...
		std::vector<int> testLogicNew(testLogic.size());
		if (logicShuffleMethod == 1) { // RANDOM
			std::cout << "testLogic shuffle (RANDOM): ";
			for (auto v : testLogic) {
				std::cout << v << ",";
			}

			int pick;
			for (int i = testLogic.size() - 1; i >= 0; i--) {
				pick = Random::getInt(0, i);
				testLogicNew[i] = testLogic[pick];
				testLogic[pick] = testLogic[i];
			}

			std::cout << "  ->  ";
			for (auto v : testLogicNew) {
				std::cout << v << ",";
			}
			std::cout << std::endl;

			testLogic = testLogicNew;
		}
		else if (logicShuffleMethod == 2) { // SLIDE
			std::cout << "testLogic shuffle (SLIDE): ";

			for (int i = testLogic.size() - 1; i >= 0; i--) {
				if (i == 0) {
					testLogicNew[i] = testLogic.back();
				}
				else {
					testLogicNew[i] = testLogic[i - 1];
				}
			}

			for (auto v : testLogic) {
				std::cout << v << ",";
			}
			std::cout << "  ->  ";
			for (auto v : testLogicNew) {
				std::cout << v << ",";
			}
			std::cout << std::endl;

			testLogic = testLogicNew;
		} // else do nothing, we already checked for bad shuffle type in constructor
	}

	int popSize = groups[groupName]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups[groupName]->population[i], analyze, visualize, debug);
	}
}


void Logic16World::evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug) {

	auto brain = org->brains[brainName];

	std::vector<double> logicScores;
	logicScores.resize(16);

	for (int repeats = evaluationsPerGeneration; repeats > 0; --repeats) {
		brain->resetBrain();
		for (int InputIndex = 0; InputIndex < 4; InputIndex++) {

			if (resetBrainBetweenInputs) {
				brain->resetBrain();
			}

			bool in0 = questions[InputIndex][0];
			bool in1 = questions[InputIndex][1];

			brain->setInput(0, in0);
			brain->setInput(1, in1);

			for (int i = 0; i < brainUpdates; i++) { // call update on brain one or more times
				brain->update();
			}

			int outputCount = 0;
			for (auto logic : testLogic) {
				// for each logic being tested, see if the brain generated the correct output for the current input
				logicScores[logic] += (double)(logic_tables[logic][in0][in1] == Bit(brain->readOutput(outputCount++)));
			}
		}
	}

	double score = 0;
	for (auto logic : testLogicUsed) {
		// collect scores
		score += logicScores[logic];
		// for each logic being tested, save that logics score normaized based on 4 tests per evaluation and number of evaluations
		org->dataMap.set(logicNames[logic] + "_score", ((logicScores[logic] / 4.0) / testLogicCounts[logic]) / (double)evaluationsPerGeneration);
	}
	if (normalizeScore) {
		// normalize score based on 4 tests per evaluation, number of logic tests, and number of evaluations per generation
		org->dataMap.set("score", (((score / 4.0)) / (double)testLogic.size()) / (double)evaluationsPerGeneration);
	}
	else {
		org->dataMap.set("score", score / (double)evaluationsPerGeneration);
	}
}

std::unordered_map<std::string, std::unordered_set<std::string>>
Logic16World::requiredGroups() {
  // agents in this world will need 2 inputs, and a number of outputs = to the number of logic tests
  std::cout << "Logic16 world requires brains with 2 inputs and " << testLogic.size() << " outputs." << std::endl;
  return { {groupNamePL->get(PT),
        {"B:" + brainNamePL->get(PT) + "," + std::to_string(2+extraBrainInputs) +"," + std::to_string(testLogic.size())}} };
}

