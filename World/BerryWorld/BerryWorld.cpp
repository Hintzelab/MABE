//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "BerryWorld.h"

shared_ptr<ParameterLink<double>> BerryWorld::TSKPL = Parameters::register_parameter("WORLD_BERRY-taskSwitchingCost", 1.4, "cost to change food sources");
shared_ptr<ParameterLink<int>> BerryWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_BERRY-worldUpdates", 400, "amount of time a brain is tested");

shared_ptr<ParameterLink<int>> BerryWorld::foodTypesPL = Parameters::register_parameter("WORLD_BERRY-foodTypes", 2, "number of types of food");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood1PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood1", 1.0, "reward for eating a Food1");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood2PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood2", 1.0, "reward for eating a Food2");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood3PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood3", 1.0, "reward for eating a Food3");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood4PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood4", 1.0, "reward for eating a Food4");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood5PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood5", 1.0, "reward for eating a Food5");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood6PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood6", 1.0, "reward for eating a Food6");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood7PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood7", 1.0, "reward for eating a Food7");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood8PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood8", 1.0, "reward for eating a Food8");

shared_ptr<ParameterLink<double>> BerryWorld::rewardForTurnPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardForTurn", 0.0, "reward for turning");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForMovePL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardForMove", 0.0, "reward for moving");

shared_ptr<ParameterLink<int>> BerryWorld::ratioFood0PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood0", 0, "Relative likelihood to leave empty space empty");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood1PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood1", 1, "Relative likelihood to place Food1");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood2PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood2", 1, "Relative likelihood to place Food2");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood3PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood3", 1, "Relative likelihood to place Food3");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood4PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood4", 1, "Relative likelihood to place Food4");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood5PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood5", 1, "Relative likelihood to place Food5");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood6PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood6", 1, "Relative likelihood to place Food6");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood7PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood7", 1, "Relative likelihood to place Food7");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood8PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood8", 1, "Relative likelihood to place Food8");

shared_ptr<ParameterLink<int>> BerryWorld::WorldXPL = Parameters::register_parameter("WORLD_BERRY-worldX", 8, "world X size");
shared_ptr<ParameterLink<int>> BerryWorld::WorldYPL = Parameters::register_parameter("WORLD_BERRY-worldY", 8, "world Y size");
shared_ptr<ParameterLink<bool>> BerryWorld::borderWallsPL = Parameters::register_parameter("WORLD_BERRY-makeBorderWalls", true, "if true world will have a bounding wall");
shared_ptr<ParameterLink<int>> BerryWorld::randomWallsPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-makeRandomWalls", 0, "add this many walls to the world");

shared_ptr<ParameterLink<bool>> BerryWorld::clearOutputsPL = Parameters::register_parameter("WORLD_BERRY-clearOutputs", false, "if true outputs will be cleared on each world update");

shared_ptr<ParameterLink<bool>> BerryWorld::allowMoveAndEatPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-allowMoveAndEat", false, "if true, the brain can move and eat in the same world update");
shared_ptr<ParameterLink<bool>> BerryWorld::senseDownPL = Parameters::register_parameter("WORLD_BERRY-senseDown", true, "if true, Agent can sense what it's standing on");
shared_ptr<ParameterLink<bool>> BerryWorld::senseFrontPL = Parameters::register_parameter("WORLD_BERRY-senseFront", true, "if true, Agent can sense what's in front of it");
shared_ptr<ParameterLink<bool>> BerryWorld::senseFrontSidesPL = Parameters::register_parameter("WORLD_BERRY-senseFrontSides", false, "if true, Agent can sense what's in front to the left and right of it");
shared_ptr<ParameterLink<bool>> BerryWorld::senseWallsPL = Parameters::register_parameter("WORLD_BERRY-senseWalls", false, "if true, Agent can sense Walls");
shared_ptr<ParameterLink<bool>> BerryWorld::senseOtherPL = Parameters::register_parameter("WORLD_BERRY-senseOther", false, "if true, Agents will be able to sense other agents (if there are other agents).");

shared_ptr<ParameterLink<int>> BerryWorld::replacementPL = Parameters::register_parameter("WORLD_BERRY-replacement", -1, "-1 = random, 0 = nPL = eplacement, 1 = replace other (note: Food0, can be replace by Food0)");

