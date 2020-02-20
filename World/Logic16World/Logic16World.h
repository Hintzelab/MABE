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

#include <World/AbstractWorld.h>

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

	virtual void evaluate(std::map<std::string, std::shared_ptr<Group>> &groups, int analyze, int visualize, int debug);
	void evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug);

	virtual std::unordered_map<std::string, std::unordered_set<std::string>>
		requiredGroups() override;
};
