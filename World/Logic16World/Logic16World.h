//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include "../AbstractWorld.h"

#include <cstdlib>
#include <thread>
#include <vector>

class Logic16World : public AbstractWorld {

public:
	static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
	static std::shared_ptr<ParameterLink<std::string>> brainNamePL;
	static std::shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
	static std::shared_ptr<ParameterLink<int>> brainUpdatesPL;
	static std::shared_ptr<ParameterLink<bool>> resetBrainBetweenInputsPL;
	static std::shared_ptr<ParameterLink<std::string>> testLogicPL;

	static std::shared_ptr<ParameterLink<int>> logicShuffleTimePL;
	static std::shared_ptr<ParameterLink<std::string>> logicShuffleMethodPL;

	static std::shared_ptr<ParameterLink<bool>> normalizeScorePL;

	bool normalizeScore;

	int logicShuffleTime;
	int logicShuffleMethod;

	std::string groupName;
	std::string brainName;

	std::vector<int> testLogic;
	std::vector<int> testLogicCounts;
	std::vector<int> testLogicUsed;

	bool resetBrainBetweenInputs;
	int brainUpdates;
	int evaluationsPerGeneration;

	int extraBrainInputs = 0; // these inputs are never set or used, but require the brain to find the input wires.

	int logic_tables[16][2][2] = {
		{{0, 0}, {0, 0}}, //0      FALSE
		{{1, 0}, {0, 0}}, //1   A  NOR  B
		{{0, 1}, {0, 0}}, //2  !A  AND  B
		{{1, 1}, {0, 0}}, //3  !A
		{{0, 0}, {1, 0}}, //4   A  AND !B
		{{1, 0}, {1, 0}}, //5          !B
		{{0, 1}, {1, 0}}, //6   A  XOR  B
		{{1, 1}, {1, 0}}, //7   A  NAND B
		{{0, 0}, {0, 1}}, //8   A  AND  B
		{{1, 0}, {0, 1}}, //9   A  NXOR B
		{{0, 1}, {0, 1}}, //10          B
		{{1, 1}, {0, 1}}, //11 !A  OR   B
		{{0, 0}, {1, 1}}, //12  A
		{{1, 0}, {1, 1}}, //13  A  OR  !B
		{{0, 1}, {1, 1}}, //14  A  OR   B
		{{1, 1}, {1, 1}}  //15     TRUE
	};

	double lastMax = 0;
	int lastMaxCount = 0;

	std::vector<std::string> logicNames = { "FALSE","a_NOR_b","!a_AND_b","!a","a_AND_!b","!b","a_XOR_b","a_NAND_b","a_AND_b","a_NXOR_b","b","!a_OR_b","a","a_OR_!b","a_OR_b","TRUE" };

	int questions[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };

	Logic16World(std::shared_ptr<ParametersTable> PT_ = nullptr);
	virtual ~Logic16World() = default;
	void evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug);

	virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug) {
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

	virtual std::unordered_map<std::string, std::unordered_set<std::string>>
		requiredGroups() override {
		// agents in this world will need 2 inputs, and a number of outputs = to the number of logic tests
		std::cout << "Logic16 world requires brains with 2 inputs and " << testLogic.size() << " outputs." << std::endl;
		return { {groupNamePL->get(PT),
				 {"B:" + brainNamePL->get(PT) + "," + std::to_string(2+extraBrainInputs) +"," + std::to_string(testLogic.size())}} };
	}
};
