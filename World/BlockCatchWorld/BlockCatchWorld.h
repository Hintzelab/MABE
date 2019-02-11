//
//  BlockCatch World
//  written by Cliff Bohm
//

// TO DO
// start positions, EVERY, EVERY_CLEAR, RANDOM, RANDOM_CLEAR
// FEEDBACK
//     play order ORDERED, RANDOM_PATTERN, RANDOM_ALL
//     feedback brain inputs
//     reset brain (between frames, between patterns)

#pragma once

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include "../../Genome/CircularGenome/CircularGenome.h"

class BlockCatchWorld : public AbstractWorld {
    
public:
	static std::shared_ptr<ParameterLink<std::string>> scoreMethodPL;
	static std::shared_ptr<ParameterLink<std::string>> paddlePL;

	static std::shared_ptr<ParameterLink<std::string>> catchPatternsPL;
	static std::shared_ptr<ParameterLink<std::string>> missPatternsPL;

	static std::shared_ptr<ParameterLink<int>> worldXMaxPL;
	static std::shared_ptr<ParameterLink<int>> worldXMinPL;
	static std::shared_ptr<ParameterLink<int>> startYMaxPL;
	static std::shared_ptr<ParameterLink<int>> startYMinPL;
	static std::shared_ptr<ParameterLink<std::string>> patternStartPositionsPL;

    static std::shared_ptr<ParameterLink<bool>> analyzeCognitiveNoiseRobustnessParameter;
    static std::shared_ptr<ParameterLink<bool>> analyzeGeneticNoiseRobustnessParameter;

	static std::shared_ptr<ParameterLink<int>> visualizeBestPL;
	int visualizeBest;

	static std::shared_ptr<ParameterLink<std::string>> groupNamePL;
	static std::shared_ptr<ParameterLink<std::string>> brainNamePL;

	int numberOfSensors = 0;
	int paddleWidth;
	std::vector<int> paddleShape;

    std::string blockPatternsString;
	int patternsCount;
	int catchPatternsCount;
	std::vector<std::vector<std::vector<int>>> allPatterns;
	std::vector<std::vector<int>> patternDirections; // 0 both, -1 left, +1 right;
	std::vector<size_t> patternSizes;


	std::string brainName;
	std::string groupName;
	
	double lastMax = 0;
	int lastMaxCount = 0;

	int worldXMax = 32;
	int worldXMin = 16;
	int startYMax = 48;
	int startYMin = 32;

	int patternStartPositions;
	int repeats = -1; // -1 = determine at runtime based on worldX and maybe paddle width

	int scoreMethod = 0;

    BlockCatchWorld (std::shared_ptr<ParametersTable> _PT = nullptr);
    ~BlockCatchWorld () = default;
	void evaluateSolo(std::shared_ptr<Organism> org, int analyse, int visualize, int debug);
	void evaluate(std::map<std::string, std::shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupName]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupName]->population[i], analyse, visualize, debug);
		}

		if (visualizeBest > 0 && Global::update % visualizeBest == 0 && Global::update > 0) {
			// get best org (org with best score)
			double bestScore = groups[groupNamePL->get(PT)]->population[0]->dataMap.getAverage("score");
			auto bestOrg = groups[groupNamePL->get(PT)]->population[0];
			for (auto org : groups[groupNamePL->get(PT)]->population) {
				double orgScore = org->dataMap.getAverage("score");
				if (bestScore != orgScore) {
					if (bestScore < orgScore) {
						bestOrg = org;
						bestScore = orgScore;
					}
				}
			}
			std::cout << "  running visualization for org with ID: " << bestOrg->ID << " and score: " << bestScore << std::endl;
			auto testOrg = bestOrg->makeCopy(bestOrg->PT); // make a copy so we don't mess up the data map
			evaluateSolo(testOrg, analyse, 1, debug);
		}
	}

	void debugDisplay(int worldX, int time, std::vector<std::vector<int>> patternBuffer, int frameIndex, std::vector<int> sensorArray, std::vector<int> gapArray);
	void visualizeDisplay(bool catchPattern, int worldX, int startYMax, int endTime, int time, int patternIndex, int catchPatternsCount,
		int correct, int incorrect, std::vector<std::vector<int>> patternBuffer, int frameIndex,
		std::vector<int> sensorArray, std::vector<int> gapArray);

	void loadPatterns(int& patternCounter, std::vector<std::string> patterns);
	virtual std::unordered_map<std::string, std::unordered_set<std::string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ","+std::to_string(numberOfSensors)+",2"} } };
	}

};
