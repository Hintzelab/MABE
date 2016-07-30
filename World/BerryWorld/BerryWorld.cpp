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
shared_ptr<ParameterLink<double>> BerryWorld::worldUpdatesBaisedOnInitialPL = Parameters::register_parameter("WORLD_BERRY-worldUpdatesBaisedOnInitial", 0.0, "if greater then 0 then worldUpdates will be this value * number of food in world at worldUpdate 0");

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

shared_ptr<ParameterLink<int>> BerryWorld::replacementPL = Parameters::register_parameter("WORLD_BERRY-replacement", -1, "-1 = random, 0 = no replacement, 1 = replace other (note: Food0, can be replace by Food0)");

shared_ptr<ParameterLink<bool>> BerryWorld::recordConsumptionRatioPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-recordConsumptionRatio", false, "if true, record greater of red/blue+1 or blue/red+1");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListPL = Parameters::register_parameter("WORLD_BERRY-recordFoodList", true, "if true, record list of food eaten");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListEatEmptyPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListEatEmpty", false, "if true, foodList will include attempts to eat 0");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListNoEatPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListNoEat", false, "if true, if true foodList will include no eat (-1)");

shared_ptr<ParameterLink<bool>> BerryWorld::alwaysStartOnFood1PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-alwaysStartOnFood1", false, "if true, if true organisms will all start on food1");

shared_ptr<ParameterLink<string>> BerryWorld::visualizationFileNamePL = Parameters::register_parameter("VISUALIZATION_MODE_WORLD_BERRY-visualizationFileName", (string) "worldUpdatesFile.txt", "in visualize mode, visualization data will be written to this file.");
shared_ptr<ParameterLink<string>> BerryWorld::mapFileListPL = Parameters::register_parameter("WORLD_BERRY-mapFileList", (string) "[]", "list of worlds in which to evaluate organism. If empty, random world will be created");
shared_ptr<ParameterLink<string>> BerryWorld::mapFileWhichMapsPL = Parameters::register_parameter("WORLD_BERRY-mapFileWhichMaps", (string) "[random]", "if mapFileList is not empty, method and list of maps from loaded file(s). Methods are random (pick one from list), all (test with every map in each evaluation). If only method is provided, then all maps will be used.");

shared_ptr<ParameterLink<bool>> BerryWorld::alwaysEatPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-alwaysEat", false, "if true, brain will only have 2 outputs (0 0 : no action, 1 0 : left, 0 1 : right, 1 1 : move). Organism will always eat if food is present.");
shared_ptr<ParameterLink<double>> BerryWorld::rewardSpatialNoveltyPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardSpatialNovelty", 0.0, "reward added to score every time organism moves on a new location");

shared_ptr<ParameterLink<bool>> BerryWorld::relativeScoringPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-relativeScoring", false, "score will be divided by the value of the positively scoring food on the initial map (useful when replacement = 0)");
shared_ptr<ParameterLink<int>> BerryWorld::boarderEdgePL = Parameters::register_parameter("WORLD_BERRY-boarderEdge", 0, "edge (may overlap wall) that will be left empty (food0)");

shared_ptr<ParameterLink<bool>> BerryWorld::senseVisitedPL = Parameters::register_parameter("WORLD_BERRY-senseVisited", false, "organism can sense if any organism has visited here");

shared_ptr<ParameterLink<string>> BerryWorld::fixedStartXRangePL = Parameters::register_parameter("WORLD_BERRY-fixedStartXRange", (string) "[-1]", "range for start location for organism (i.e. [x] for a fixed value, [x,y] to place in range), [-1] = random");
shared_ptr<ParameterLink<string>> BerryWorld::fixedStartYRangePL = Parameters::register_parameter("WORLD_BERRY-fixedStartYRange", (string) "[-1]", "range for start location for organism (i.e. [x] for a fixed value, [x,y] to place in range), [-1] = random");
shared_ptr<ParameterLink<int>> BerryWorld::fixedStartFacingPL = Parameters::register_parameter("WORLD_BERRY-fixedStartFacing", -1, "start facing direction (range 0-7) for organism, -1 = random");

// load a line from FILE. IF the line is empty or a comment (starts with #), skip line.
// if the line is not empty/comment, clean ss and load line.
// rawLine is the string version of the same data as ss
bool loadLineToSS(ifstream& FILE, string& rawLine, stringstream& ss) {
	rawLine.resize(0);
	if (FILE.is_open() && !FILE.eof()) {
		while ((rawLine.size() == 0 || rawLine[0] == '#') && !FILE.eof()) {
			getline(FILE, rawLine);
		}
		ss.clear();
		ss.str(string());
		ss << rawLine;
	} else if (!FILE.eof()) {
		cout << "in loadSS, FILE is not open!\n  Exiting." << endl;
		exit(1);
	}
	return FILE.eof();
}