shared_ptr<ParameterLink<bool>> BerryWorld::recordConsumptionRatioPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-recordConsumptionRatio", false, "if true, record greater of red/blue+1 or blue/red+1");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListPL = Parameters::register_parameter("WORLD_BERRY-recordFoodList", true, "if true, record list of food eaten");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListEatEmptyPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListEatEmpty", false, "if true, foodList will include attempts to eat 0");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListNoEatPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListNoEat", false, "if true, if true foodList will include no eat (-1)");

shared_ptr<ParameterLink<bool>> BerryWorld::alwaysStartOnFood1PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-alwaysStartOnFood1", false, "if true, if true organisms will all start on food1");

BerryWorld::BerryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_BERRY-worldUpdates");

	foodTypes = (PT == nullptr) ? foodTypesPL->lookup() : PT->lookupInt("WORLD_BERRY-foodTypes");

	TSK = (PT == nullptr) ? TSKPL->lookup() : PT->lookupDouble("WORLD_BERRY-taskSwitchingCost");

	rewardForTurn = (PT == nullptr) ? rewardForTurnPL->lookup() : PT->lookupDouble("WORLD_BERRY_ADVANCED-rewardForTurn");
	rewardForMove = (PT == nullptr) ? rewardForMovePL->lookup() : PT->lookupDouble("WORLD_BERRY_ADVANCED-rewardForMove");

	WorldY = (PT == nullptr) ? WorldYPL->lookup() : PT->lookupInt("WORLD_BERRY-worldY");
	WorldX = (PT == nullptr) ? WorldXPL->lookup() : PT->lookupInt("WORLD_BERRY-worldX");
	borderWalls = (PT == nullptr) ? borderWallsPL->lookup() : PT->lookupBool("WORLD_BERRY-makeBorderWalls");
	randomWalls = (PT == nullptr) ? randomWallsPL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-makeRandomWalls");

	allowMoveAndEat = (PT == nullptr) ? allowMoveAndEatPL->lookup() : PT->lookupBool("WORLD_BERRY_ADVANCED-allowMoveAndEat");

	senseDown = (PT == nullptr) ? senseDownPL->lookup() : PT->lookupBool("WORLD_BERRY-senseDown");
	senseFront = (PT == nullptr) ? senseFrontPL->lookup() : PT->lookupBool("WORLD_BERRY-senseFront");
	senseFrontSides = (PT == nullptr) ? senseFrontSidesPL->lookup() : PT->lookupBool("WORLD_BERRY-senseFrontSides");
	senseWalls = (PT == nullptr) ? senseWallsPL->lookup() : PT->lookupBool("WORLD_BERRY-senseWalls");

	senseOther = (PT == nullptr) ? senseOtherPL->lookup() : PT->lookupBool("WORLD_BERRY-senseOther");

	clearOutputs = (PT == nullptr) ? clearOutputsPL->lookup() : PT->lookupBool("WORLD_BERRY-clearOutputs");

	replacement = (PT == nullptr) ? replacementPL->lookup() : PT->lookupInt("WORLD_BERRY-replacement");

	recordConsumptionRatio = (PT == nullptr) ? recordConsumptionRatioPL->lookup() : PT->lookupBool("WORLD_BERRY_ADVANCED-recordConsumptionRatio");
	recordFoodList = (PT == nullptr) ? recordFoodListPL->lookup() : PT->lookupBool("WORLD_BERRY-recordFoodList");
	recordFoodListEatEmpty = (PT == nullptr) ? recordFoodListEatEmptyPL->lookup() : PT->lookupBool("WORLD_BERRY-recordFoodListEatEmpty");
	recordFoodListNoEat = (PT == nullptr) ? recordFoodListNoEatPL->lookup() : PT->lookupBool("WORLD_BERRY-recordFoodListNoEat");

	alwaysStartOnFood1 = (PT == nullptr) ? alwaysStartOnFood1PL->lookup() : PT->lookupBool("WORLD_BERRY_ADVANCED-alwaysStartOnFood1");

	if (foodTypes < 1 || foodTypes > 8) {
		cout << "In BerryWorld you either have too few or too many foodTypes (must be >0 and <=8)\n\nExiting\n\n";
		exit(1);
	}

	outputNodesCount = 3;  // number of brain nodes used for output, 2 for move, 1 for eat

	//if (senseWalls) {
	inputNodesCount = senseDown * foodTypes + ((senseFront * foodTypes) + senseWalls + senseOther) + (2 * ((senseFrontSides * foodTypes) + senseWalls + senseOther));
	////// sense down does not include walls (can't stand on a wall (yet!) * types of food
	////// senseFront * types of food + wall, same for senseFrontSides, but there are 2
	//} else {  // no border walls
	//	inputNodesCount = (senseDown * foodTypes) + (senseFront * foodTypes) + (2 * (senseFrontSides * foodTypes));
	////// sense down * types of food, same for senseFront, same for senseFrontSides, but there are 2
	//}

	cout << "BerryWorld requires brains with at least " << inputNodesCount + outputNodesCount << " nodes.\n";
	if (inputNodesCount == 0) {
		cout << "    " << inputNodesCount << " Inputs\t No Inputs\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes 0 to " << outputNodesCount - 1 << "\n";
	} else {
		cout << "    " << inputNodesCount << " Inputs\t nodes 0 to " << inputNodesCount - 1 << "\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes " << inputNodesCount << " to " << inputNodesCount + outputNodesCount - 1 << "\n";

	}
	foodRatioLookup.resize(9);  // stores reward of each type of food NOTE: food is indexed from 1 so 0th entry is chance to leave empty
	foodRatioLookup[0] = (PT == nullptr) ? ratioFood0PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood0");
	foodRatioLookup[1] = (PT == nullptr) ? ratioFood1PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood1");
	foodRatioLookup[2] = (PT == nullptr) ? ratioFood2PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood2");
	foodRatioLookup[3] = (PT == nullptr) ? ratioFood3PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood3");
	foodRatioLookup[4] = (PT == nullptr) ? ratioFood4PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood4");
	foodRatioLookup[5] = (PT == nullptr) ? ratioFood5PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood5");
	foodRatioLookup[6] = (PT == nullptr) ? ratioFood6PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood6");
	foodRatioLookup[7] = (PT == nullptr) ? ratioFood7PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood7");
	foodRatioLookup[8] = (PT == nullptr) ? ratioFood8PL->lookup() : PT->lookupInt("WORLD_BERRY_ADVANCED-replacementRatioFood8");

	foodRatioTotal = 0;
	for (int i = 0; i <= foodTypes; i++) {
		foodRatioTotal += foodRatioLookup[i];
	}

	foodRewards.resize(9);  // stores reward of each type of food
	foodRewards[0] = 0;
	foodRewards[1] = (PT == nullptr) ? rewardForFood1PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood1");
	foodRewards[2] = (PT == nullptr) ? rewardForFood2PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood2");
	foodRewards[3] = (PT == nullptr) ? rewardForFood3PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood3");
	foodRewards[4] = (PT == nullptr) ? rewardForFood4PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood4");
	foodRewards[5] = (PT == nullptr) ? rewardForFood5PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood5");
	foodRewards[6] = (PT == nullptr) ? rewardForFood6PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood6");
	foodRewards[7] = (PT == nullptr) ? rewardForFood7PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood7");
	foodRewards[8] = (PT == nullptr) ? rewardForFood8PL->lookup() : PT->lookupDouble("WORLD_BERRY-rewardForFood8");

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("total");
	if (foodTypes > 1) {
		aveFileColumns.push_back("switches");
	}
	for (int i = 0; i <= foodTypes; i++) {
		string temp_name = "food" + to_string(i);  // make food names i.e. food1, food2, etc.
		aveFileColumns.push_back(temp_name);
	}
	if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
		aveFileColumns.push_back("consumptionRatio");
	}
}

