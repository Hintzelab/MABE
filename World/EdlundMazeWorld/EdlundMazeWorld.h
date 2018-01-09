//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#pragma once

#include "../AbstractWorld.h"
#include "../../Genome/CircularGenome/CircularGenome.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include <iomanip>

using namespace std;

namespace EdlundMaze {
    typedef std::vector<int> MazeDescription;
    const int T = 300;
    const int M_WIDTH = 18;
    const int M_MAXWIDTH = EdlundMaze::M_WIDTH-1;
    const int M_LENGTH = 50;
    const int M_MAXLENGTH = EdlundMaze::M_LENGTH-1;
    const int EMPTY = -1;
    const int SIGNAL_LEFT_OR_STRAIGHT = 0;
    const int SIGNAL_RIGHT = 1;

    const int S0_EYE_LEFT = 0;
    const int S1_EYE_CENTER = 1;
    const int S2_EYE_RIGHT = 2;
    const int S3_DOOR_CUE = 3;
    const int S4_TOUCH_LEFT = 4;
    const int S5_TOUCH_RIGHT = 5;
}

class EdlundMazeWorld : public AbstractWorld {

public:
	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;
	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
    static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	/// world-specific
    EdlundMaze::MazeDescription maze;
	EdlundMaze::MazeDescription signals;
	EdlundMaze::MazeDescription solutions; /// from each column, how many steps to final exit
	EdlundMaze::MazeDescription nextDoorLocations;
	int verticalSteps; /// used for solution, recalculated for each maze upon construction
	int horizontalSteps; /// used for solution
	void makeNewMaze();
	void makeTestMaze();
	int getNumberOfDoors();
	void findBestCaseSolutions();
	int getDistanceToExit(int,int);
	int getLongestShortestPath();
	float playGameAndReturnGofM();
	int totalDoorsPassed;
	int totalDoorsAchievable;
	string brainName;
    string groupName;

	EdlundMazeWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~EdlundMazeWorld() = default;
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse, visualize, debug);
		}
	}
	
	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",6,2"} } }; // default requires a root group and a brain (in root namespace) and no genome
	}
};