bool BerryWorld::WorldMap::loadMap(ifstream& FILE) {
	sizeX = sizeY = startXMin = startXMax = startYMin = startYMax = startFacing = 0;
	grid.resize(0);
	string rawLine, rubbishString;
	char charBuffer;
	stringstream ss("");
	bool atEOF = false;
	bool done = false;

	sizeX = 0;
	sizeY = 1;
	if (FILE.is_open()) {
		atEOF = loadLineToSS(FILE, rawLine, ss);
		ss >> rubbishString >> name;
		atEOF = loadLineToSS(FILE, rawLine, ss);
		ss >> rubbishString >> startXMin >> charBuffer >> startXMax;
		atEOF = loadLineToSS(FILE, rawLine, ss);
		ss >> rubbishString >> startYMin >> charBuffer >> startYMax;
		atEOF = loadLineToSS(FILE, rawLine, ss);
		ss >> rubbishString >> startFacing;
		atEOF = loadLineToSS(FILE, rawLine, ss);
		ss >> charBuffer;
		while (!ss.fail()) { // load first line of map, keep loading chars until end of line (i.e. ss.fail because char could not be read)
			grid.push_back(charBuffer);
			sizeX++; // set X, this X will be used for the rest of the map.
			ss >> charBuffer;
		}
		while (!atEOF && !done) {
			atEOF = loadLineToSS(FILE, rawLine, ss);
			ss >> charBuffer;
			if (charBuffer == '-') { // if the first char on the line is "-" then we are at the end of the mapx
				done = true;
			} else {
				sizeY++; // not yet at end of map, add a row
				for (int c = 0; c < sizeX; c++) {
					grid.push_back(charBuffer);
					ss >> charBuffer;
				}
			}
		}
	}
	return done;
}

