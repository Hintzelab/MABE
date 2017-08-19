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

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include "../../Utilities/SensorArcs.h"

#include "../AbstractWorld.h"

using namespace std;

class BerryPlusWorld: public AbstractWorld {
private:
	int outputNodesCount, inputNodesCount;
public:
	Sensor visionSensor;
	Sensor smellSensor;
	int visionSensorCount;
	int smellSensorCount;

	// if numberOfDirections = 8, sensorCount = 3, and sensorPositions = -1,0,1
	// then there would be a forward, a 45 degree left and a 45 degree right sensor

	vector<double> deltaX; // offset in X and Y for each direction (precalculated in constructor)
	vector<double> deltaY;

	const int EMPTY = 0;
	const int WALL = 9;
	const char facingDisplay[8] = { 94, 47, 62, 92, 118, 47, 60, 92 };

	// Parameters
	static shared_ptr<ParameterLink<int>> numberOfDirectionsPL;

	static shared_ptr<ParameterLink<int>> visionSensorDistanceMaxPL;
	static shared_ptr<ParameterLink<int>> visionSensorArcSizePL;
	static shared_ptr<ParameterLink<string>> visionSensorPositionsStringPL;

	static shared_ptr<ParameterLink<int>> smellSensorDistanceMaxPL;
	static shared_ptr<ParameterLink<int>> smellSensorArcSizePL;
	static shared_ptr<ParameterLink<string>> smellSensorPositionsStringPL;

	static shared_ptr<ParameterLink<double>> timeCostMovePL;
	static shared_ptr<ParameterLink<double>> timeCostTurnPL;
	static shared_ptr<ParameterLink<double>> timeCostEatPL;
	static shared_ptr<ParameterLink<double>> timeCostNoActionPL;

	static shared_ptr<ParameterLink<double>> TSKPL;
	static shared_ptr<ParameterLink<int>> worldUpdatesPL;

	static shared_ptr<ParameterLink<int>> foodTypesPL;
	static shared_ptr<ParameterLink<double>> rewardForFood0PL;
	static shared_ptr<ParameterLink<double>> rewardForFood1PL;
	static shared_ptr<ParameterLink<double>> rewardForFood2PL;
	static shared_ptr<ParameterLink<double>> rewardForFood3PL;
	static shared_ptr<ParameterLink<double>> rewardForFood4PL;
	static shared_ptr<ParameterLink<double>> rewardForFood5PL;
	static shared_ptr<ParameterLink<double>> rewardForFood6PL;
	static shared_ptr<ParameterLink<double>> rewardForFood7PL;
	static shared_ptr<ParameterLink<double>> rewardForFood8PL;

	static shared_ptr<ParameterLink<double>> rewardForTurnPL;
	static shared_ptr<ParameterLink<double>> rewardForMovePL;

	static shared_ptr<ParameterLink<int>> ratioFood0PL;
	static shared_ptr<ParameterLink<int>> ratioFood1PL;
	static shared_ptr<ParameterLink<int>> ratioFood2PL;
	static shared_ptr<ParameterLink<int>> ratioFood3PL;
	static shared_ptr<ParameterLink<int>> ratioFood4PL;
	static shared_ptr<ParameterLink<int>> ratioFood5PL;
	static shared_ptr<ParameterLink<int>> ratioFood6PL;
	static shared_ptr<ParameterLink<int>> ratioFood7PL;
	static shared_ptr<ParameterLink<int>> ratioFood8PL;

	static shared_ptr<ParameterLink<int>> WorldYPL;
	static shared_ptr<ParameterLink<int>> WorldXPL;
	static shared_ptr<ParameterLink<bool>> borderWallsPL;
	static shared_ptr<ParameterLink<int>> randomWallsPL;

	static shared_ptr<ParameterLink<bool>> allowMoveAndEatPL;
	static shared_ptr<ParameterLink<bool>> alwaysEatPL;
	static shared_ptr<ParameterLink<double>> rewardSpatialNoveltyPL;