void BerryWorld::printGrid(vector<int> grid, pair<int, int> loc, int facing) {
	for (int y = 0; y < WorldY; y++) {
		for (int x = 0; x < WorldX; x++) {
			if ((x == loc.first) && (y == loc.second)) {
				cout << facingDisplay[facing] << " ";
			} else {
				if (getGridValue(grid, { x, y }) == WALL) {
					cout << "X";
				} else {
					cout << getGridValue(grid, { x, y });
				}
				cout << " ";
			}
		}
		cout << "\n";
	}
	cout << "\n";
}

void BerryWorld::runWorld(shared_ptr<Group> group, bool analyse, bool visualize, bool debug) {
	vector<double> scores(group->population.size(), 0);

	vector<int> grid = makeTestGrid();
	vector<int> orgPositionsGrid;
	orgPositionsGrid.resize(WorldX * WorldY);

	vector<pair<int, int>> currentLocation;
	vector<int> facing;
	if ((int) group->population.size() > ((borderWalls) ? ((WorldX - 2) * (WorldY - 2) - randomWalls) : ((WorldX) * (WorldY) - randomWalls))) {
		cout << "Berry world is too small. There are more organisms then space in the world.\n";
		string RW = (borderWalls) ? "on" : "off";
		int totalSpaces = (borderWalls) ? ((WorldX - 2) * (WorldY - 2) - randomWalls) : ((WorldX) * (WorldY) - randomWalls);
		cout << "World is " << WorldX << " by " << WorldY << ". Border walls are " << RW << " and there are " << randomWalls << " random walls.\n";
		cout << "Total number of spaces in the world = " << totalSpaces << "\nPopulation size = " << group->population.size() << endl;
		cout << "Increase WORLD_BERRY-worldX and/or WORLD_BERRY-worldY or run with smaller population." << endl;
		cout << "exiting." << endl;
		exit(1);
	}
	for (int i = 0; i < (int) group->population.size(); i++) {
		pair<int, int> newLocation = { Random::getIndex(WorldX), Random::getIndex(WorldY) };
		int c = 0;

		if (alwaysStartOnFood1) {
			while ((getGridValue(orgPositionsGrid, newLocation) == 1 || getGridValue(grid, newLocation) == WALL) || getGridValue(grid, newLocation) != 1) {
				newLocation = {Random::getIndex(WorldX), Random::getIndex(WorldY)};
				c++;
				if (c % 1000000 == 0) {
					cout << "Warning! One million attempts were made to place an organism, your world may be too small!" << endl;
				}
			}
		} else {
			while (getGridValue(orgPositionsGrid, newLocation) == 1 || getGridValue(grid, newLocation) == WALL) {
				newLocation = {Random::getIndex(WorldX), Random::getIndex(WorldY)};
				c++;
				if (c % 1000000 == 0) {
					cout << "Warning! One million attempts were made to place an organism, your world may be too small!" << endl;
					cout << c << endl;
				}
			}
		}
		currentLocation.push_back(newLocation);  // location of the organism
		facing.push_back(Random::getIndex(8));  // direction the agent is facing
		setGridValue(orgPositionsGrid, newLocation, 1);
	}

	//printGrid(grid,{0,0},0);
	//printGrid(orgPositionsGrid,{0,0},0);

	// set up to track what food is eaten
	vector<int> switches(group->population.size(), 0);  // number of times organism has switched food source
	vector<int> lastFood(group->population.size(), -1);  //nothing has been eaten yet!
	vector<int> foodHereOnArrival(group->population.size(), 0);
	vector<vector<int>> eaten;  // stores number of each type of food was eaten in total for this test. [0] stores number of times org attempted to eat on empty location
	eaten.resize(group->population.size());
	for (int i = 0; i < (int) group->population.size(); i++) {
		foodHereOnArrival[i] = getGridValue(grid, currentLocation[i]);  //value of the food when we got here - needed for replacement method.
		eaten[i].resize(foodTypes + 1);
		if (recordFoodList) {
			group->population[i]->dataMap.Append("foodList", -2);  // -2 = a world initialization, -1 = did not eat this step
		}
		group->population[i]->brain->resetBrain();
	}

	// set up vars needed to run
	int output1 = 0;  // store outputs from brain
	int output2 = 0;

	int here, leftFront, front, rightFront;  // store grid values relitive to organsism
	int otherLeftFront, otherFront, otherRightFront;  // store grid values relitive to organsism

	int nodesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs

	vector<int> stateCollector;  // used in the phi calculation

	// make sure the brain does not have values from last run

	vector<int> orgList;
	int orgListIndex;
	int orgIndex;

	if (visualize) {  // save state of world before we get started.
		BerryWorld::SaveWorldState("worldUpdatesFile.txt", grid, currentLocation, facing);
	}

	for (int t = 0; t < worldUpdates; t++) {  //run agent for "worldUpdates" brain updates
		orgList.clear();
		for (int i = 0; i < (int) group->population.size(); i++) {
			orgList.push_back(i);
		}
		while (orgList.size() > 0) {
			orgListIndex = Random::getIndex(orgList.size());
			orgIndex = orgList[orgListIndex];
			orgList[orgListIndex] = orgList[orgList.size() - 1];
			orgList.pop_back();

			here = getGridValue(grid, currentLocation[orgIndex]);
			front = getGridValue(grid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex]));
			leftFront = getGridValue(grid, moveOnGrid(currentLocation[orgIndex], turnLeft(facing[orgIndex])));
			rightFront = getGridValue(grid, moveOnGrid(currentLocation[orgIndex], turnRight(facing[orgIndex])));


			nodesAssignmentCounter = 0;  // get ready to start assigning inputs
			if (senseWalls) {
				if (senseDown) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (here == i + 1));
					}
				}
				if (senseFront) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (front == i + 1));
					}
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (front == WALL));
				}
				if (senseFrontSides) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
					}
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (leftFront == WALL));
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (rightFront == WALL));
				}
			} else {  // don't sense walls
				if (senseDown) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (here == i + 1));
					}
				}
				if (senseFront) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (front == i + 1));
					}
				}
				if (senseFrontSides) {
					for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
					}
				}
			}

			if (senseOther) {
				otherFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex]));
				otherLeftFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], turnLeft(facing[orgIndex])));
				otherRightFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], turnRight(facing[orgIndex])));

				if (senseFront) {
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, otherFront);
				}
				if (senseFrontSides) {
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, otherLeftFront);
					group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, otherRightFront);

				}
			}
			if (debug) {
				cout << "\n----------------------------\n";
				cout << "\ngeneration update: " << Global::update << "  world update: " << t << "\n";
				cout << "currentLocation: " << currentLocation[orgIndex].first << "," << currentLocation[orgIndex].second << "  :  " << facing[orgIndex] << "\n";
				cout << "inNodes: ";
				for (int i = 0; i < inputNodesCount; i++) {
					cout << group->population[orgIndex]->brain->readInput(i) << " ";
				}
				cout << "\nlast outNodes: ";
				for (int i = 0; i < outputNodesCount; i++) {
					cout << group->population[orgIndex]->brain->readOutput(i) << " ";
				}
				cout << "\n\n  -- brain update --\n\n";
			}

			// inputNodesCount is now set to the first output Brain State Address. we will not move it until the next world update!
			if (clearOutputs) {
				group->population[orgIndex]->brain->resetOutputs();
			}

			group->population[orgIndex]->brain->update();  // just run the update!

			// set output values
			// output1 has info about the first 2 output bits these [00 eat, 10 left, 01 right, 11 move]
			output1 = Bit(group->population[orgIndex]->brain->readOutput(0)) + (Bit(group->population[orgIndex]->brain->readOutput(1)) << 1);
			// output 2 has info about the 3rd output bit, which either does nothing, or is eat.
			output2 = Bit(group->population[orgIndex]->brain->readOutput(2));

			if (output2 == 1) {  // if org tried to eat
				int foodHere = getGridValue(grid, currentLocation[orgIndex]);
				if ((recordFoodList && foodHere != 0) || (recordFoodList && recordFoodListEatEmpty)) {
					group->population[orgIndex]->dataMap.Append("foodList", foodHere);  // record that org ate food (or tried to at any rate)
				}
				eaten[orgIndex][foodHere]++;  // track the number of each berry eaten, including 0s
				if (foodHere != EMPTY) {  // eat food here (if there is food here)
					if (lastFood[orgIndex] != -1) {  // if some food has already been eaten
						if (lastFood[orgIndex] != foodHere) {  // if this food is different then the last food eaten
							scores[orgIndex] -= TSK;  // pay the task switch cost
							switches[orgIndex]++;
						}
					}
					lastFood[orgIndex] = foodHere;  // remember the last food eaten
					scores[orgIndex] += foodRewards[foodHere];  // you ate a food... good for you!
					setGridValue(grid, currentLocation[orgIndex], 0);  // clear this location
				}
			} else {
				if (recordFoodList && recordFoodListNoEat) {
					group->population[orgIndex]->dataMap.Append("foodList", -1);  // record that org did not try to eat this time
				}
			}

			if ((output2 == 0) || (allowMoveAndEat == 1)) {  // if we did not eat or we allow moving and eating in the same world update
				switch (output1) {
				case 0:  //nothing
					break;
				case 1:  //turn left
					facing[orgIndex] = turnLeft(facing[orgIndex]);
					scores[orgIndex] += rewardForTurn;
					break;
				case 2:  //turn right
					facing[orgIndex] = turnRight(facing[orgIndex]);
					scores[orgIndex] += rewardForTurn;
					break;
				case 3:  //move forward
					if (getGridValue(grid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex])) != WALL && getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex])) != 1) {  // if the proposed move is not a wall and is not occupied by another org
						scores[orgIndex] += rewardForMove;
						if (getGridValue(grid, currentLocation[orgIndex]) == EMPTY) {  // if the current location is empty...
							if (replacement == -1 || foodHereOnArrival[orgIndex] == EMPTY) {  // if replacement = random (-1) or the this location was already empty when org got here...
								setGridValue(grid, currentLocation[orgIndex], pickFood(-1));  // plant a random food
							} else if (replacement == 1 && foodHereOnArrival[orgIndex] > EMPTY) {  // if replacement = other (1) and there was some food here when org got here...
								setGridValue(grid, currentLocation[orgIndex], pickFood(foodHereOnArrival[orgIndex]));  // plant a different food when what was here
							}
							// if replacement = no replacement (0), no replacement/do nothing
						}
						setGridValue(orgPositionsGrid, currentLocation[orgIndex], 0);  // show location as not occupied.
						currentLocation[orgIndex] = moveOnGrid(currentLocation[orgIndex], facing[orgIndex]);
						setGridValue(orgPositionsGrid, currentLocation[orgIndex], 1);  // show new location as occupied.
						foodHereOnArrival[orgIndex] = getGridValue(grid, currentLocation[orgIndex]);  //value of the food when we got here - needed for replacement method.
					}
					break;
				}
			}

			if (debug) {
				cout << "outNodes: " << Bit(group->population[orgIndex]->brain->readOutput(0)) << " " << Bit(group->population[orgIndex]->brain->readOutput(1)) << " " << Bit(group->population[orgIndex]->brain->readOutput(2)) << "\n";
				cout << "output1: " << output1 << "  output2: " << output2 << "\n";
				cout << "\n  -- world update --\n\n";
				printGrid(grid, currentLocation[orgIndex], facing[orgIndex]);
				cout << "last: " << lastFood[orgIndex] << " here: " << getGridValue(grid, currentLocation[orgIndex]) << "\nloc: " << currentLocation[orgIndex].first << "," << currentLocation[orgIndex].second << "  facing: " << facing[orgIndex] << "\n";
				cout << "score: " << scores[orgIndex] << "\nfood1: " << eaten[orgIndex][1] << "  food2: " << eaten[orgIndex][2] << "  switches: " << switches[orgIndex] << "\n";
			}
		}  // end world evaluation loop
		if (visualize) {
			BerryWorld::SaveWorldState("worldUpdatesFile.txt", grid, currentLocation, facing);
		}
	}

	for (int i = 0; i < (int) group->population.size(); i++) {
		int total_eaten = 0;
		for (int f = 0; f <= foodTypes; f++) {
			if (f != 0) {  // don't count the attempts to eat empty!
				total_eaten += eaten[i][f];
			}
			string temp_name = "allfood" + to_string(f);  // make food names i.e. food1, food2, etc.
			group->population[i]->dataMap.Append(temp_name, eaten[i][f]);
		}
		if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
			(eaten[i][1] > eaten[i][2]) ? group->population[i]->dataMap.Append("allconsumptionRatio", (double) eaten[i][1] / (double) (eaten[i][2] + 1)) : group->population[i]->dataMap.Append("allconsumptionRatio", (double) eaten[i][2] / (double) (eaten[i][1] + 1));
		}
		group->population[i]->dataMap.Append("alltotal", total_eaten);  // total food eaten (regardless of type)

		group->population[i]->dataMap.Append("allswitches", switches[i]);

		if (scores[i] < 0.0) {
			scores[i] = 0.0;
		}
		group->population[i]->score = scores[i];
		group->population[i]->dataMap.Append("allscore", scores[i]);

	}
}

void BerryWorld::SaveWorldState(string fileName, vector<int> grid, vector<pair<int, int>> currentLocation, vector<int> facing) {
	string stateNow = "";

	int xCount = 0;

	for (auto loc : grid) {
		stateNow += to_string(loc) + ",";
		xCount++;
		if (xCount >= WorldX) {
			stateNow += "\n";
			xCount = 0;
		}
	}
	stateNow += "-\n";
	for (int i = 0; i < (int) currentLocation.size(); i++) {
		stateNow += to_string(currentLocation[i].first) + "\n";
		stateNow += to_string(currentLocation[i].second) + "\n";
		stateNow += to_string(facing[i]) + "\n";
	}
	stateNow += "-";
	FileManager::writeToFile(fileName, stateNow, to_string(WorldX) + ',' + to_string(WorldY));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
}