BerryWorld::BerryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_BERRY-worldUpdates");
	worldUpdatesBaisedOnInitial = (PT == nullptr) ? worldUpdatesBaisedOnInitialPL->lookup() : PT->lookupDouble("WORLD_BERRY-worldUpdatesBaisedOnInitial");


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

	visualizationFileName = (PT == nullptr) ? visualizationFileNamePL->lookup() : PT->lookupString("VISUALIZATION_MODE_WORLD_BERRY-visualizationFileName");

	convertCSVListToVector((PT == nullptr) ? mapFileListPL->lookup() : PT->lookupString("WORLD_BERRY-mapFileList"), mapFileList);
	convertCSVListToVector((PT == nullptr) ? mapFileWhichMapsPL->lookup() : PT->lookupString("WORLD_BERRY-mapFileWhichMaps"), mapFileWhichMaps);

	alwaysEat = (PT == nullptr) ? alwaysEatPL->lookup() : PT->lookupBool("WORLD_BERRY_ADVANCED-alwaysEat");
	rewardSpatialNovelty = (PT == nullptr) ? rewardSpatialNoveltyPL->lookup() : PT->lookupDouble("WORLD_BERRY_ADVANCED-rewardSpatialNovelty");

	relativeScoring = (PT == nullptr) ? relativeScoringPL->lookup() : PT->lookupBool("WORLD_BERRY_ADVANCED-relativeScoring");
	boarderEdge = (PT == nullptr) ? boarderEdgePL->lookup() : PT->lookupInt("WORLD_BERRY-boarderEdge");

	senseVisited = (PT == nullptr) ? senseVisitedPL->lookup() : PT->lookupBool("WORLD_BERRY-senseVisited");

	vector<int> rangeHolder;
	convertCSVListToVector((PT == nullptr) ? fixedStartXRangePL->lookup() : PT->lookupString("WORLD_BERRY-fixedStartXRange"), rangeHolder);
	if (rangeHolder.size() == 1) {
		fixedStartXMin = rangeHolder[0];
		fixedStartXMax = rangeHolder[0];
	} else {
		fixedStartXMin = rangeHolder[0];
		fixedStartXMax = rangeHolder[1];
	}

	convertCSVListToVector((PT == nullptr) ? fixedStartYRangePL->lookup() : PT->lookupString("WORLD_BERRY-fixedStartYRange"), rangeHolder);
	if (rangeHolder.size() == 1) {
		fixedStartYMin = rangeHolder[0];
		fixedStartYMax = rangeHolder[1];
	} else {
		fixedStartYMin = rangeHolder[0];
		fixedStartYMax = rangeHolder[1];
	}

	fixedStartFacing = (PT == nullptr) ? fixedStartFacingPL->lookup() : PT->lookupInt("WORLD_BERRY-fixedStartFacing");

	//LOAD MAPS FROM FILES
	if (mapFileList.size() > 0) {
		cout << "BerryWorld loading maps..." << endl;
		WorldMap newMap;

		for (auto fileName : mapFileList) {
			ifstream FILE(fileName);
			bool goodRead = false;
			while (!FILE.eof()) {
				goodRead = newMap.loadMap(FILE);
				if (goodRead) {
					cout << "    \"" << newMap.name << "\" from file \"" << fileName << "\"" << endl;
					if (worldMaps.find(newMap.name) == worldMaps.end()) {
						worldMaps.insert( { newMap.name, newMap });
					} else {
						cout << "  ...while reading world maps for berry world, found multiple maps with the same name. Duplicate name \"" << newMap.name << "\" found in file \"" << fileName << "\".\n  Exiting." << endl;
						exit(1);
					}
				}
			}
			//FILE.close();
		}
	}

	if (foodTypes < 1 || foodTypes > 8) {
		cout << "In BerryWorld you either have too few or too many foodTypes (must be >0 and <=8)\n\nExiting\n\n";
		exit(1);
	}
	if (alwaysEat) {
		outputNodesCount = 2;
		allowMoveAndEat = 1; // if always eat, then we must allow move and eat
	} else {
		outputNodesCount = 3;  // number of brain nodes used for output, 2 for move, 1 for eat
	}

	inputNodesCount = senseDown * (foodTypes + senseVisited) + ((senseFront * (foodTypes + senseVisited)) + senseWalls + senseOther) + (2 * (senseFrontSides * (foodTypes + senseVisited + senseWalls + senseOther)));

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

	aveFileColumns.push_back("novelty");
	aveFileColumns.push_back("repeated");

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

	int numWorlds = 1;
	if (mapFileWhichMaps.size() == 1 && mapFileWhichMaps[0] == "all") { // if method is all, but not worlds are listed, we must append all of the map names to mapFileWhichMaps.
		for (auto w : worldMaps) {
			mapFileWhichMaps.push_back(w.first);
		}
	}

	if (worldMaps.size() > 1) { // if maps have been loaded, check to see if mapFileWhichMaps method is all. If yes, then mapFileWhichMaps.size() - 1 is number of worlds
		if (mapFileWhichMaps.size() > 0 && mapFileWhichMaps[0] == "all") {
			numWorlds = mapFileWhichMaps.size() - 1;
		}
	}

	vector<double> summedScores(group->population.size(), 0);

	for (int worldCount = 0; worldCount < numWorlds; worldCount++) {

		vector<double> scores(group->population.size(), 0);
		int MAXSCORE = 1; // scores will be divided by MAXSCORE. If relativeScoring is true MAXSCORE will be set (see relativeScoring/MAXSCORE below)
		int FOODCOUNT = 0; // used if modulateWorldTime is set

		vector<int> novelty(group->population.size(), 0);
		vector<int> repeated(group->population.size(), 0);
		//vector<int> visitedGrid = makeGrid(WorldX, WorldY);

		vector<int> grid;

		if (worldMaps.size() == 0) {
			grid = makeTestGrid();
		} else {
			WorldMap thisMap;
			if (mapFileWhichMaps.size() == 0 || (mapFileWhichMaps[0] != "random" && mapFileWhichMaps[0] != "all")) {
				cout << "  In BerryWorld: WORLD_BERRY-mapFileList was provided, but WORLD_BERRY-mapFileWhichMaps does not provide a method.\n  The first element in WORLD_BERRY-mapFileWhichMaps must provide a method (random or all).\n    Exiting." << endl;
			} else if (mapFileWhichMaps.size() == 1 && mapFileWhichMaps[0] == "random") { // randomly select from all loaded maps
				auto item = worldMaps.begin();
				advance(item, Random::getIndex(worldMaps.size()));
				thisMap = (*item).second;
			} else if (mapFileWhichMaps.size() > 1 && mapFileWhichMaps[0] == "random") { // randomly select from list of loaded maps
				string mapKey = mapFileWhichMaps[Random::getInt(1, mapFileWhichMaps.size() - 1)];
				thisMap = worldMaps[mapKey];
			} else if (mapFileWhichMaps.size() > 1 && mapFileWhichMaps[0] == "all") { // select map baised on loop control variable
				string mapKey = mapFileWhichMaps[worldCount + 1];
				thisMap = worldMaps[mapKey];
			} else {
				cout << "  In BerryWorld: WORLD_BERRY-mapFileList was provided, but WORLD_BERRY-mapFileWhichMaps does not provide a method.\n  The first element in WORLD_BERRY-mapFileWhichMaps must provide a method (random or all).\n    Exiting." << endl;
			}

			// convert worldMap to world
			WorldX = thisMap.sizeX;
			WorldY = thisMap.sizeY;
			fixedStartXMin = thisMap.startXMin;
			fixedStartXMax = thisMap.startXMax;
			fixedStartYMin = thisMap.startYMin;
			fixedStartYMax = thisMap.startYMax;
			fixedStartFacing = thisMap.startFacing;
			grid.clear();
			for (auto c : thisMap.grid) {
				if (isdigit(c)) {
					grid.push_back((int) c - (int) ('0'));
				} else {
					if (c == '?') { // put a random food here
						grid.push_back(pickFood(-1));
					}
				}
			}
		}

		if (relativeScoring) {
			MAXSCORE = 0;
			for (auto v : grid) {
				if (v > 0 && v != WALL && foodRewards[v] > 0) {
					MAXSCORE += foodRewards[v];
					FOODCOUNT++;
				}
			}
		}

		vector<int> orgPositionsGrid = makeGrid(WorldX, WorldY);

		vector<int> visitedGrid = makeGrid(WorldX, WorldY);

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
			if (alwaysStartOnFood1 && (fixedStartXMin != -1 || fixedStartYMin != -1)) {
				cout << "  A problem has been encounter in BerryWorld. a fixedStart value (range) has been set at the same time as alwaysStartOnFood1... please pick one or the other./n  Exiting.";
				exit(1);
			}
			if (alwaysStartOnFood1) {
				while ((getGridValue(orgPositionsGrid, newLocation) == 1 || getGridValue(grid, newLocation) == WALL) || getGridValue(grid, newLocation) != 1) {
					newLocation = {Random::getIndex(WorldX), Random::getIndex(WorldY)};
					c++;
					if (c % 1000000 == 0) {
						cout << "Warning! One million attempts were made to place an organism, your world may be too small!" << endl;
					}
				}
			} else {
				if (fixedStartXMin != -1 || fixedStartYMin != -1) {
					if (fixedStartXMin != -1 && fixedStartYMin == -1) {
						newLocation = {Random::getInt(fixedStartXMin,fixedStartXMax),Random::getInt((int)borderWalls, WorldY - (int)borderWalls - 1)};
					} else if(fixedStartXMin == -1 && fixedStartYMin != -1) {
						newLocation = {Random::getInt((int)borderWalls, WorldX - (int)borderWalls - 1), Random::getInt(fixedStartYMin,fixedStartYMax)};
					}
					else {
						newLocation = {Random::getInt(fixedStartXMin,fixedStartXMax),Random::getInt(fixedStartYMin,fixedStartYMax)};
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
			}
			currentLocation.push_back(newLocation);  // location of the organism
			if (fixedStartFacing == -1) {
				facing.push_back(Random::getIndex(8));  // direction the agent is facing
			} else {
				facing.push_back(fixedStartFacing);
			}
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
		int otherLeftFront, otherFront, otherRightFront;  // used to store info about other organisms for sense other
		int visitedHere, visitedLeftFront, visitedFront, visitedRightFront;  // used to store info from vistedGrid

		int nodesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs

		vector<int> stateCollector;  // used in the phi calculation

		// make sure the brain does not have values from last run

		vector<int> orgList;
		int orgListIndex;
		int orgIndex;

		if (visualize) {  // save state of world before we get started.
			BerryWorld::SaveWorldState(visualizationFileName, grid, visitedGrid, currentLocation, facing, true);
		}

		int realWorldUpdates = (worldUpdatesBaisedOnInitial <= 0)?worldUpdates:MAXSCORE*worldUpdatesBaisedOnInitial;
		for (int t = 0; t < realWorldUpdates; t++) {  //run agent for "worldUpdates" brain updates
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
				if (senseVisited) {
					visitedHere = getGridValue(visitedGrid, currentLocation[orgIndex]);
					visitedFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex]));
					visitedLeftFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], turnLeft(facing[orgIndex])));
					visitedRightFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], turnRight(facing[orgIndex])));
					if (senseDown) {
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, visitedHere);
					}
					if (senseFront) {
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, visitedFront);
					}
					if (senseFrontSides) {
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, visitedLeftFront);
						group->population[orgIndex]->brain->setInput(nodesAssignmentCounter++, visitedRightFront);

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
				if (alwaysEat) {
					output2 = 1;
				} else {
					output2 = Bit(group->population[orgIndex]->brain->readOutput(2));
				}

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
								//cout << replacement << endl;
								// replacement rules
								// -1 random
								// 0 no replacment
								// 1 other (0 can be replaced by other)
								if (replacement == -1 || (replacement == 1 && foodHereOnArrival[orgIndex] == EMPTY)) {  // if replacement = random (-1).or replacment other (1) and was empty..
									setGridValue(grid, currentLocation[orgIndex], pickFood(-1));  // plant a random food
									//cout << "replacement = -1 (random) .. ";
								} else if (replacement == 1 && foodHereOnArrival[orgIndex] > EMPTY) {  // if replacement = other (1) and there was some food here when org got here...
									setGridValue(grid, currentLocation[orgIndex], pickFood(foodHereOnArrival[orgIndex]));  // plant a different food when what was here
									//cout << "replacement = 1 (other) and EMPTY.. ";
								} else { // replacement = 0, no replacement
									//cout << "no replace .. ";
								}
								//cout << "move done." << endl;
								// if replacement = no replacement (0), no replacement/do nothing
							}
							setGridValue(orgPositionsGrid, currentLocation[orgIndex], 0);  // show location as not occupied.
							setGridValue(visitedGrid, currentLocation[orgIndex], 1); // leave a marker in the visitedGrid
							currentLocation[orgIndex] = moveOnGrid(currentLocation[orgIndex], facing[orgIndex]); // move organism
							setGridValue(orgPositionsGrid, currentLocation[orgIndex], 1);  // show new location as occupied.
							if (getGridValue(visitedGrid, currentLocation[orgIndex]) == 0) { // if this is a novel location
								novelty[orgIndex]++;
								scores[orgIndex] += rewardSpatialNovelty;
							} else { // if anyone has been here before
								repeated[orgIndex]++;
							}
							foodHereOnArrival[orgIndex] = getGridValue(grid, currentLocation[orgIndex]);  //value of the food when we got here - needed for replacement method.
						}
						break;
					}
				}

				if (debug) {
					for (int i = 0; i < outputNodesCount; i++) {
						cout << Bit(group->population[orgIndex]->brain->readOutput(i)) << " ";
					}
					cout << "output1: " << output1 << "  output2: " << output2 << "\n";
					cout << "\n  -- world update --\n\n";
					printGrid(grid, currentLocation[orgIndex], facing[orgIndex]);
					cout << "last eaten: " << lastFood[orgIndex] << " here: " << getGridValue(grid, currentLocation[orgIndex]) << "\nloc: " << currentLocation[orgIndex].first << "," << currentLocation[orgIndex].second << "  facing: " << facing[orgIndex] << "\n";
					cout << "score: " << scores[orgIndex] << " switches: " << switches[orgIndex] << "\n";
				}
			}  // end world evaluation loop
			if (visualize) {
				BerryWorld::SaveWorldState(visualizationFileName, grid, visitedGrid, currentLocation, facing);
			}
		}

		for (int i = 0; i < (int) group->population.size(); i++) {
			int total_eaten = 0;
			for (int f = 0; f <= foodTypes; f++) {
				if (f != 0) {  // don't count the attempts to eat empty!'
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
			group->population[i]->dataMap.Append("allnovelty", novelty[i]);
			group->population[i]->dataMap.Append("allrepeated", repeated[i]);

			if (scores[i] < 0.0) {
				scores[i] = 0.0;
			}

			summedScores[i] += scores[i] / MAXSCORE;
		}
	}
	for (int i = 0; i < (int) group->population.size(); i++) {

		group->population[i]->score = summedScores[i] / numWorlds;
		group->population[i]->dataMap.Append("allscore", summedScores[i] / numWorlds);
	}
}

void BerryWorld::SaveWorldState(string fileName, vector<int> grid, vector<int> vistedGrid, vector<pair<int, int>> currentLocation, vector<int> facing, bool reset) {
	string stateNow = "";

	if (reset){
		stateNow += "**\n";
	}
	int xCount = 0;
	int vistedGridCount = 0;

	for (auto loc : grid) {
		stateNow += to_string(loc) + ",";
		xCount++;
		vistedGridCount++;
		if (xCount >= WorldX) {
			stateNow += "\n";
			xCount = 0;
		}
	}
	stateNow += "-\n";
	for (auto loc : vistedGrid) {
		stateNow += to_string(loc) + ",";
		xCount++;
		vistedGridCount++;
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
	FileManager::writeToFile(fileName, stateNow, "8," + to_string(WorldX) + ',' + to_string(WorldY));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
}