	static shared_ptr<ParameterLink<bool>> senseWallsPL;
	static shared_ptr<ParameterLink<bool>> senseOtherPL;

	static shared_ptr<ParameterLink<bool>> clearOutputsPL;

	static shared_ptr<ParameterLink<int>> replacementDefaultRulePL;
	static shared_ptr<ParameterLink<int>> replacementFood0PL;
	static shared_ptr<ParameterLink<int>> replacementFood1PL;
	static shared_ptr<ParameterLink<int>> replacementFood2PL;
	static shared_ptr<ParameterLink<int>> replacementFood3PL;
	static shared_ptr<ParameterLink<int>> replacementFood4PL;
	static shared_ptr<ParameterLink<int>> replacementFood5PL;
	static shared_ptr<ParameterLink<int>> replacementFood6PL;
	static shared_ptr<ParameterLink<int>> replacementFood7PL;
	static shared_ptr<ParameterLink<int>> replacementFood8PL;

	static shared_ptr<ParameterLink<bool>> recordConsumptionRatioPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListEatEmptyPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListNoEatPL;

	static shared_ptr<ParameterLink<int>> alwaysStartOnFoodPL;

	static shared_ptr<ParameterLink<bool>> saveOrgActionsPL;
	static shared_ptr<ParameterLink<string>> visualizationFileNamePL;

	static shared_ptr<ParameterLink<string>> mapFileListPL;
	static shared_ptr<ParameterLink<string>> mapFileWhichMapsPL;


	static shared_ptr<ParameterLink<int>> boarderEdgePL;

	static shared_ptr<ParameterLink<bool>> senseVisitedPL;

	static shared_ptr<ParameterLink<string>> fixedStartXRangePL;
	static shared_ptr<ParameterLink<string>> fixedStartYRangePL;
	static shared_ptr<ParameterLink<int>> fixedStartFacingPL;
	static shared_ptr<ParameterLink<bool>> relativeScoringPL;

	static shared_ptr<ParameterLink<int>> repeatsPL;
	static shared_ptr<ParameterLink<bool>> groupEvaluationPL;

	int repeats;
	bool groupEvaluation;

	int relativeScoring;

	int boarderEdge;
	bool senseVisited;

	int fixedStartXMin;
	int fixedStartXMax;

	int fixedStartYMin;
	int fixedStartYMax;
	int fixedStartFacing;

	// end parameters

	int worldUpdates;

	int numberOfDirections;

	int visionSensorDistanceMax;
	int visionSensorArcSize;
	vector<int> visionSensorPositions;

	int smellSensorDistanceMax;
	int smellSensorArcSize;
	vector<int> smellSensorPositions;

	double timeCostMove;
	double timeCostTurn;
	double timeCostEat;
	double timeCostNoAction;


	double TSK;

	int foodTypes;

	double rewardForTurn;
	double rewardForMove;

	int WorldY;
	int WorldX;
	bool borderWalls;
	int randomWalls;

	bool allowMoveAndEat;

	bool senseWalls;

	bool senseOther;
	bool clearOutputs;

	int replacementDefaultRule;
	vector<int> replacementRules;

	bool recordConsumptionRatio;
	bool recordFoodList;
	bool recordFoodListEatEmpty;
	bool recordFoodListNoEat;

	int alwaysStartOnFood;

	bool saveOrgActions;
	string visualizationFileName;

	bool alwaysEat;
	double rewardSpatialNovelty;
	vector<string> mapFileList;
	vector<string> mapFileWhichMaps;
	int foodRatioTotal;  // sum of ratioFood for foods in use
	vector<int> foodRatioLookup;
	vector<double> foodRewards;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;


	class WorldMap {
	public:
		shared_ptr<ParametersTable> PT;
		string fileName;
		string mapName;
		vector<char> grid;
		bool loadMap(ifstream& ss, const string fileName, shared_ptr<ParametersTable> parentPT);
	};

	map<string,map<string,WorldMap>> worldMaps; // [fileName][mapName]

