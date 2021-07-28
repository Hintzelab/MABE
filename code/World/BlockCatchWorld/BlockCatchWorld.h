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

#include <World/AbstractWorld.h>

#include "../../Utilities/Utilities.h"
//#include "../../Brain/MarkovBrain/MarkovBrain.h"
//#include "../../Brain/RNNBrain/RNNBrain.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include <Genome/CircularGenome/CircularGenome.h>
#include <algorithm>    // std::rotate

#include "../../Analyze/brainTools.h"
#include "../../Analyze/smearedness.h"

class BlockCatchWorld : public AbstractWorld {
    
public:
	static std::shared_ptr<ParameterLink<int>> testMutantsPL;

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

	static std::shared_ptr<ParameterLink<bool>> saveFragOverTimePL;
	static std::shared_ptr<ParameterLink<bool>> saveBrainStructureAndConnectomePL;
	static std::shared_ptr<ParameterLink<bool>> saveStateToStatePL;
	static std::shared_ptr<ParameterLink<bool>> save_R_FragMatrixPL;
	static std::shared_ptr<ParameterLink<bool>> saveFlowMatrixPL;

	bool saveFragOverTime;
	bool saveBrainStructureAndConnectome;
	bool saveStateToState;
	bool save_R_FragMatrix;
	bool saveFlowMatrix;


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
	
	int testMutants = 0;

	double lastMax = 0;
	int lastMaxCount = 0;

	int worldXMax = 32;
	int worldXMin = 16;
	int startYMax = 48;
	int startYMin = 32;

	int patternStartPositions;
	size_t repeats = 0; // -1 = determine at runtime based on worldX and maybe paddle width

	int scoreMethod = 0;

    BlockCatchWorld (std::shared_ptr<ParametersTable> _PT = nullptr);
    ~BlockCatchWorld () = default;
	void evaluateSolo(std::shared_ptr<Organism> org, int analyse, int visualize, int debug);
	void evaluate(std::map<std::string, std::shared_ptr<Group>>& groups, int analyse, int visualize, int debug);

	void debugDisplay(int worldX, int time, std::vector<std::vector<int>> patternBuffer, int frameIndex, std::vector<int> sensorArray, std::vector<int> gapArray);
	void visualizeDisplay(bool catchPattern, int worldX, int startYMax, int endTime, int time, int patternIndex, int catchPatternsCount,
		int correct, int incorrect, std::vector<std::vector<int>> patternBuffer, int frameIndex,
		std::vector<int> sensorArray, std::vector<int> gapArray);

	void loadPatterns(int& patternCounter, std::vector<std::string> patterns);
	virtual std::unordered_map<std::string, std::unordered_set<std::string>> requiredGroups() override;

	// given a map and key, if key in map, map[key]++ else map[key] = 1
	template<typename T>
	void mapIncrement(std::unordered_map<T, int> & data, T key) {
		if (data.find(key) == data.end()) { // key is not in map, make a new entry
			data[key] = 1;
		}
		else {
			data[key] += 1;
		}
	}
};
