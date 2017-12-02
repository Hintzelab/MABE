//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "BerryWorld.h"

shared_ptr<ParameterLink<double>> BerryWorld::TSKPL = Parameters::register_parameter("WORLD_BERRY-taskSwitchingCost", 1.4, "cost to change food sources");
shared_ptr<ParameterLink<int>> BerryWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_BERRY-worldUpdates", 400, "amount of time a brain is tested");
shared_ptr<ParameterLink<double>> BerryWorld::worldUpdatesBaisedOnInitialPL = Parameters::register_parameter("WORLD_BERRY-worldUpdatesBaisedOnInitial", 0.0, "if greater then 0 then worldUpdates will be this value * number of food in world at worldUpdate 0");

shared_ptr<ParameterLink<int>> BerryWorld::foodTypesPL = Parameters::register_parameter("WORLD_BERRY-foodTypes", 2, "number of types of food");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood0PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood0", 0.0, "reward for eating a Food0");
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

shared_ptr<ParameterLink<int>> BerryWorld::replacementDefaultRulePL = Parameters::register_parameter("WORLD_BERRY-replacementDefaultRule", -1, "-1 = random, 0 = no replacement, 1 = replace other (note: Food0, can be replace by Food0)");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood0PL = Parameters::register_parameter("WORLD_BERRY-replacementFood0", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood1PL = Parameters::register_parameter("WORLD_BERRY-replacementFood1", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood2PL = Parameters::register_parameter("WORLD_BERRY-replacementFood2", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood3PL = Parameters::register_parameter("WORLD_BERRY-replacementFood3", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood4PL = Parameters::register_parameter("WORLD_BERRY-replacementFood4", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood5PL = Parameters::register_parameter("WORLD_BERRY-replacementFood5", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood6PL = Parameters::register_parameter("WORLD_BERRY-replacementFood6", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood7PL = Parameters::register_parameter("WORLD_BERRY-replacementFood7", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");
shared_ptr<ParameterLink<int>> BerryWorld::replacementFood8PL = Parameters::register_parameter("WORLD_BERRY-replacementFood8", -1, "when this type of food is replaced, replace with value, -1 = use 'replacement' value");

shared_ptr<ParameterLink<bool>> BerryWorld::recordConsumptionRatioPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-recordConsumptionRatio", false, "if true, record greater of red/blue+1 or blue/red+1");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListPL = Parameters::register_parameter("WORLD_BERRY-recordFoodList", false, "if true, record list of food eaten");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListEatEmptyPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListEatEmpty", false, "if true, foodList will include attempts to eat 0");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListNoEatPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListNoEat", false, "if true, if true foodList will include no eat (-1)");

shared_ptr<ParameterLink<int>> BerryWorld::alwaysStartOnFoodPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-alwaysStartOnFoodOfType", -1, "if -1 organisms are placed randomly. if > -1, all organisms will start on this type of food (must be < 9)");

shared_ptr<ParameterLink<bool>> BerryWorld::saveOrgActionsPL = Parameters::register_parameter("VISUALIZATION_MODE_WORLD_BERRY-saveOrgActions", false, "in visualize mode, save organisms actions to file with name [Org->ID]_actions.txt");
shared_ptr<ParameterLink<string>> BerryWorld::visualizationFileNamePL = Parameters::register_parameter("VISUALIZATION_MODE_WORLD_BERRY-visualizationFileName", (string) "worldUpdatesFile.txt", "in visualize mode, visualization data will be written to this file.");

shared_ptr<ParameterLink<string>> BerryWorld::mapFileListPL = Parameters::register_parameter("WORLD_BERRY-mapFileList", (string) "[]", "list of worlds in which to evaluate organism. If empty, random world will be created");
shared_ptr<ParameterLink<string>> BerryWorld::mapFileWhichMapsPL = Parameters::register_parameter("WORLD_BERRY-mapFileWhichMaps", (string) "[random]", "if mapFileList is not empty, this parameter will determine which maps are seen by an organism in one evaluation.\n[random] select one random map\n[all] select all maps (from all files)\nif two values are present the first determines which files to pull maps from, the second which maps from those files\nthe options for the first position (file) are:\n  'all' (pull from all files)\n  '#' (pull from # random files - with possible repeats)\n  'u#' (pull from # unique files)\nthe options for the second position (map) are:\n  'all' (all maps in the file)\n  '#' (# random maps from file - with possible repeats)\n  'u#' (# unique maps from file)\nexample1: [all,u2] = from all files, 2 unique maps\nexample2: [2,1] one map from each two files (might be the same file twice)");

shared_ptr<ParameterLink<bool>> BerryWorld::alwaysEatPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-alwaysEat", false, "if true, brain will only have 2 outputs (0 0 : no action, 1 0 : left, 0 1 : right, 1 1 : move). Organism will always eat if food is present.");
shared_ptr<ParameterLink<double>> BerryWorld::rewardSpatialNoveltyPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardSpatialNovelty", 0.0, "reward added to score every time organism moves on a new location");

shared_ptr<ParameterLink<bool>> BerryWorld::relativeScoringPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-relativeScoring", false, "score will be divided by the value of the positively scoring food on the initial map (useful when replacement = 0)");
shared_ptr<ParameterLink<int>> BerryWorld::boarderEdgePL = Parameters::register_parameter("WORLD_BERRY-boarderEdge", 0, "edge (may overlap wall) that will be left empty (food0)");

shared_ptr<ParameterLink<bool>> BerryWorld::senseVisitedPL = Parameters::register_parameter("WORLD_BERRY-senseVisited", false, "organism can sense if any organism has visited here");

shared_ptr<ParameterLink<string>> BerryWorld::fixedStartXRangePL = Parameters::register_parameter("WORLD_BERRY-fixedStartXRange", (string) "[-1]", "range for start location for organism (i.e. [x] for a fixed value, [x,y] to place in range), [-1] = random");
shared_ptr<ParameterLink<string>> BerryWorld::fixedStartYRangePL = Parameters::register_parameter("WORLD_BERRY-fixedStartYRange", (string) "[-1]", "range for start location for organism (i.e. [x] for a fixed value, [x,y] to place in range), [-1] = random");
shared_ptr<ParameterLink<int>> BerryWorld::fixedStartFacingPL = Parameters::register_parameter("WORLD_BERRY-fixedStartFacing", -1, "start facing direction (range 0-7) for organism, -1 = random");


shared_ptr<ParameterLink<int>> BerryWorld::repeatsPL = Parameters::register_parameter("WORLD_BERRY-repeats", 3, "Number of times to test each Organism per generation");
shared_ptr<ParameterLink<bool>> BerryWorld::groupEvaluationPL = Parameters::register_parameter("WORLD_BERRY-groupEvaluation", false, "if true, evaluate population concurrently");


shared_ptr<ParameterLink<string>> BerryWorld::groupNamePL = Parameters::register_parameter("WORLD_BERRY_NAMES-groupNameSpace", (string)"root::", "namespace for group to be evaluated");
shared_ptr<ParameterLink<string>> BerryWorld::brainNamePL = Parameters::register_parameter("WORLD_BERRY_NAMES-brainNameSpace", (string)"root::", "namespace for parameters used to define brain");


// load a line from FILE. IF the line is empty or a comment (starts with #), skip line.
// if the line is not empty/comment, clean ss and load line.
// rawLine is the string version of the same data as ss

bool BerryWorld::WorldMap::loadMap(ifstream& FILE, const string _fileName, shared_ptr<ParametersTable> parentPT) {
	fileName = _fileName;
	string parameterName, parameterValue;
	grid.resize(0);
	string rawLine, rubbishString;
	char charBuffer;
	stringstream ss("");
	bool atEOF = false;
	bool done = false;

	int sizeX = 0;
	int sizeY = 1;

	if (FILE.is_open()) {
		atEOF = loadLineToSS(FILE, rawLine, ss);
		mapName = "";
		ss >> rubbishString >> rubbishString >> mapName;

		// now that we have the name, we can make the PT. The namespace will be based on the parents (the world) name space.
		// PT = make_shared<ParametersTable>(parentPT->getTableNameSpace()+"::"+fileName+"__"+name,Parameters::root);
		if (mapName != "") {
			PT = Parameters::root->makeTable(parentPT->getTableNameSpace() + fileName.substr(0, fileName.size() - 4) + "__" + mapName + "::", Parameters::root);
			atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
			while (rawLine[0] == '-' && rawLine[1] == 'p' && !atEOF) {
				ss >> rubbishString >> parameterName >> rubbishString >> parameterValue; // "-p">>name>>
				PT->setExistingParameter(parameterName, parameterValue);

				atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
			}

			ss >> charBuffer;

			while (!ss.fail()) { // load first line of map, keep loading chars until end of line (i.e. ss.fail because char could not be read)
				grid.push_back(charBuffer);
				sizeX++; // set X, this X will be used for the rest of the map.
				ss >> charBuffer;
			}
			while (!atEOF && !done) {
				atEOF = loadLineToSS(FILE, rawLine, ss);
				ss >> charBuffer;
				if (charBuffer == '*') { // if the first char on the line is "-" then we are at the end of the mapx
					done = true;
				} else {
					sizeY++; // not yet at end of map, add a row
					for (int c = 0; c < sizeX; c++) {
						grid.push_back(charBuffer);
						ss >> charBuffer;
					}
				}
			}
			PT->setParameter("WORLD_BERRY-worldX", sizeX);
			PT->setParameter("WORLD_BERRY-worldY", sizeY);
		}
	}
	return done;
}

BerryWorld::BerryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	worldUpdates =worldUpdatesPL->get(PT);
	worldUpdatesBaisedOnInitial =worldUpdatesBaisedOnInitialPL->get(PT);

	foodTypes =foodTypesPL->get(PT);

	TSK =TSKPL->get(PT);

	rewardForTurn =rewardForTurnPL->get(PT);
	rewardForMove =rewardForMovePL->get(PT);

	WorldY =WorldYPL->get(PT);
	WorldX =WorldXPL->get(PT);
	borderWalls =borderWallsPL->get(PT);
	randomWalls =randomWallsPL->get(PT);

	allowMoveAndEat =allowMoveAndEatPL->get(PT);

	senseDown =senseDownPL->get(PT);
	senseFront =senseFrontPL->get(PT);
	senseFrontSides =senseFrontSidesPL->get(PT);
	senseWalls =senseWallsPL->get(PT);

	senseOther =senseOtherPL->get(PT);

	clearOutputs =clearOutputsPL->get(PT);

	replacementDefaultRule =replacementDefaultRulePL->get(PT);
	replacementRules.resize(9);
	replacementRules[0] =replacementFood0PL->get(PT);
	replacementRules[1] =replacementFood1PL->get(PT);
	replacementRules[2] =replacementFood2PL->get(PT);
	replacementRules[3] =replacementFood3PL->get(PT);
	replacementRules[4] =replacementFood4PL->get(PT);
	replacementRules[5] =replacementFood5PL->get(PT);
	replacementRules[6] =replacementFood6PL->get(PT);
	replacementRules[7] =replacementFood7PL->get(PT);
	replacementRules[8] =replacementFood8PL->get(PT);

	recordConsumptionRatio =recordConsumptionRatioPL->get(PT);
	recordFoodList =recordFoodListPL->get(PT);
	recordFoodListEatEmpty =recordFoodListEatEmptyPL->get(PT);
	recordFoodListNoEat =recordFoodListNoEatPL->get(PT);

	alwaysStartOnFood =alwaysStartOnFoodPL->get(PT);

	saveOrgActions = saveOrgActionsPL->get(PT);
	saveOrgActions = saveOrgActions && Global::modePL->get() == "visualize";
	visualizationFileName =visualizationFileNamePL->get(PT);

	convertCSVListToVector(mapFileListPL->get(PT), mapFileList);
	convertCSVListToVector(mapFileWhichMapsPL->get(PT), mapFileWhichMaps);

	alwaysEat =alwaysEatPL->get(PT);
	rewardSpatialNovelty =rewardSpatialNoveltyPL->get(PT);

	relativeScoring =relativeScoringPL->get(PT);
	boarderEdge =boarderEdgePL->get(PT);

	senseVisited =senseVisitedPL->get(PT);

	vector<int> rangeHolder;
	convertCSVListToVector(fixedStartXRangePL->get(PT), rangeHolder);
	if (rangeHolder.size() == 1) {
		fixedStartXMin = rangeHolder[0];
		fixedStartXMax = rangeHolder[0];
	} else if (rangeHolder.size() == 2) {
		fixedStartXMin = rangeHolder[0];
		fixedStartXMax = rangeHolder[1];
	} else {
		cout << "  Bad Setting! WORLD_BERRY-fixedStartXRange is set to an invalid value : \"" << (fixedStartXRangePL->get(PT)) << "\"\n  Exiting." << endl;
		exit(1);
	}

	convertCSVListToVector(fixedStartYRangePL->get(PT), rangeHolder);
	if (rangeHolder.size() == 1) {
		fixedStartYMin = rangeHolder[0];
		fixedStartYMax = rangeHolder[0];
	} else if (rangeHolder.size() == 2) {
		fixedStartYMin = rangeHolder[0];
		fixedStartYMax = rangeHolder[1];
	} else {
		cout << "  Bad Setting! WORLD_BERRY-fixedStartYRange is set to an invalid value : \"" << (fixedStartYRangePL->get(PT)) << "\"\n  Exiting." << endl;
		exit(1);
	}

	fixedStartFacing =fixedStartFacingPL->get(PT);


	repeats =repeatsPL->get(PT);
	groupEvaluation =groupEvaluationPL->get(PT);

	string groupName =groupNamePL->get(PT);
	brainName =brainNamePL->get(PT);

	/////////////////////////////////////////////////////////////////////////////////////
	//  LOAD MAPS FROM FILES  ///////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////

	if (mapFileList.size() > 0) {
		cout << "BerryWorld loading maps..." << endl;

		for (auto fileName : mapFileList) {
			worldMaps[fileName] = map<string, WorldMap>();
			ifstream FILE(fileName);
			bool goodRead = false;
			while (!FILE.eof()) {
				WorldMap newMap;
				goodRead = newMap.loadMap(FILE, fileName, PT);
				if (goodRead) {
					cout << "    \"" << newMap.mapName << "\" from file \"" << fileName << "\"" << endl;
					if (worldMaps[fileName].find(newMap.mapName) == worldMaps[fileName].end()) {
						worldMaps[fileName].insert( { newMap.mapName, newMap });
					} else {
						cout << "  ...while reading world maps for berry world, found multiple maps with the same name. Duplicate name \"" << newMap.fileName + "__" + newMap.mapName << "\" found in file \"" << fileName << "\".\n  Exiting." << endl;
						exit(1);
					}
				}
			}
			FILE.close();
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
	cout << "    groupName: " << groupNamePL->get(PT) << "   brainName: " << brainNamePL->get(PT) << endl;



	foodRatioLookup.resize(9);  // stores reward of each type of food NOTE: food is indexed from 1 so 0th entry is chance to leave empty
	foodRatioLookup[0] =ratioFood0PL->get(PT);
	foodRatioLookup[1] =ratioFood1PL->get(PT);
	foodRatioLookup[2] =ratioFood2PL->get(PT);
	foodRatioLookup[3] =ratioFood3PL->get(PT);
	foodRatioLookup[4] =ratioFood4PL->get(PT);
	foodRatioLookup[5] =ratioFood5PL->get(PT);
	foodRatioLookup[6] =ratioFood6PL->get(PT);
	foodRatioLookup[7] =ratioFood7PL->get(PT);
	foodRatioLookup[8] =ratioFood8PL->get(PT);

	foodRatioTotal = 0;
	for (int i = 0; i <= foodTypes; i++) {
		foodRatioTotal += foodRatioLookup[i];
	}

	foodRewards.resize(9);  // stores reward of each type of food
	foodRewards[0] =rewardForFood0PL->get(PT);
	foodRewards[1] =rewardForFood1PL->get(PT);
	foodRewards[2] =rewardForFood2PL->get(PT);
	foodRewards[3] =rewardForFood3PL->get(PT);
	foodRewards[4] =rewardForFood4PL->get(PT);
	foodRewards[5] =rewardForFood5PL->get(PT);
	foodRewards[6] =rewardForFood6PL->get(PT);
	foodRewards[7] =rewardForFood7PL->get(PT);
	foodRewards[8] =rewardForFood8PL->get(PT);

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("total");

	popFileColumns.push_back("novelty");
	popFileColumns.push_back("repeated");

	if (foodTypes > 1) {
		popFileColumns.push_back("switches");
	}
	for (int i = 0; i <= foodTypes; i++) {
		string temp_name = "food" + to_string(i);  // make food names i.e. food1, food2, etc.
		popFileColumns.push_back(temp_name);
	}
	if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
		popFileColumns.push_back("consumptionRatio");
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

void BerryWorld::runWorld(shared_ptr<Group> group, int analyse, int visualize, int debug) {

	int numWorlds = 1;
	int howManyFiles;
	int howManyMaps;

	vector<double> summedScores(group->population.size(), 0);

	DataMap dataMap;

	vector<pair<string, string>> worldList; // make a list of worlds to test this (possibly population) organism in. If empty, a random world is generated.
	vector<string> fileList;

	// make list of fileName,mapName pairs that this org (population) will visit

	if (mapFileList.size() != 0) {
		if (mapFileWhichMaps.size() == 1 && mapFileWhichMaps[0] == "all") { // if method is all, append all of the map names to worldList.
			for (auto file : worldMaps) { // for every map in worldMaps (i.e. every file)
				for (auto map : file.second) { // for every map in that file
					worldList.push_back( { file.first, map.first }); // add file name and map name to worldList
				}
			}
		}

		if (mapFileWhichMaps.size() == 1 && mapFileWhichMaps[0] == "random") { // if method is all, append all of the map names to worldList.
			auto item1 = worldMaps.begin();
			advance(item1, Random::getIndex((int) worldMaps.size()));
			string filePick = item1->first;

			auto item2 = worldMaps[filePick].begin();
			advance(item2, Random::getIndex((int) worldMaps[filePick].size()));

			worldList.push_back( { filePick, item2->first }); // add file name and map name to worldList
		}

		// if mapFileWhichMaps has 2 elements, then first, determine the file list
		//     if [all,*], add all file names to fileList
		//     if [u#,*], add # unique file names to fileList
		//     if [#,*], add # file names to fileList (with repeats)
		// once the file list has been determined, figure out which maps to use from these files
		// a list of pairs <fileName,mapName>, worldList, will be created, this will be used later to select maps.
		//     if [*,all], use all maps from the indicated files
		//     if [*,u#], use # unique maps from each file... it is possible that the same map will be repeated if a file name appears twice in fileList
		//     if [*,#], use # unique file names to fileList

		if (mapFileWhichMaps.size() == 2) {
			if (mapFileWhichMaps[0] == "all") { // pull from all files, add all file names to fileList
				for (auto file : worldMaps) {
					fileList.push_back(file.first);
				}
			} else if (mapFileWhichMaps[0][0] == 'u') { //pull some number of unique files
				load_value(mapFileWhichMaps[0].substr(1, mapFileWhichMaps[0].size() - 1), howManyFiles);
				if (howManyFiles > (int) worldMaps.size()) {
					cout << "  in BerryWorld, selecting worlds from file... " << howManyFiles << " unique files are being requested, but only " << worldMaps.size() << " files have been loaded!\n  Exiting." << endl;
					exit(1);
				} else { // select howManyFiles unique files
					set<int> picks;
					while ((int) picks.size() < howManyFiles) {
						picks.insert(Random::getIndex((int) worldMaps.size())); // keep adding numbers to picks till there are howManyMaps number in picks
					}
					for (auto pick : picks) { // add each pick to worldList
						auto item = worldMaps.begin();
						advance(item, pick);
						fileList.push_back(item->first);
					}
				}
			} else { // pull some number of files, with repeats
				load_value(mapFileWhichMaps[0], howManyFiles);
				for (int i = 0; i < howManyFiles; i++) {
					auto item = worldMaps.begin();
					advance(item, Random::getIndex((int) worldMaps.size()));
					fileList.push_back(item->first);
				}
			} // at this point we should have some file names.
			for (auto fileName : fileList) { // for each file name, select maps.
				auto file = worldMaps[fileName];
				if (mapFileWhichMaps[1] == "all") { // fyi, [all,all]  add all maps from all of the files (same as just setting [all])
					for (auto map : file) {
						worldList.push_back( { fileName, map.first }); // add file name and map name to worldList
					}
				} else { // not all maps
					if (mapFileWhichMaps[1][0] == 'u') { //pull some number of unique Maps from each file
						load_value(mapFileWhichMaps[1].substr(1, mapFileWhichMaps[1].size() - 1), howManyMaps);
						if (howManyMaps > (int) file.size()) {
							cout << "  in BerryWorld, selecting worlds from file... " << howManyMaps << " unique maps are being requested, but file \"" << fileName << "\" only has " << file.size() << " maps!\n  Exiting." << endl;
							exit(1);
						} else { // select howManyMaps unique maps from file
							set<int> picks;
							while ((int) picks.size() < howManyMaps) {
								picks.insert(Random::getIndex((int) file.size())); // keep adding numbers to picks till there are howManyMaps number in picks
							}
							for (auto pick : picks) { // add each pick to worldList
								auto item = file.begin();
								advance(item, pick);
								worldList.push_back( { fileName, item->first });
							}
							//newMapName
						}
					} else { // select some number of random maps (with repeats)
						load_value(mapFileWhichMaps[1], howManyMaps);
						for (int i = 0; i < howManyMaps; i++) {
							auto item = file.begin();
							advance(item, Random::getIndex((int) file.size()));
							worldList.push_back( { fileName, item->first });
						}
					}
				}
			}
		}
	}

	bool lastActionWasMove = false;
	numWorlds = max(numWorlds, (int) worldList.size());
	//cout << "numWorlds: " << numWorlds;
	for (int worldCount = 0; worldCount < numWorlds; worldCount++) {

		vector<double> scores(group->population.size(), 0);
		double MAXSCORE = 1; // scores will be divided by MAXSCORE. If relativeScoring is true MAXSCORE will be set (see relativeScoring/MAXSCORE below)
		int FOODCOUNT = 0; // used if modulateWorldTime is set

		vector<int> novelty(group->population.size(), 0);
		vector<int> repeated(group->population.size(), 0);
		//vector<int> visitedGrid = makeGrid(WorldX, WorldY);

		vector<int> grid;

		if (worldList.size() == 0) {
			grid = makeTestGrid();
		} else {

			WorldMap thisMap = worldMaps[worldList[worldCount].first][worldList[worldCount].second];

			// convert worldMap to world
			WorldX = WorldXPL->get(PT);
			WorldY = WorldYPL->get(PT);

			vector<int> rangeHolder;
			convertCSVListToVector(fixedStartXRangePL->get(PT), rangeHolder);
			if (rangeHolder.size() == 1) {
				fixedStartXMin = rangeHolder[0];
				fixedStartXMax = rangeHolder[0];
			} else if (rangeHolder.size() == 2) {
				fixedStartXMin = rangeHolder[0];
				fixedStartXMax = rangeHolder[1];
			} else {
				cout << "  Bad Setting! WORLD_BERRY-fixedStartXRange is set to an invalid value : \"" << (fixedStartXRangePL->get(PT)) << "\"\n  Exiting." << endl;
				exit(1);
			}

			convertCSVListToVector(fixedStartYRangePL->get(PT), rangeHolder);
			if (rangeHolder.size() == 1) {
				fixedStartYMin = rangeHolder[0];
				fixedStartYMax = rangeHolder[0];
			} else if (rangeHolder.size() == 2) {
				fixedStartYMin = rangeHolder[0];
				fixedStartYMax = rangeHolder[1];
			} else {
				cout << "  Bad Setting! WORLD_BERRY-fixedStartYRange is set to an invalid value : \"" << (fixedStartYRangePL->get(PT)) << "\"\n  Exiting." << endl;
				exit(1);
			}

			fixedStartFacing = fixedStartFacingPL->get(PT);
			worldUpdates = worldUpdatesPL->get(PT);

			grid.clear();
			for (auto c : thisMap.grid) {
				if (isdigit(c)) {
					grid.push_back((int) c - (int) ('0')); // if it's a number, put that number here
				} else {
					if (c == '?') { // if "?" put a random food here
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
			if (alwaysStartOnFood != -1 && (fixedStartXMin != -1 || fixedStartYMin != -1)) {
				cout << "  A problem has been encounter in BerryWorld. a fixedStart value (range) has been set at the same time as alwaysStartOnFood... please pick one or the other./n  Exiting.";
				exit(1);
			}
			if (alwaysStartOnFood > -1) {
				while ((getGridValue(orgPositionsGrid, newLocation) == 1 || getGridValue(grid, newLocation) == WALL) || getGridValue(grid, newLocation) != alwaysStartOnFood) {
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

		// set up to track what food is eaten
		vector<int> switches(group->population.size(), 0);	// number of times organism has switched food source
		vector<int> lastFood(group->population.size(), -1);	//nothing has been eaten yet!
		vector<int> foodHereOnArrival(group->population.size(), 0);
		vector<vector<int>> eaten;	// stores number of each type of food was eaten in total for this test. [0] stores number of times org attempted to eat on empty location
		eaten.resize(group->population.size());
		for (int i = 0; i < (int) group->population.size(); i++) {
			foodHereOnArrival[i] = getGridValue(grid, currentLocation[i]);  //value of the food when we got here - needed for replacement method.
			if (saveOrgActions) { // if saveOrgActions save the type of the food org starts on.
				dataMap.append(to_string(group->population[i]->ID) + "_moves", foodHereOnArrival[i]);
			}
			eaten[i].resize(foodTypes + 1);
			if (recordFoodList) {
				group->population[i]->dataMap.append("foodList", -2);  // -2 = a world initialization, -1 = did not eat this step
			}

			group->population[i]->brains[brainName]->resetBrain();
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

		int realWorldUpdates = (worldUpdatesBaisedOnInitial <= 0) ? worldUpdates : (int)(MAXSCORE * worldUpdatesBaisedOnInitial);
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
				shared_ptr<AbstractBrain> evalBrain = group->population[orgIndex]->brains[brainName];
				if (senseWalls) {
					if (senseDown) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
							evalBrain->setInput(nodesAssignmentCounter++, (here == i + 1));
						}
					}
					if (senseFront) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
							evalBrain->setInput(nodesAssignmentCounter++, (front == i + 1));
						}
						evalBrain->setInput(nodesAssignmentCounter++, (front == WALL));
					}
					if (senseFrontSides) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
							evalBrain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
							evalBrain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
						}
						evalBrain->setInput(nodesAssignmentCounter++, (leftFront == WALL));
						evalBrain->setInput(nodesAssignmentCounter++, (rightFront == WALL));
					}
				} else {  // don't sense walls
					if (senseDown) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
							evalBrain->setInput(nodesAssignmentCounter++, (here == i + 1));
						}
					}
					if (senseFront) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
							evalBrain->setInput(nodesAssignmentCounter++, (front == i + 1));
						}
					}
					if (senseFrontSides) {
						for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
							evalBrain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
							evalBrain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
						}
					}
				}

				if (senseOther) {
					otherFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex]));
					otherLeftFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], turnLeft(facing[orgIndex])));
					otherRightFront = getGridValue(orgPositionsGrid, moveOnGrid(currentLocation[orgIndex], turnRight(facing[orgIndex])));

					if (senseFront) {
						evalBrain->setInput(nodesAssignmentCounter++, otherFront);
					}
					if (senseFrontSides) {
						evalBrain->setInput(nodesAssignmentCounter++, otherLeftFront);
						evalBrain->setInput(nodesAssignmentCounter++, otherRightFront);

					}
				}
				if (senseVisited) {
					visitedHere = getGridValue(visitedGrid, currentLocation[orgIndex]);
					visitedFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], facing[orgIndex]));
					visitedLeftFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], turnLeft(facing[orgIndex])));
					visitedRightFront = getGridValue(visitedGrid, moveOnGrid(currentLocation[orgIndex], turnRight(facing[orgIndex])));
					if (senseDown) {
						evalBrain->setInput(nodesAssignmentCounter++, visitedHere);
					}
					if (senseFront) {
						evalBrain->setInput(nodesAssignmentCounter++, visitedFront);
					}
					if (senseFrontSides) {
						evalBrain->setInput(nodesAssignmentCounter++, visitedLeftFront);
						evalBrain->setInput(nodesAssignmentCounter++, visitedRightFront);

					}

				}

				if (debug) {
					cout << "\n----------------------------\n";
					cout << "\ngeneration update: " << Global::update << "  world update: " << t << "\n";
					cout << "currentLocation: " << currentLocation[orgIndex].first << "," << currentLocation[orgIndex].second << "  :  " << facing[orgIndex] << "\n";
					cout << "inNodes: ";
					for (int i = 0; i < inputNodesCount; i++) {
						cout << evalBrain->readInput(i) << " ";
					}
					cout << "\nlast outNodes: ";
					for (int i = 0; i < outputNodesCount; i++) {
						cout << evalBrain->readOutput(i) << " ";
					}
					cout << "\n\n  -- brain update --\n\n";
				}

				// inputNodesCount is now set to the first output Brain State Address. we will not move it until the next world update!
				if (clearOutputs) {
					evalBrain->resetOutputs();
				}

				evalBrain->update();  // just run the update!

				// set output values
				// output1 has info about the first 2 output bits these [00 eat, 10 left, 01 right, 11 move]
				output1 = Bit(evalBrain->readOutput(0)) + (Bit(evalBrain->readOutput(1)) << 1);
				// output 2 has info about the 3rd output bit, which either does nothing, or is eat.
				if (alwaysEat) {
					output2 = 1;
				} else {
					output2 = Bit(evalBrain->readOutput(2));
				}

				if (saveOrgActions) { // if saveOrgActions save the output.
									  // + values in ID_moves are food
									  // - values are actions
					if (alwaysEat) {
						if (output1 != 0) {
							dataMap.append(to_string(group->population[orgIndex]->ID) + "_moves", -1 * (output1));
							// bits   int
							// -00 =  -0 =   no action // not recorded
							// -01 =  -1 =   right
							// -10 =  -2 =   left
							// -11 =  -3 =   forward
						}
					} else {
						if ((output1 << 1) + output2 != 0) {
							dataMap.append(to_string(group->population[orgIndex]->ID) + "_moves", -1 * ((output1 << 1) + output2));
							// bits    int
							// -000 =  -0 =   no action // not recorded
							// -001 =  -1 =   eat
							// -010 =  -2 =   right
							// -011 =  -3 =   eat
							// -100 =  -4 =   left
							// -101 =  -5 =   eat
							// -110 =  -6 =   forward
							// -111 =  -7 =   eat
						}
					}
				}

				if ((output2 == 1 && !alwaysEat) || (alwaysEat && lastActionWasMove)) {  // if org tried to eat or always eat and last action was move
					int foodHere = getGridValue(grid, currentLocation[orgIndex]);
					if ((recordFoodList && foodHere != 0) || (recordFoodList && recordFoodListEatEmpty)) {
						group->population[orgIndex]->dataMap.append("foodList", foodHere);  // record that org ate food (or tried to at any rate)
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
						scores[orgIndex] += foodRewards[foodHere];  // you ate a food... good for you! (or bad)
						//cout << "  ate food: " << foodHere << " reward: " << foodRewards[foodHere] << " total score: " << scores[orgIndex] << endl;
						setGridValue(grid, currentLocation[orgIndex], 0);					// clear this location
					} else { // no food here!
						scores[orgIndex] += foodRewards[foodHere]; // you ate a food... good for you! (or bad)
						//cout << "  ate food: " << foodHere << " reward: " << foodRewards[foodHere] << " total score: " << scores[orgIndex] << endl;
					}
				} else {
					if (recordFoodList && recordFoodListNoEat) {
						group->population[orgIndex]->dataMap.append("foodList", -1);  // record that org did not try to eat this time
					}
				}
				lastActionWasMove = false;
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
							lastActionWasMove = true;
							scores[orgIndex] += rewardForMove;
							if (getGridValue(grid, currentLocation[orgIndex]) == EMPTY) {  // if the current location is empty...
								//cout << replacement << endl;
								// replacement rules
								// if replacementRule[food] == -1
								//   case replacementDefaultRule
								//     -1 random
								//     0 no replacement
								//     1 other (0 can be replaced by other)
								// else replacementRules[food]
								if (replacementRules[foodHereOnArrival[orgIndex]] == -1) {
									if (replacementDefaultRule == -1 || (replacementDefaultRule == 1 && foodHereOnArrival[orgIndex] == EMPTY)) {  // if replacement = random (-1).or replacment other (1) and was empty..
										setGridValue(grid, currentLocation[orgIndex], pickFood(-1));  // plant a random food
										//cout << "replacement = -1 (random) .. ";
									} else if (replacementDefaultRule == 1 && foodHereOnArrival[orgIndex] > EMPTY) {  // if replacement = other (1) and there was some food here when org got here...
										setGridValue(grid, currentLocation[orgIndex], pickFood(foodHereOnArrival[orgIndex]));  // plant a different food when what was here
										//cout << "replacement = 1 (other) and EMPTY.. ";
									} else { // replacement = 0, no replacement
										//cout << "no replace .. ";
									}
								} else { // this food type has a replacment rule
									setGridValue(grid, currentLocation[orgIndex], replacementRules[foodHereOnArrival[orgIndex]]); // plant food based on replacement rule
								}

								//cout << "move done." << endl;
								// if replacement = no replacement (0), no replacement/do nothing
							}
							setGridValue(orgPositionsGrid, currentLocation[orgIndex], 0);  // show location as not occupied.
							setGridValue(visitedGrid, currentLocation[orgIndex], 1);  // leave a marker in the visitedGrid
							currentLocation[orgIndex] = moveOnGrid(currentLocation[orgIndex], facing[orgIndex]);  // move organism
							setGridValue(orgPositionsGrid, currentLocation[orgIndex], 1);  // show new location as occupied.
							if (getGridValue(visitedGrid, currentLocation[orgIndex]) == 0) { // if this is a novel location
								novelty[orgIndex]++;
								scores[orgIndex] += rewardSpatialNovelty;
							} else { // if anyone has been here before
								repeated[orgIndex]++;
							}
							foodHereOnArrival[orgIndex] = getGridValue(grid, currentLocation[orgIndex]);  //value of the food when we got here - needed for replacement method.
							if (saveOrgActions) { // if saveOrgActions save the type of the food org moves onto.
								dataMap.append(to_string(group->population[orgIndex]->ID) + "_moves", foodHereOnArrival[orgIndex]);
							}
						}
						break;
					}
				}

				if (debug) {
					for (int i = 0; i < outputNodesCount; i++) {
						cout << Bit(evalBrain->readOutput(i)) << " ";
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
				string temp_name = "food" + to_string(f);  // make food names i.e. food1, food2, etc.
				group->population[i]->dataMap.append(temp_name, eaten[i][f]);

			}
			if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
				(eaten[i][1] > eaten[i][2]) ? group->population[i]->dataMap.append("consumptionRatio", (double) eaten[i][1] / (double) (eaten[i][2] + 1)) : group->population[i]->dataMap.append("consumptionRatio", (double) eaten[i][2] / (double) (eaten[i][1] + 1));
			}

			group->population[i]->dataMap.append("total", total_eaten);  // total food eaten (regardless of type)

			group->population[i]->dataMap.append("switches", switches[i]);
			group->population[i]->dataMap.append("novelty", novelty[i]);
			group->population[i]->dataMap.append("repeated", repeated[i]);