	BerryPlusWorld(shared_ptr<ParametersTable> _PT);

	void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) override {
		//vector<double> scores(groups[groupName]->population.size(), 0);
		int groupSize = groups[groupNamePL->get(PT)]->population.size();
		if (groupEvaluation) {
			for (int r = 0; r < repeats; r++) {
				runWorld(groups[groupNamePL->get(PT)], analyse, visualize, debug);
//				for (int i = 0; i < groupSize; i++) {
//					scores[i] += groups["root"]->population[i]->score;
//				}
			}
		} else {
			vector<shared_ptr<Organism>> soloPopulation;
			shared_ptr<Group> soloGroup = make_shared<Group>(soloPopulation, groups[groupNamePL->get(PT)]->optimizer, groups[groupNamePL->get(PT)]->archivist);
			for (int i = 0; i < groupSize; i++) {
				soloGroup->population.clear();
				soloGroup->population.push_back(groups[groupNamePL->get(PT)]->population[i]);
				for (int r = 0; r < repeats; r++) {
					runWorld(soloGroup, analyse, visualize, debug);
					//scores[i] += groups["root"]->population[i]->score;
				}
			}
		}
//		for (size_t i = 0; i < groups["root"]->population.size(); i++) {
//			groups["root"]->population[i]->score = scores[i] / repeatsPL->lookup();
//		}

		if (visualize) {  // save endflag.
			FileManager::writeToFile(visualizationFileName, "*end*", "8," + to_string(WorldX) + ',' + to_string(WorldY));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
		}

	}



	virtual void runWorld(shared_ptr<Group> group, bool analyse, bool visualize, bool debug);

	// if lastfood < 0, do not consider last food, pick randomly
	// if
	int pickFood(int lastfood) {
		//cout << "In BerryWorldPlus::pickFood(int lastfood)\n";
		int lookup, counter, pick;
		if (lastfood < 0) {  // if lastfood is < 0 (or was 0) then return a random food
			lookup = Random::getInt(1, foodRatioTotal);  // get a random int [1,sum of food ratios]
			counter = foodRatioLookup[0];  // set the counter to the ratio of replace with empty (0)
			pick = 0;  // set pick to empty (0)
			while (counter < lookup) {
				pick++;  // this is not our pick, so advance to the next pick and...
				counter += foodRatioLookup[pick];  // add this new picks ratio to counter
			}
		} else {  // if given a last food, pick a food that is not that.
			if (lastfood > foodTypes) {
				cout << "ERROR: In BerryWorldPlus::pickFood() - lastfood > foodTypes (i.e. last food eaten is not in foodTypes!)\nExiting.\n\n";
				exit(1);
			}
			if (foodRatioTotal - foodRatioLookup[lastfood] == 0) {
				cout << "ERROR: In BerryWorldPlus::pickFood() : lastfood is not <= 0, and foodTypes = 1.\nThere is only one foodType! Pick can not be a different foodType\n\nExiting";
				exit(1);
			}
			lookup = Random::getInt(1, foodRatioTotal - foodRatioLookup[lastfood]);  // get a random int [1,sum of food ratios] but leave out the ratio of last food
			if (lastfood == 0) {  // if the last food was empty...
				counter = foodRatioLookup[1];  // set counter to ratio of food1 instead
				pick = 1;  // set pick to food1 (1)
			} else {
				counter = foodRatioLookup[0];  // set the counter to the ratio of replace with empty (0)
				pick = 0;  // set pick to empty (0)
			}
			while (counter < lookup) {
				pick++;  // this is not our pick, so advance to the next pick and...
				if (pick == lastfood) {
					pick++;  // if the new pick = lastfood, then skip it (we already removed the foodRatio for this food) and...
				}
				counter += foodRatioLookup[pick];  // add this new picks ratio to counter
			}
		}
		//cout << "  Leaving BerryWorldPlus::pickFood(int lastfood)\n";
		return pick;

	}

