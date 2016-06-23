//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__BerryWorld__
#define __BasicMarkovBrainTemplate__BerryWorld__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "../AbstractWorld.h"

using namespace std;

class BerryWorld : public AbstractWorld {
private:
	int outputNodesCount, inputNodesCount;
public:
	const int numberOfDirections = 8;
	const int xm[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };  //these are directions
	const int ym[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	const char facingDisplay[8] = { 94, 47, 62, 92, 118, 47, 60, 92 };

	const int EMPTY = 0;
	const int WALL = 9;

	// Parameters
	static shared_ptr<ParameterLink<double>> TSKPL;
	static shared_ptr<ParameterLink<int>> worldUpdatesPL;
	static shared_ptr<ParameterLink<int>> foodTypesPL;
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

	static shared_ptr<ParameterLink<bool>> senseDownPL;
	static shared_ptr<ParameterLink<bool>> senseFrontPL;
	static shared_ptr<ParameterLink<bool>> senseFrontSidesPL;
	static shared_ptr<ParameterLink<bool>> senseWallsPL;

	static shared_ptr<ParameterLink<bool>> senseOtherPL;

	static shared_ptr<ParameterLink<bool>> clearOutputsPL;

	static shared_ptr<ParameterLink<int>> replacementPL;

	static shared_ptr<ParameterLink<bool>> recordConsumptionRatioPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListEatEmptyPL;
	static shared_ptr<ParameterLink<bool>> recordFoodListNoEatPL;

	static shared_ptr<ParameterLink<bool>> alwaysStartOnFood1PL;
	// end parameters



	int worldUpdates;

	double TSK;

	int foodTypes;

	double rewardForTurn;
	double rewardForMove;

	int WorldY;
	int WorldX;
	bool borderWalls;
	int randomWalls;

	bool allowMoveAndEat;

	bool senseDown;
	bool senseFront;
	bool senseFrontSides;
	bool senseWalls;

	bool senseOther;
	bool clearOutputs;

	int replacement;

	bool recordConsumptionRatio;
	bool recordFoodList;
	bool recordFoodListEatEmpty;
	bool recordFoodListNoEat;

	bool alwaysStartOnFood1;


	int foodRatioTotal;  // sum of ratioFood for foods in use
	vector<int> foodRatioLookup;
	vector<double> foodRewards;

	BerryWorld(shared_ptr<ParametersTable> _PT = nullptr);

	virtual void runWorld(shared_ptr<Group> group, bool analyse, bool visualize, bool debug) override;

	// if lastfood < 0, do not consider last food, pick randomly
	// if
	int pickFood(int lastfood) {
		//cout << "In BerryWorld::pickFood(int lastfood)\n";
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
				cout << "ERROR: In BerryWorld::pickFood() - lastfood > foodTypes (i.e. last food eaten is not in foodTypes!)\nExiting.\n\n";
				exit(1);
			}
			if (foodRatioTotal - foodRatioLookup[lastfood] == 0) {
				cout << "ERROR: In BerryWorld::pickFood() : lastfood is not <= 0, and foodTypes = 1.\nThere is only one foodType! Pick can not be a different foodType\n\nExiting";
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
		//cout << "  Leaving BerryWorld::pickFood(int lastfood)\n";
		return pick;

	}

	// convert x,y into a grid index
	int getGridIndexFromXY(pair<int, int> loc) {
		return loc.first + (loc.second * WorldX);
	}

	// return value on grid at index
	int getGridValue(const vector<int> &grid, int index) {
		return grid[index];
	}

	// return the value on grid at x,y
	int getGridValue(const vector<int> &grid, pair<int, int> loc) {
		return getGridValue(grid, getGridIndexFromXY(loc));
	}

	// takes x,y and updates them by moving one step in facing
	pair<int, int> moveOnGrid(pair<int, int> loc, int facing) {
		return {loopMod((loc.first + xm[facing]), WorldX), loopMod((loc.second + ym[facing]), WorldY)};
	}

	// update value at index in grid
	void setGridValue(vector<int> &grid, int index, int value) {
		grid[index] = value;
	}

	// update value at x,y in grid
	void setGridValue(vector<int> &grid, pair<int, int> loc, int value) {
		setGridValue(grid, getGridIndexFromXY(loc), value);
	}

	// return a vector of size x*y
	vector<int> makeGrid(int x, int y) {
		vector<int> grid;
		grid.resize(x * y);
		return grid;
	}

	// return a vector of size x*y (grid) with walls with borderWalls (if borderWalls = true) and randomWalls (that many) randomly placed walls
	vector<int> makeTestGrid() {
		vector<int> grid = makeGrid(WorldX, WorldY);

		for (int y = 0; y < WorldY; y++) {  // fill grid with food (and outer wall if needed)
			for (int x = 0; x < WorldX; x++) {
				if (borderWalls && (x == 0 || x == WorldX - 1 || y == 0 || y == WorldY - 1)) {
					setGridValue(grid, { x, y }, WALL);  // place walls on edge
				} else {
					setGridValue(grid, { x, y }, pickFood(-1));  // place random food where there is not a wall
				}
			}
		}

		if ((randomWalls >= WorldX * WorldY) && !borderWalls) {
			cout << "In BerryWorld::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}
		if ((randomWalls >= (WorldX - 2) * (WorldY - 2)) && borderWalls) {
			cout << "In BerryWorld::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}

		for (int i = 0; i < randomWalls; i++) {  // add random walls
			if (borderWalls) {
				setGridValue(grid, { Random::getInt(1, WorldX - 2), Random::getInt(1, WorldY - 2) }, WALL);  // if borderWalls than don't place random walls on the outer edge
			} else {
				setGridValue(grid, { Random::getIndex(WorldX), Random::getIndex(WorldY) }, WALL);  // place walls anywhere
			}
		}
		return grid;
	}

	inline int turnLeft(int facing) {
		return (facing < 1) ? numberOfDirections - 1 : facing - 1;
	}
	inline int turnRight(int facing) {
		return ((facing >= (numberOfDirections - 1)) ? 0 : facing + 1);
	}

	void printGrid(vector<int> grid, pair<int, int> loc, int facing);

	virtual int requiredInputs() override{
		return inputNodesCount;
	}
	virtual int requiredOutputs() override {
		return outputNodesCount;
	}

	virtual int maxOrgsAllowed() override {
		return -1;
	}

	virtual int minOrgsAllowed() override {
		return 1;
	}

	void SaveWorldState(string fileName, vector<int> grid, vector<pair<int, int>> currentLocation, vector<int> facing);
};

#endif /* defined(__BasicMarkovBrainTemplate__BerryWorld__) */