//			if (scores[i] < 0.0) {
//				scores[i] = 0.0;
//			}

			summedScores[i] += scores[i] / MAXSCORE;
		}
	}

	if (visualize) {  // save endflag.
		FileManager::writeToFile(visualizationFileName, "*end*", "8," + to_string(WorldX) + ',' + to_string(WorldY));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
	}

	for (int orgIndex = 0; orgIndex < (int) group->population.size(); orgIndex++) {

		//group->population[orgIndex]->score = summedScores[orgIndex] / numWorlds;
		group->population[orgIndex]->dataMap.append("score", summedScores[orgIndex] / numWorlds);

		// set up output behaviors for entries in data map

		if (saveOrgActions) { // if saveOrgActions save the output.
			vector<int> simplifiedMoves;
			auto moves = dataMap.getIntVector(to_string(group->population[orgIndex]->ID) + "_moves");
			int movesCount = moves.size();
			int turn;
			int leftTurnCount = 0;
			int rightTurnCount = 0;
			//cout << "movesCount: " << movesCount << endl;
			for (int i = 0; i < movesCount; i++) {
				turn = 0;
				//cout << "time: " << i << endl;
				//cout << "alwaysEat " << alwaysEat << endl;
				if (alwaysEat) { // there will only be turns and move (0,1,2,3)
					//cout << "  AE  ";
					while (moves[i] < 0 && i < movesCount) { // while not food (>=0) and moves left
						//cout << ".." << moves[i];
						if (moves[i] == -3) { // move
							//cout << moves[i] << " forward " << turn << endl;
							if (i + 1 < movesCount && moves[i + 1] < 0) { // org failed to move
								moves[i] = 9; // 9 in the output will indicate org attempted to move
							} else {
								i++; // this should put us on a new food (including 0), no need to mark a move, it will be indicated by a value of 0 to 9
							}
						} else if (moves[i] == -1) { // right
							turn++;
							//cout << moves[i] << " right " << turn << endl;
							i++;
						} else if (moves[i] == -2) { // left
							turn--;
							//cout << moves[i] << " left " << turn << endl;
							i++;
							//cout << moves[i] << " no action " << turn << endl;
						} else { // no action
							i++;
						}
					}
					//cout << "before: " << turn;
					turn = (abs(turn) % 8) * ((0 < turn) - (turn < 0));
					//cout << "  after: " << turn << endl;

					if (turn != 0) {
						if (turn < 0) {
							turn = 8 + turn;
						}
						if (turn == 1) {
							rightTurnCount++;
							simplifiedMoves.push_back(21); // soft turn
						}
						if (turn == 2) {
							rightTurnCount++;
							simplifiedMoves.push_back(22); // turn
						}
						if (turn == 3) {
							rightTurnCount++;
							simplifiedMoves.push_back(23); // hard turn
						}
						if (turn == 4) {
							simplifiedMoves.push_back(40); // turn around
						}
						if (turn == 5) {
							leftTurnCount++;
							simplifiedMoves.push_back(33); // hard turn
						}
						if (turn == 6) {
							leftTurnCount++;
							simplifiedMoves.push_back(32); // turn
						}
						if (turn == 7) {
							leftTurnCount++;
							simplifiedMoves.push_back(31); // soft turn
						}
					}
					if (i < movesCount) { // this must be a food (including 0)
						simplifiedMoves.push_back(moves[i]);
					}
				}
			}

			// now count left and right turns and correct if needed so that 2x turns are Max
			for (int i = 0; i < (int) simplifiedMoves.size(); i++) {
				if (leftTurnCount > rightTurnCount) {
					if (simplifiedMoves[i] > 20 && simplifiedMoves[i] < 40) { // for each value that is a turn, reverse it's direction
						if (simplifiedMoves[i] == 21) {
							simplifiedMoves[i] = 31;
						} else if (simplifiedMoves[i] == 22) {
							simplifiedMoves[i] = 32;
						} else if (simplifiedMoves[i] == 23) {
							simplifiedMoves[i] = 33;
						} else if (simplifiedMoves[i] == 31) {
							simplifiedMoves[i] = 21;
						} else if (simplifiedMoves[i] == 32) {
							simplifiedMoves[i] = 22;
						} else if (simplifiedMoves[i] == 33) {
							simplifiedMoves[i] = 23;
						}
					}
				}
			}

			dataMap.set(to_string(group->population[orgIndex]->ID) + "_SimplifiedMoves", simplifiedMoves);

		}

	}

	if (saveOrgActions) { // if saveOrgActions save the output.
		dataMap.writeToFile(visualizationFileName+"_actions.txt");
	}
}

void BerryWorld::SaveWorldState(string fileName, vector<int> grid, vector<int> vistedGrid, vector<pair<int, int>> currentLocation, vector<int> facing, bool reset) {

	string stateNow = "";

	if (reset) {
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