//	// convert x,y into a grid index
//	int getGridIndexFromXY(pair<int, int> loc) {
//		return loc.first + (loc.second * WorldX);
//	}
//
//	// return value on grid at index
//	int getGridValue(const vector<int> &grid, int index) {
//		return grid[index];
//	}
//
//	// return the value on grid at x,y
//	int getGridValue(const vector<int> &grid, pair<int, int> loc) {
//		return getGridValue(grid, getGridIndexFromXY(loc));
//	}

	// takes x,y and updates them by moving one step in facing
	pair<double, double> moveOnGrid(pair<double, double> loc, int facing) {
//		cout << "\n -----------------------"<<endl;
//		cout << loc.first << "," << loc.second << endl;
//		cout << deltaX[facing] << "," << deltaY[facing] << endl;
//		cout << loc.first + deltaX[facing] << "," << loc.second + deltaY[facing] << endl;
//		cout << loopModDouble((loc.first + deltaX[facing]), WorldX) << "," << loopModDouble((loc.second + deltaY[facing]), WorldY) << endl;

		return {loopModDouble((loc.first + deltaX[facing]), WorldX), loopModDouble((loc.second + deltaY[facing]), WorldY)};
	}

	// update value at index in grid
//	void setGridValue(vector<int> &grid, int index, int value) {
//		grid[index] = value;
//	}

	// update value at x,y in grid
//	void setGridValue(vector<int> &grid, pair<int, int> loc, int value) {
//		setGridValue(grid, getGridIndexFromXY(loc), value);
//	}

	// return a vector of size x*y
	vector<int> makeGrid(int x, int y) {
		vector<int> grid;
		grid.resize(x * y);
		return grid;
	}

	// return a vector of size x*y (grid) with walls with borderWalls (if borderWalls = true) and randomWalls (that many) randomly placed walls
	// if default > -1, fill grid with default value
	Vector2d<int> makeTestGrid(int defaultValue = -1) {
		Vector2d<int> grid(WorldX, WorldY);

		for (int y = 0; y < WorldY; y++) {  // fill grid with food (and outer wall if needed)
			for (int x = 0; x < WorldX; x++) {
				if (borderWalls && (x == 0 || x == WorldX - 1 || y == 0 || y == WorldY - 1)) {
					grid(x, y ) = WALL;  // place walls on edge
				} else if (defaultValue == -1) {
					if ((x >= boarderEdge && x <= WorldX - boarderEdge - 1) && (y >= boarderEdge && y <= WorldY - boarderEdge - 1)) {
						grid( x, y ) = pickFood(-1);  // place random food where there is not a wall, if it is not in the boarder edge
					}
				}
			}
		}

		if ((randomWalls >= WorldX * WorldY) && !borderWalls) {
			cout << "In BerryWorldPlus::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}
		if ((randomWalls >= (WorldX - 2) * (WorldY - 2)) && borderWalls) {
			cout << "In BerryWorldPlus::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}

		for (int i = 0; i < randomWalls; i++) {  // add random walls
			if (borderWalls) {
				grid(Random::getInt(1, WorldX - 2), Random::getInt(1, WorldY - 2)) = WALL;  // if borderWalls than don't place random walls on the outer edge
			} else {
				grid(Random::getIndex(WorldX), Random::getIndex(WorldY)) = WALL;  // place walls anywhere
			}
		}
		return grid;
	}

	inline int turnLeft(int facing, int offset = 1) {
		return loopMod(facing - offset,numberOfDirections);
	}
	inline int turnRight(int facing, int offset = 1) {
		return loopMod(facing + offset,numberOfDirections);
	}

	void printGrid(Vector2d<int> grid, pair<double, double> loc, int facing);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + "," + to_string(inputNodesCount) + "," + to_string(outputNodesCount) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

	void SaveWorldState(string fileName, Vector2d<int> grid, Vector2d<int> vistedGrid, vector<pair<double, double>> currentLocation, vector<int> facing, bool reset = false);
};
