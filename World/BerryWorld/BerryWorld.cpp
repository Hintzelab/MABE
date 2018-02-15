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

shared_ptr<ParameterLink<int>> BerryWorld::worldSizeXPL = Parameters::register_parameter("WORLD_BERRY-worldSizeX", 8, "width of world if no map files are provided (in which case a random world is generated)");
shared_ptr<ParameterLink<int>> BerryWorld::worldSizeYPL = Parameters::register_parameter("WORLD_BERRY-worldSizeY", 8, "height of world if no map files are provided");
shared_ptr<ParameterLink<bool>> BerryWorld::worldHasWallPL = Parameters::register_parameter("WORLD_BERRY-worldHasWall", true, "if map is not provided, does the generated world have a wall around the edge? (maps must define their own walls)");
shared_ptr<ParameterLink<int>> BerryWorld::rotationResolutionPL = Parameters::register_parameter("WORLD_BERRY_GEOMETRY-directions", 8, "the number of directions organisms can face\n  i.e. 36 = 10 degree turns, 8 = 45 degree turns, etc.");
shared_ptr<ParameterLink<double>> BerryWorld::maxTurnPL = Parameters::register_parameter("WORLD_BERRY_GEOMETRY-maxTurn", .25, "harvesters can turn this amount per world update (i.e. if directions = 16 and maxTurn = .25, then 4 turns are allowed per\nworld update). This is implemented by allowing multipile turn (by 360 / directions degrees) actions in a single world update.");

shared_ptr<ParameterLink<bool>> BerryWorld::seeOtherPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-seeOther", false, "can harvesters detect other harvesters with their vision sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::smellOtherPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellOther", false, "can harvesters detect other harvesters with their smell sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::seeFoodPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-seeFood", true, "can harvesters detect food with their vision sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::smellFoodPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellFood", true, "can harvesters detect food with their smell sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::seeWallsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-seeWalls", true, "can harvesters detect walls with their vision sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::smellWallsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellWalls", false, "can harvesters detect walls with their smell sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::useDownSensorPL = Parameters::register_parameter("WORLD_BERRY_SENSORS-downSensorSwitch", true, "can harvesters detect food at their current location?");
shared_ptr<ParameterLink<bool>> BerryWorld::useSmellSensorPL = Parameters::register_parameter("WORLD_BERRY_SENSORS-smellSensorSwitch", false, "do harvesters have a smell sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::useVisionSensorPL = Parameters::register_parameter("WORLD_BERRY_SENSORS-visionSensorSwitch", false, "do harvesters have a vision sensor?");
shared_ptr<ParameterLink<bool>> BerryWorld::usePerfectSensorPL = Parameters::register_parameter("WORLD_BERRY_SENSORS-perfectSensorSwitch", true, "do harvesters have a perfect sensor? (reqires direction is 4 or 8)");
shared_ptr<ParameterLink<string>> BerryWorld::perfectSensorFilePL = Parameters::register_parameter("WORLD_BERRY_SENSORS_PERFECT-perfectSensorFile", (string)"World/BerryWorld/perfectSensors/smallFront.txt", "file which defines perfect sensor?");

shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsFoodPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsFood", true, "can perfect sensor detect food?");
shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsOtherPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsOther", false, "can perfect sensor detect other?");
shared_ptr<ParameterLink<bool>> BerryWorld::perfectDetectsWallsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_PERFECT-perfectSensorDetectsWalls", false, "can perfect sensor detect walls?");



shared_ptr<ParameterLink<bool>> BerryWorld::wallsBlockVisonSensorsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-wallsBlockVisonSensors", false , "if true, objects behind walls can not been seen (setting to false allows for larger sensors)");
shared_ptr<ParameterLink<bool>> BerryWorld::wallsBlockSmellSensorsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-wallsBlockSmellSensors", false, "if true, objects behind walls can not been smelled (setting to false allows for larger sensors)");

shared_ptr<ParameterLink<int>> BerryWorld::visionSensorDistancePL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-visionSensorDistanceMax", 6, "how far can orgs see?");
shared_ptr<ParameterLink<int>> BerryWorld::visionSensorArcSizePL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-visionSensorArcSize", 5, "how wide is a vision arc (degrees)");
shared_ptr<ParameterLink<string>> BerryWorld::visionSensorDirectionsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_VISION-visionSensorDirections", (string) "[-2,0,1]", "what directions can org see? (if value is < 1, then this value will be multipied by directions)");

shared_ptr<ParameterLink<int>> BerryWorld::smellSensorDistancePL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellSensorDistanceMax", 2, "how far can orgs smell?");
shared_ptr<ParameterLink<int>> BerryWorld::smellSensorArcSizePL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellSensorArcSize", 180, "how wide is a smell arc (degrees)");
shared_ptr<ParameterLink<string>> BerryWorld::smellSensorDirectionsPL = Parameters::register_parameter("WORLD_BERRY_SENSORS_SMELL-smellSensorDirections", (string) "[0,.25,.5,.75]", "what directions can org smell? (if value is < 1, then this value will be multipied by directions)");

shared_ptr<ParameterLink<int>> BerryWorld::evalTimePL = Parameters::register_parameter("WORLD_BERRY-evalationTime", 400, "number of world updates per generation");
shared_ptr<ParameterLink<int>> BerryWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_BERRY-evaluationsPerGeneration", 1, "how many times should this world be run when evaluating an agent (useful to correct for chance)");
shared_ptr<ParameterLink<int>> BerryWorld::evaluateGroupSizePL = Parameters::register_parameter("WORLD_BERRY_GROUP-groupSize", 1, "organisms will be evaluated in groups of this size, the groups will be determined randomly.  \nIf (population size / groupSize) has a remainder, some organisms will be evaluated more then once.\n  -1 indicates to evaluate all organisms at the same time.");
shared_ptr<ParameterLink<string>> BerryWorld::cloneScoreRulePL = Parameters::register_parameter("WORLD_BERRY_GROUP-cloneScoreRule", (string)"ALL", "if clones is > 0 what score rule will be used. ALL = score all clones, BEST = score only best clone, WORST = score only worst clone");
shared_ptr<ParameterLink<int>> BerryWorld::clonesPL = Parameters::register_parameter("WORLD_BERRY_GROUP-clones", 0, "evaluation groups will be padded with this number of clones for each harvester. I.e. if group size is 3 and clones is 1, the actual group size will be 6");

shared_ptr<ParameterLink<double>> BerryWorld::switchCostPL = Parameters::register_parameter("WORLD_BERRY_SCORE-switchCost", 1.4, "cost paid when switching between food types");
shared_ptr<ParameterLink<double>> BerryWorld::hitWallCostPL = Parameters::register_parameter("WORLD_BERRY_SCORE-hitWallCost", 0.0, "cost paid if a move failed because of a wall being in the way");
shared_ptr<ParameterLink<double>> BerryWorld::hitOtherCostPL = Parameters::register_parameter("WORLD_BERRY_SCORE-hitOtherCost", 0.0, "cost paid if a move failed because of another harvester being in the way");
shared_ptr<ParameterLink<int>> BerryWorld::alwaysStartOnPL = Parameters::register_parameter("WORLD_BERRY-alwaysStartOnResource", -1, "all organisms will start on this type of food (-1 = random start locations)");

shared_ptr<ParameterLink<int>> BerryWorld::foodTypesPL = Parameters::register_parameter("WORLD_BERRY_FOOD-foodTypes", 2, "how many different types of food (1,2 or 3)");

shared_ptr<ParameterLink<string>> BerryWorld::initialFoodDistributionPL = Parameters::register_parameter("WORLD_BERRY_FOOD-initialFoodDistribution", (string)"[-1]", "values to use when filling in (auto generated maps)\n  [1,1,2] = 2/3 1, 1/3 2\n  [-1,0,2] = 1/3 random from food types, 1/3 0, 1/3 2");

shared_ptr<ParameterLink<double>> BerryWorld::reward1PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward1", 1.0, "reward for eating food1");
shared_ptr<ParameterLink<double>> BerryWorld::reward2PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward2", 1.0, "reward for eating food2");
shared_ptr<ParameterLink<double>> BerryWorld::reward3PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward3", 1.0, "reward for eating food3");
shared_ptr<ParameterLink<double>> BerryWorld::reward4PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward4", 1.0, "reward for eating food4");
shared_ptr<ParameterLink<double>> BerryWorld::reward5PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward5", 1.0, "reward for eating food5");
shared_ptr<ParameterLink<double>> BerryWorld::reward6PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward6", 1.0, "reward for eating food6");
shared_ptr<ParameterLink<double>> BerryWorld::reward7PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward7", 1.0, "reward for eating food7");
shared_ptr<ParameterLink<double>> BerryWorld::reward8PL = Parameters::register_parameter("WORLD_BERRY_SCORE-reward8", 1.0, "reward for eating food8");

shared_ptr<ParameterLink<string>> BerryWorld::replace1PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace1", (string)"[-1]", "when food 1 is eaten, what should replace it?\nreplacement is chosen randomly from list\nelements that appear more then once are more likely to be selected\n-1 will choose randomly from available food types");
shared_ptr<ParameterLink<string>> BerryWorld::replace2PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace2", (string)"[-1]", "replace rule for food2");
shared_ptr<ParameterLink<string>> BerryWorld::replace3PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace3", (string)"[-1]", "replace rule for food3");
shared_ptr<ParameterLink<string>> BerryWorld::replace4PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace4", (string)"[-1]", "replace rule for food4");
shared_ptr<ParameterLink<string>> BerryWorld::replace5PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace5", (string)"[-1]", "replace rule for food5");
shared_ptr<ParameterLink<string>> BerryWorld::replace6PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace6", (string)"[-1]", "replace rule for food6");
shared_ptr<ParameterLink<string>> BerryWorld::replace7PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace7", (string)"[-1]", "replace rule for food7");
shared_ptr<ParameterLink<string>> BerryWorld::replace8PL = Parameters::register_parameter("WORLD_BERRY_FOOD-replace8", (string)"[-1]", "replace rule for food8");

shared_ptr<ParameterLink<double>> BerryWorld::poison0PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison0", 0.0, "cost for landing on empty");
shared_ptr<ParameterLink<double>> BerryWorld::poison1PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison1", 0.0, "cost for landing on food1");
shared_ptr<ParameterLink<double>> BerryWorld::poison2PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison2", 0.0, "cost for landing on food2");
shared_ptr<ParameterLink<double>> BerryWorld::poison3PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison3", 0.0, "cost for landing on food3");
shared_ptr<ParameterLink<double>> BerryWorld::poison4PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison4", 0.0, "cost for landing on food4");
shared_ptr<ParameterLink<double>> BerryWorld::poison5PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison5", 0.0, "cost for landing on food5");
shared_ptr<ParameterLink<double>> BerryWorld::poison6PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison6", 0.0, "cost for landing on food6");
shared_ptr<ParameterLink<double>> BerryWorld::poison7PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison7", 0.0, "cost for landing on food7");
shared_ptr<ParameterLink<double>> BerryWorld::poison8PL = Parameters::register_parameter("WORLD_BERRY_SCORE-poison8", 0.0, "cost for landing on food8");

shared_ptr<ParameterLink<bool>> BerryWorld::allowMoveAndEatPL = Parameters::register_parameter("WORLD_BERRY-allowEatAndMove", false, "if false, and there is an eat output and move output, move outputs will be ignored\nif true and there is both an eat and move output, eat will happen first, then move.");
shared_ptr<ParameterLink<bool>> BerryWorld::alwaysEatPL = Parameters::register_parameter("WORLD_BERRY-alwaysEat", false, "if true, organism will have only two outputs. organism will always attempt eat before moving. If false, there will be a third input for eat");

shared_ptr<ParameterLink<string>> BerryWorld::groupNameSpacePL = Parameters::register_parameter("WORLD_BERRY_NAMESPACE-NameSpace_group", (string)"root::", "namespace of group to be evaluated");
shared_ptr<ParameterLink<string>> BerryWorld::brainNameSpacePL = Parameters::register_parameter("WORLD_BERRY_NAMESPACE-NameSpace_brain", (string)"root::", "namespace for parameters used to define brain");

shared_ptr<ParameterLink<double>> BerryWorld::moveDefaultPL = Parameters::register_parameter("WORLD_BERRY_GEOMETRY-moveDefault", 1.0, "distance harvester moves when output is move");
shared_ptr<ParameterLink<double>> BerryWorld::moveMinPL = Parameters::register_parameter("WORLD_BERRY_GEOMETRY-moveMin", 0.0, "min distance harvester moves every update (even if output is not move)");
shared_ptr<ParameterLink<bool>> BerryWorld::snapToGridPL = Parameters::register_parameter("WORLD_BERRY_GEOMETRY-snapTpGrid", true, "if true, harvester will always move to center of nearest space");

shared_ptr<ParameterLink<string>> BerryWorld::mapFilesPL = Parameters::register_parameter("WORLD_BERRY_MAPS-mapFiles", (string) "[]", "list of names of file containing maps (if NONE, random map will be generated) e.g. [World/HarvestXWorld/maps/patch.txt]");
shared_ptr<ParameterLink<string>> BerryWorld::whichMapsPL = Parameters::register_parameter("WORLD_BERRY_MAPS-whichMaps", (string) "[1/1]", "list of maps from mapFiles to use to evaluate each harvester, type/name will select named map,\n* in either position = all,\na number = randomly select that number of maps (will fail if there are not enough maps)");


vector<int> pickUnique(int numAvalible, int numPicks) {
	vector<int> picks;
	for (int i = 0; i < numAvalible; ++i) {
		picks.push_back(i);
	}
	if (numPicks != numAvalible) {
		for (int i = 0; i < numPicks; ++i) {
			int j = Random::getInt(i, numAvalible - 1); 
			if (j != i) {
				auto temp = picks[i];
				picks[i] = picks[j];
				picks[j] = temp;
			}
		}
	}
	vector<int> newVec(picks.begin(), picks.begin() + numPicks);
	return newVec;
}

bool readFileLine(ifstream& FILE, string& rawLine, stringstream& ss) {
	bool atEOF = false;
	atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
	string cleanLine = rawLine;
	cleanLine.erase(remove_if(cleanLine.begin(), cleanLine.end(), ::isspace), cleanLine.end());
	while (!atEOF && cleanLine[0]!='#' && cleanLine.size() == 0) { // keep reading lines until we find a line that is not end of file, or starting with # or empty
		atEOF = loadLineToSS(FILE, rawLine, ss); // read next line of file
		cleanLine = rawLine;
		cleanLine.erase(remove_if(cleanLine.begin(), cleanLine.end(), ::isspace), cleanLine.end());
	}
	return atEOF;
}

Vector2d<char> loadSquareGridOfIntsFromFile(ifstream& FILE) {
	// this assumes we have a file with a square char "image"
	string rawLine, rubbishString;
	char charBuffer;
	stringstream ss("");
	bool atEOF = false;
	bool goodRead = false;

	int gridSize = 0;

	vector<char> rawgrid;
	Vector2d<char> grid;

	atEOF = readFileLine(FILE, rawLine, ss);
	gridSize = rawLine.size();
	for (int r = 0; r < gridSize; r++) {
		for (int c = 0; c < gridSize; c++) {
			rawgrid.push_back(ss.get());
		}
		atEOF = readFileLine(FILE, rawLine, ss);
	}
	grid.reset(gridSize, gridSize);
	grid.assign(rawgrid);
	//grid.showGrid();
	return grid;
}

void fillInVision(vector<vector<vector<Point2d>>>& vision, bool reflectFirst = false) {
	vector<vector<Point2d>> tempVec;
	int numArcs = vision.size();

	// if vision is size one then there is one arc (forward (0)) which will be reflected to left(1), down(2) and right(3).
	// i.e. 4 directions

	// if vision is size two then there are two arcs (forward(0) and forward right(1)) which will be reflected to right(2), back right (3), back(4)...forwad left (7)
	// i.e. 8 directions

	if (reflectFirst) { // this is used when there are non semetic arcs (i.e. left vs. right arc)
		// one arc is passed in with reflectFirst = true and this will return the rotations for the reflected arc.
		for (auto distance : vision[0]) {
			tempVec.push_back({});
			for (auto p : distance) {
				tempVec.back().push_back(Point2d(-p.x, p.y)); // reflect around x
			}
		}
		vision[0] = tempVec;
		if (numArcs == 2) {
			tempVec.clear();
			for (auto distance : vision[1]) {
				tempVec.push_back({});
				for (auto p : distance) {
					tempVec.back().push_back(Point2d(-p.y, -p.x)); // reflect around x,-y
				}
			}
			vision[1] = tempVec;
			tempVec.clear();
		}
	}
	if (numArcs == 2) { // we need to make right, down right, down, down left, left and up left
		for (int i = 0; i < 6; i++) {
			for (auto line : vision[i % 2]) {
				tempVec.push_back({});
				for (auto p : line) {
					if ((int)i / 2 == 0) {
						tempVec.back().push_back(Point2d(-p.y, p.x));
					}
					else if ((int)i / 2 == 1) {
						tempVec.back().push_back(Point2d(-p.x, -p.y));
					}
					else {
						tempVec.back().push_back(Point2d(p.y, -p.x));
					}
				}
			}
			vision.push_back(tempVec);
			tempVec.clear();
		}
	}
	else { // numArcs = 1, we only need to create left, down and right
		for (int i = 0; i < 3; i++) {
			for (auto line : vision[0]) {
				tempVec.push_back({});
				for (auto p : line) {
					if ((int)i == 0) {
						tempVec.back().push_back(Point2d(-p.y, p.x));
					}
					else if ((int)i == 1) {
						tempVec.back().push_back(Point2d(-p.x, -p.y));
					}
					else {
						tempVec.back().push_back(Point2d(p.y, -p.x));
					}
				}
			}
			vision.push_back(tempVec);
			tempVec.clear();
		}
	}
}

bool BerryWorld::WorldMap::loadMap(ifstream& FILE, const string _fileName) {
	file = _fileName;
	string rawLine, rubbishString;
	char charBuffer;
	stringstream ss("");
	bool atEOF = false;
	bool goodRead = false;

	worldX = 0;
	worldY = 0;

	int intConverter;
	vector<int> rawMap;
	vector<int> rawStartMap;

	if (FILE.is_open()) {
		atEOF = readFileLine(FILE, rawLine, ss);
		ss >> rubbishString >> rubbishString >> name;
		// now that we have the name and type, we can make the PT. The namespace will be based on the parents (the world) name space.
		// PT = make_shared<ParametersTable>(parentPT->getTableNameSpace()+"::"+fileName+"__"+name,Parameters::root);
		if (name != "") {
			atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
			worldX = rawLine.size(); // x size of map is length of this line
			while (!atEOF && ss.peek() != '*' && ss.peek() != '+') {
				if (rawLine.size() != worldX) {
					cout << "  While loading map " << name << " from file: " << file << ", map has a line with the incorrect length\nexiting" << endl;
					exit(1);
				}
				for (int x = 0; x < worldX; x++) { // load first line of map, keep loading chars until end of line (i.e. ss.fail because char could not be read)
					charBuffer = ss.get();
					rawMap.push_back(charBuffer - '0');
					//// experiment to allow random values in maps - causes problems when trying to creat valid start locations... ////////////////////
					//if (charBuffer == 'r') {
					//	rawMap.push_back(-1);
					//}
					//else {
					//	rawMap.push_back(charBuffer - '0');
					//}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				}
				worldY++;
				atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
			}
			if (ss.peek() == '+') {
				useStartMap = true;
				for (int y = 0; y < worldY; y++) {
					atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
					if (ss.peek() == '*') {
						cout << "  While loading map " << name << " from file: " << file << ", starting locations size is smaller then map size\nexiting" << endl;
						exit(1);
					}
					if (rawLine.size() != worldX) {
						cout << "  While loading map " << name << " from file: " << file << ", starting locations has a line with the incorrect length\nexiting" << endl;
						exit(1);
					}
					for (int x = 0; x < worldX; x++) {
						rawStartMap.push_back(ss.get() - '0');
					}
				}
				atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
			}

			// LOAD GENERATORS
			while (ss.peek() == 'G') { // this line defines a generator
				if (AbstractWorld::debugPL->get()) {
					cout << "  While loading map found generator definition: " << rawLine << endl;
				}
				string cleanLine;
				for (auto c : rawLine) { // remove spaces for rawLine to create cleanLine
					if (c != ' ') {
						cleanLine.push_back(c);
					}
				}
				vector<string> splitLine(1);
				for (auto c : cleanLine) { // make splitLine by breaking up cleanLine on ':'
					if (c == ':') {
						splitLine.push_back("");
					}
					else {
						splitLine.back().push_back(c);
					}
				}

				vector<int> locations;
				vector<int> size;
				vector<double> rate;
				vector <vector<double>> resourcePrograms;

				convertCSVListToVector(splitLine[1], locations);
				convertCSVListToVector(splitLine[2], size);
				convertCSVListToVector(splitLine[3], rate);

				vector<string> resourceRulesStrings(1); // this will hold string for resource rules
				
				bool inList = false;
				// get resource programs
				for (int c = 1; c < (int)splitLine[4].size()-1; c++) {
					if (splitLine[4][c] == '[') { // if we are in a list, ignore ','s until out of list
						inList = true;
					}
					if (splitLine[4][c] == ']') { // found end of list
						inList = false;
					}
					if (!inList && splitLine[4][c] == ',') { // if we fond a ',' outsize of a list then add a new program
						resourceRulesStrings.push_back("");
					}
					else {
						resourceRulesStrings.back().push_back(splitLine[4][c]); // else add char to current program
					} 
				}

				for (auto rule : resourceRulesStrings) {
					resourcePrograms.push_back({});
					convertCSVListToVector(rule, resourcePrograms.back()); // convert each resource rule from string to numbers
				}
				
				int replaceRule; // set replacment rule
				if (splitLine[5] == "RN") {
					replaceRule = 0;
				}
				else  if (splitLine[5] == "RF") {
					replaceRule = 1;
				}
				else  if (splitLine[5] == "RW") {
					replaceRule = 2;
				}
				else  if (splitLine[5] == "RA") {
					replaceRule = 3;
				}
				else {
					cout << "  While loading generator for map " << name << " from file: " << file << ", found generator with bad replace rule (must be RN,RF,RW or, RA)\nexiting" << endl;
					exit(1);
				}
				for (int i = 0; i < locations.size(); i += 2) { // make a resource generator for each pair in locations list
					ResourceGenerator newGenerator;
					newGenerator.x = locations[i];
					newGenerator.y = locations[i+1];
					if (size[0] > 1 || size[1] > 1) {
						newGenerator.geo = 1; // area/square generator
						newGenerator.x1 = newGenerator.x + size[0] - 1;
						newGenerator.y1 = newGenerator.y + size[1] - 1;
					}
					else {
						newGenerator.geo = 0; // point generator
					}
					newGenerator.rate = rate;
					newGenerator.resourcePrograms = resourcePrograms;
					newGenerator.replaceRule = replaceRule;
					generators.push_back(newGenerator);
				}

				atEOF = readFileLine(FILE, rawLine, ss); // read next line of file
			}
			if (ss.peek() == '*') { // this checks that we read to the end of the map definition
				goodRead = true;
			}
			else {
				cout << "  While loading map " << name << " from file: " << file << ", a problem was encountered. possibly, starting locations size is larger then map size (or it's something else...)\nexiting" << endl;
				exit(1);
			}
		}
	}
	data.reset(worldX, worldY);
	data.assign(rawMap);
	if (useStartMap) {
		startData.reset(worldX, worldY);
		startData.assign(rawStartMap);
	}
	return goodRead;
}

BerryWorld::BerryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	cout << "Berry world setup:" << endl;

	////////////////////////////////////////////////////////
	// localize values /////////////////////////////////////
	////////////////////////////////////////////////////////

	convertCSVListToVector(initialFoodDistributionPL->get(PT), initialFoodDistribution);
	moveDefault = moveDefaultPL->get(PT);
	moveMin = moveMinPL->get(PT);
	snapToGrid = snapToGridPL->get(PT);
	worldX = worldSizeXPL->get(PT);
	worldY = worldSizeYPL->get(PT);
	alwaysEat = alwaysEatPL->get(PT);
	allowMoveAndEat = allowMoveAndEatPL->get(PT) || alwaysEat;


	if (cloneScoreRulePL->get(PT) == "ALL") {
		cloneScoreRule = 0;
	}
	else  if (cloneScoreRulePL->get(PT) == "BEST") {
		cloneScoreRule = 1;
	}
	else  if (cloneScoreRulePL->get(PT) == "WORST") {
		cloneScoreRule = 2;
	}
	else {
		cout << "    cloneScoreRule \"" << cloneScoreRulePL->get(PT) << "\" is not a valid rule - must be ALL, BEST or WORST.\n    exiting." << endl;
		exit(1);
	}
	////////////////////////////////////////////////////////
	// setup food and poison ///////////////////////////////
	////////////////////////////////////////////////////////
	foodRewards = { 0,
		reward1PL->get(PT),
		reward2PL->get(PT),
		reward3PL->get(PT),
		reward4PL->get(PT),
		reward5PL->get(PT),
		reward6PL->get(PT),
		reward7PL->get(PT),
		reward8PL->get(PT) };

	foodTypes = foodTypesPL->get(PT);
	replaceRules.resize(9);
	replaceRules[0] = {0};
	convertCSVListToVector(replace1PL->get(PT), replaceRules[1]);
	convertCSVListToVector(replace2PL->get(PT), replaceRules[2]);
	convertCSVListToVector(replace3PL->get(PT), replaceRules[3]);
	convertCSVListToVector(replace4PL->get(PT), replaceRules[4]);
	convertCSVListToVector(replace5PL->get(PT), replaceRules[5]);
	convertCSVListToVector(replace6PL->get(PT), replaceRules[6]);
	convertCSVListToVector(replace7PL->get(PT), replaceRules[7]);
	convertCSVListToVector(replace8PL->get(PT), replaceRules[8]);

	for (int i = 0; i <= foodTypes; i++) {
		for (int repVal : replaceRules[i]) {
			if (repVal > foodTypes && repVal != 9) {
				cout << "    replacement rule for food" << i << " includes values > foodTypes.\n    exiting." << endl;
				exit(1);
			}
		}
	}

	poisonRules = { poison0PL->get(PT),
		poison1PL->get(PT),
		poison2PL->get(PT),
		poison3PL->get(PT),
		poison4PL->get(PT),
		poison5PL->get(PT),
		poison6PL->get(PT),
		poison7PL->get(PT),
		poison8PL->get(PT) };

	////////////////////////////////////////////////////////
	// setup vision ////////////////////////////////////////
	////////////////////////////////////////////////////////
	rotationResolution = rotationResolutionPL->get(PT);
	maxTurn = maxTurnPL->get(PT);

	// vistion and smell sensor arcs are defines with lists which contian directions
	// if < 1 then, this value indicates a direction = rotationResolution * value
	// if >= then this value defines the direction directly

	visionSensorDistance = visionSensorDistancePL->get(PT);
	visionSensorArcSize = visionSensorArcSizePL->get(PT);
	if (useVisionSensorPL->get(PT)) {
		vector<double> tempVec;
		convertCSVListToVector(visionSensorDirectionsPL->get(PT), tempVec);
		for (int i = 0; i < tempVec.size(); i++) {
			visionSensorDirections.push_back(abs(tempVec[i])<1?(int)(tempVec[i] * rotationResolution):(int)tempVec[i]);
		}
	}
	visionSensorCount = (int)visionSensorDirections.size();

	smellSensorDistance = smellSensorDistancePL->get(PT);
	smellSensorArcSize = smellSensorArcSizePL->get(PT);
	if (useSmellSensorPL->get(PT)) {
		vector<double> tempVec;
		convertCSVListToVector(smellSensorDirectionsPL->get(PT), tempVec);
		for (int i = 0; i < tempVec.size(); i++) {
			smellSensorDirections.push_back(abs(tempVec[i]<1) ? (int)(tempVec[i] * rotationResolution) : (int)tempVec[i]);
		}
	}
	smellSensorCount = (int)smellSensorDirections.size();

	wallsBlockVisonSensors = wallsBlockVisonSensorsPL->get(PT) ? WALL : -1;
	wallsBlockSmellSensors = wallsBlockSmellSensorsPL->get(PT) ? WALL : -1;
	if (useDownSensorPL->get(PT)) {
		cout << "  using Down Sensor." << endl;
	}
	if (visionSensorCount > 0) {
		cout << "  using Vision Sensor: (arc: " << visionSensorArcSize * -.5 << "," << visionSensorArcSize * .5 << "    distances: " << 1 << "," << visionSensorDistance << "     resolution: " << rotationResolution << "  blocking: " << wallsBlockVisonSensors << ")" << endl;
		cout << "    sensor directions: ";
		for (auto d : visionSensorDirections) {
			cout << d << " ";
		}
		cout << endl;
		Sensor newVisionSensor(visionSensorArcSize * -.5, visionSensorArcSize * .5, visionSensorDistance, 1, rotationResolution, wallsBlockVisonSensors);
		visionSensor = newVisionSensor;
	}
	if (smellSensorCount > 0) {
		cout << "  using Smell Sensor:  (arc: " << smellSensorArcSize * -.5 << "," << smellSensorArcSize * .5 << "    distances: " << 1 << "," << smellSensorDistance << "     resolution: " << rotationResolution << "  blocking: " << wallsBlockSmellSensors << ")" << endl;
		cout << "    sensor directions: ";
		for (auto d : smellSensorDirections) {
			cout << d << " ";
		}
		cout << endl;
		Sensor newSmellSensor(smellSensorArcSize * -.5, smellSensorArcSize * .5, smellSensorDistance, 1, rotationResolution, wallsBlockSmellSensors);
		smellSensor = newSmellSensor;
	}

	int perfectSensorCount = 0;
	///// load perfect sensor
	if (usePerfectSensorPL->get(PT) && (rotationResolution == 8 || rotationResolution == 4)) {
		ifstream FILE(perfectSensorFilePL->get(PT));
		if (!FILE.is_open()) {
			cout << "  while trying to load perfect sensor, failed to open file: " << perfectSensorFilePL->get(PT) << "\n  exiting." << endl;
			exit(1);
		}

		auto rawGrid = loadSquareGridOfIntsFromFile(FILE);
		if (rawGrid.x() != rawGrid.x()) {
			cout << "  while trying to load perfect sensor, found non-square definition in file: " << perfectSensorFilePL->get(PT) << "\n  exiting." << endl;
			exit(1);
		}
		int halfSize = rawGrid.x() / 2;
		// convert sensor values so that they start at 0 and load this into perfectSensorSites[0] (where 0 is up)
		perfectSensorSites.push_back({});
		map<char, int> lookup;
		int count = 0;
		for (int x = 0; x < rawGrid.x(); x++) {
			for (int y = 0; y < rawGrid.y(); y++) {
				if (rawGrid(x, y) != '^' && rawGrid(x, y) != '.') {
					if (lookup.find(rawGrid(x, y)) == lookup.end()) {
						lookup[rawGrid(x, y)] = count++;
						perfectSensorSites[0].push_back({});
						perfectSensorCount++;
					}
					perfectSensorSites[0][lookup[rawGrid(x, y)]].push_back(Point2d(x - halfSize, y - halfSize));
				}
			}
		}

		// now preform transformations to create 4 or 8 directions.
		if (rotationResolution == 8) {
			// set perfectSensorSites[1] to rotation 1 (45 degrees right)
			perfectSensorSites.push_back({});
			for (int line = 0; line < perfectSensorSites[0].size(); line++) {
				perfectSensorSites[1].push_back({});
				for (auto site : perfectSensorSites[0][line]) {
					if (site.x == 0) {
						perfectSensorSites[1][line].push_back(Point2d(-1 * site.y, site.y));
					}
					if (site.y == 0) {
						perfectSensorSites[1][line].push_back(Point2d(site.x, site.x));
					}
					if ((site.y < 0 && site.x < 0) || (site.y > 0 && site.x > 0)) {
						if (abs(site.x) <= abs(site.y)) {
							perfectSensorSites[1][line].push_back(Point2d(site.x-site.y, site.y));
						}
						else {
							perfectSensorSites[1][line].push_back(Point2d(site.x-site.y, site.x));
						}
					}
					if ((site.y > 0 && site.x < 0) || (site.y < 0 && site.x > 0)) {
						if (abs(site.x) <= abs(site.y)) {
							perfectSensorSites[1][line].push_back(Point2d(-1 * site.y, site.y + site.x));
						}
						else {
							perfectSensorSites[1][line].push_back(Point2d(site.x, site.y+site.x));
						}
					}
				}
			}
		}
		fillInVision(perfectSensorSites, false);
		cout << "  using Perfect Sensor. Perfect Sensor has " << perfectSensorSites[0].size() << " inputs." << endl;

	}
	else if (usePerfectSensorPL->get(PT)) {
		cout << "  perfect sensor requires directions to be 4 or 8, but directions is set to: " << rotationResolution << "\n  exiting." << endl;
		exit(1);

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  setup move offsets (if agent is facing in a given direction and they move, how much should x and y change by? //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	moveDeltas.resize(rotationResolution);
	for (int i = 0; i < rotationResolution; i++) {
		int tempi = ((rotationResolution - i) + (rotationResolution / 2)) % rotationResolution;
		moveDeltas[i] = { sin(tempi * (360 / (double)rotationResolution) * (3.14159 / 180.0)),
			cos(tempi * (360 / (double)rotationResolution) * (3.14159 / 180.0)) };
		moveDeltas[i].x = ((double)((int)(moveDeltas[i].x * 10000))) / 10000;
		moveDeltas[i].y = ((double)((int)(moveDeltas[i].y * 10000))) / 10000;

		if (AbstractWorld::debugPL->get(PT)) {
			cout << "   " << i << "    move deltas     " << moveDeltas[i].x << "," << moveDeltas[i].y << endl;
			if (visionSensorCount > 0) {
				visionSensor.angles[i]->drawArc();
			}
			if (smellSensorCount > 0) {
				smellSensor.angles[i]->drawArc();
			}
		}
		
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//  LOAD MAPS FROM FILES  ///////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	auto alwaysStartOn = alwaysStartOnPL->get(PT);
	convertCSVListToVector(mapFilesPL->get(PT), mapFiles);

	if (mapFiles.size() == 0) {
		mapFiles.push_back("NONE");
	}
	if (mapFiles[0] != "NONE") {
		cout << "Berry World loading maps..." << endl;
		for (auto mapFile : mapFiles) {
			ifstream FILE(mapFile);
			if (!FILE.is_open()) {
				cout << "  failed to open file: " << mapFile << "\n  exiting." << endl;
				exit(1);
			}
			bool goodRead = false;
			while (!FILE.eof()) {
				WorldMap newMap;
				goodRead = newMap.loadMap(FILE, mapFile);
				if (goodRead) {
					// set up valid locations
					for (int x = 0; x < newMap.worldX; x++) { // for every location in the map (excluding the edge)
						for (int y = 0; y < newMap.worldY; y++) {
							Point2d loc(x, y);
							int valueHere;
							if (newMap.useStartMap) {
								valueHere = newMap.startData(loc) > 0 ? alwaysStartOn : WALL;
							}
							else {
								valueHere = (int)newMap.data(loc);
							}
							if ((alwaysStartOn == -1 && valueHere < WALL) || valueHere == alwaysStartOn) {
								newMap.startLocations.push_back(loc); // if random start locations (-1) or food at this space matches alwaysStartOn, add to validSpaces
								newMap.startFacing.push_back((int)newMap.startData(loc));
							}
							if (valueHere > foodTypes && valueHere != 9 && valueHere != -1) {
								cout << "  file map " << newMap.name << " from file: " << mapFile << " contains foodype not currently in use.\nexiting." << endl;
								exit(1);
							}
						}
					}

					worldMaps[mapFile][newMap.name] = newMap;
					mapNames[mapFile].push_back(newMap.name);
					cout << "    loaded map: " << newMap.name << " from file: " << mapFile << endl;
				}
				else {
					cout << "  While loading map " << newMap.name << " from file: " << mapFile << ", a problem was encountered.\nexiting" << endl;
					exit(1);
				}
			}
			FILE.close();
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//  get map usage //////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	vector<string> temp;
	if (mapFiles[0] != "NONE") {
		convertCSVListToVector(whichMapsPL->get(PT), temp);
		cout << "    found the following whichMaps values:" << endl;
		for (auto v : temp) {
			string delimiter = "/";
			whichMaps.push_back(v.substr(0, v.find(delimiter)));
			whichMaps.push_back(v.substr(v.find(delimiter) + 1, v.size()));
			cout << "      file: " << whichMaps[whichMaps.size() - 2] << "  map: " << whichMaps.back() << endl;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//  set Required inputs and Required outputs ////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	if (foodTypesPL->get(PT) < 1 || foodTypesPL->get(PT) > 8) {
		cout << "In BerryWorld you either have too few or too many foodTypes (must be >0 and <=8)\n\nExiting\n\n";
		exit(1);
	}
	if (alwaysEatPL->get(PT)) {
		requiredOutputs = 2; // number of outputs brain must provide, 2 for move
	}
	else {
		requiredOutputs = 3;  // number of outputs brain must provide, 2 for move, 1 for eat
	}

	requiredInputs = visionSensorCount * ( (foodTypes * seeFoodPL->get(PT) ) + seeOtherPL->get(PT) + seeWallsPL->get(PT));
	requiredInputs += smellSensorCount * ( (foodTypes * smellFoodPL->get(PT) ) + smellOtherPL->get(PT) + smellWallsPL->get(PT));
	requiredInputs += perfectSensorCount * ((foodTypes * perfectDetectsFoodPL->get(PT)) + perfectDetectsOtherPL->get(PT) + perfectDetectsWallsPL->get(PT));
	requiredInputs += useDownSensorPL->get(PT) * foodTypes; // down sensor (just food)
	cout << "  HarvestWorld (with current settings) requires organisms with a brain (" << brainNameSpacePL->get(PT) << ") with " << requiredInputs << " inputs and " << requiredOutputs << " outputs.\n";
	/////////////////////////////////////////////////////////////////////////////////////
	//  set up pop file columns /////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	popFileColumns.clear();
	popFileColumns.push_back("score");
	//popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically by DataMap because _VAR)
	popFileColumns.push_back("switches");
	popFileColumns.push_back("consumptionRatio");
	for (int f = 1; f <= foodTypes; f++) {
		popFileColumns.push_back("food" + to_string(f));
	}
	for (int f = 0; f <= foodTypes; f++) {
		if (poisonRules[f] != 0) {
			popFileColumns.push_back("poison" + to_string(f));
		}
	}
}

void BerryWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	// call runWorld evaluations per generation times
	for (int i = 0; i < evaluationsPerGenerationPL->get(PT); i++) {
		runWorld(groups, analyse, visualize, debug);
	}
}

void BerryWorld::runWorld(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	auto tempPopulation = groups[groupNameSpacePL->get(PT)]->population; // make a copy of the population so we can pull unique organisms when making evaluation groups
	auto populationSize = tempPopulation.size();
	auto groupSize = evaluateGroupSizePL->get(PT);

	int numberOfEvalGroups = ceil(((double)populationSize) / ((double)groupSize));
	vector<vector<shared_ptr<Organism>>> evalGroups;


	// eval groups are generated each with evalGroupSize organisms
	// Some organisms may be evaluated twice (if populaiton is not divisable by evalGroupSize) 

	if (groupSize > populationSize) {
		cout << "In BerryWorld:: groupSize > populationSize! This is now allowed.\n  Please provide a larger population or reduce groupSize.\n  If you want to force clones in eval groups, use clones parameter.\n  exiting." << endl;
		exit(1);
	}
	for (int p = 0; p < numberOfEvalGroups; p++) { // for each eval group
		vector<shared_ptr<Organism>> thisGroup; // create a temporary holder for this
		if (tempPopulation.size() > groupSize) { // if there are enough organisms in tempPopulation for this group
			for (int o = 0; o < groupSize; o++) { // pull evalGroupSize organisms
				auto pick = Random::getIndex(tempPopulation.size()); // get a random index
				thisGroup.push_back(tempPopulation[pick]); // place org at index into thisGroup
				tempPopulation[pick] = tempPopulation.back(); // copy last org in tempPopulation to index location
				tempPopulation.pop_back(); // remove unneeded copy
			}
		}
		else {// tempPopulation.size() is < evalGroupSize, some orgs need to be evaluateded twice (i.e. be in more then one eval group)
			thisGroup = tempPopulation; // place the rest of the orgs in temp population into thisGroup
			tempPopulation = groups[groupNameSpacePL->get(PT)]->population; // refresh tempPopulation
			// fill in any remaining spaces randomly...
			for (int o = thisGroup.size(); o < groupSize; o++) { // fill in the rest of thisGroup
				auto pick = Random::getIndex(tempPopulation.size()); // get a random index
				thisGroup.push_back(tempPopulation[pick]); // place org at index into thisGroup
				tempPopulation[pick] = tempPopulation.back(); // copy last org in tempPopulation to index location
				tempPopulation.pop_back(); // remove unneeded copy
			}
		}
		evalGroups.push_back(thisGroup); // add thisGroup to evalGroups
	} // end create evalGroups for loop

	if (debug) { // display which organisms are in which evaluation group
		cout << "populationSize:" << populationSize << "  groupSize:" << groupSize << "  number of groups:" << numberOfEvalGroups << endl;
		for (int gID = 0; gID < evalGroups.size(); gID++) {
			cout << "group " << gID << " contains: ";
			for (auto o : evalGroups[gID]) {
				cout << o->ID << " ";
			}
			cout << endl;
		}
	}

	// localize some values. This makes using these values easier.
	auto evalTime = evalTimePL->get(PT);
	auto alwaysStartOn = alwaysStartOnPL->get(PT);


	Vector2d<int> foodMap; // what food is here?

	vector<Point2d> validSpaces;
	vector<int> startFacing;

	vector<string> whichMapsActual; // because whichMaps contains wild cards (* and #) whichMapsActual will be filled in once per generation to determin the actual maps for this generation

	if (mapFiles[0] != "NONE" && whichMaps.size() > 0) { // if there are defined maps
		// make a list of file names for each entry in whichMaps
		for (int i = 0; i < whichMaps.size(); i += 2) {
			vector<string> FILENAMES;
			if (whichMaps[i] == "*") { // if the file name provided in a *, then we will be pulling from all files
				FILENAMES = mapFiles;
			}
			else if (isdigit(whichMaps[i][0])) { // if the file name provided in a number, then pick that number of random file names
				int numPicks;
				load_value(whichMaps[i], numPicks);
				int avalibleFiles = mapFiles.size();
				if (numPicks > avalibleFiles) {
					cout << "  In Berry world, while selecting maps number of files requested is > number of map files.\n  exiting." << endl;
					exit(1);
				}
				auto picks = pickUnique(avalibleFiles, numPicks);
				for (auto pick : picks) {
					FILENAMES.push_back(mapFiles[pick]);
				}
			}
			else if (find(mapFiles.begin(), mapFiles.end(), whichMaps[i]) != mapFiles.end()) { // if the file names provided is a name, make sure that name is in file list and then pick form that file
				FILENAMES.push_back(whichMaps[i]);
			}
			else { // FILE ERROR
				cout << "    In Berry world, while selecting maps was not able resolve: " << whichMaps[i] << "/" << whichMaps[i + 1] << ".\n    exiting." << endl;
				exit(1);
			}
			// now that we have the files names for this entry, figure out which maps from those files
			for (auto FILENAME : FILENAMES) {
				if (whichMaps[i + 1] == "*") { // if the map name is * then include all maps from FILENAME
					for (auto mapName : mapNames[FILENAME]) {
						whichMapsActual.push_back(FILENAME);
						whichMapsActual.push_back(mapName);
					}
				}
				else if (isdigit(whichMaps[i + 1][0])) { // if the map name is a # then include this # of maps from FILENAME
					int numPicks;
					load_value(whichMaps[i + 1], numPicks);
					int avalibleMaps = mapNames[FILENAME].size();
					if (numPicks > avalibleMaps) {
						cout << "  In Berry world, while selecting maps from file " << FILENAME << " number of maps requested is > number of maps in file.\n  exiting." << endl;
						exit(1);
					}
					auto picks = pickUnique(avalibleMaps, numPicks);
					for (auto pick : picks) {
						whichMapsActual.push_back(FILENAME);
						whichMapsActual.push_back(mapNames[FILENAME][pick]);
					}
				}
				else if (find(mapNames[FILENAME].begin(), mapNames[FILENAME].end(), whichMaps[i + 1]) != mapNames[FILENAME].end()) { // If the map name is a name, and that name is in FILENAME, include that
					whichMapsActual.push_back(FILENAME);
					whichMapsActual.push_back(whichMaps[i + 1]);
				}
				else { // FILE * ERROR
					cout << "    In Berry world, while selecting maps was not able resolve: " << whichMaps[i] << "/" << whichMaps[i + 1] << ".\n    exiting." << endl;
					exit(1);
				}
			}
		}
	}
	else { // if no maps were defined...
		whichMapsActual = { "NONE","NONE" };
	}


	generators.clear();
	vector<WorldMap::ResourceGenerator> savedGenerators;

	// for each map in whichMapsActual, run evaluation (if NONE, make a random map)
	for (int whichMapIndex = 0; whichMapIndex < (int)whichMapsActual.size(); whichMapIndex += 2) {
		if (debug) {
			cout << "running: " << whichMapsActual[whichMapIndex] << "   " << whichMapsActual[whichMapIndex + 1] << endl;
		}
		if (whichMapsActual[0] != "NONE") {
			worldX = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].worldX;
			worldY = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].worldY;
			foodMap = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].data;
			validSpaces = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].startLocations;
			startFacing = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].startFacing;
			generators = worldMaps[whichMapsActual[whichMapIndex]][whichMapsActual[whichMapIndex + 1]].generators;
			savedGenerators = generators;
		}
		else { // no whichMaps were provided... generate an uninitalize map.
			foodMap.reset(worldX, worldY);
			for (int x = 0; x < worldX; x++) {
				for (int y = 0; y < worldY; y++) {
					if (worldHasWallPL->get(PT) && (x == 0 || y == 0 || x == worldX - 1 || y == worldY - 1)) {
						foodMap(x, y) = WALL; // if this location is on the edge, make it a wall
					}
					else {
						auto pick = initialFoodDistribution[Random::getIndex(initialFoodDistribution.size())];
						if (pick == -1) { // if the value picked from distribution is -1, replace with a random food type
							pick = Random::getInt(1, foodTypes);
						}
						foodMap(x, y) = pick;
					}
				}
			}
			for (int x = 0; x < worldX; x++) { // for every location in the map
				for (int y = 0; y < worldY; y++) {
					Point2d loc(x, y);
					if ((alwaysStartOn == -1 && (int)foodMap(loc) < WALL) || (int)foodMap(loc) == alwaysStartOn) {
						validSpaces.push_back(loc); // if random start locations (-1) or food at this space matches alwaysStartOn, add to validSpaces
						startFacing.push_back(1); // if random start locations (-1) or food at this space matches alwaysStartOn, add 1 (start random facing) to startFacing vector
					}
				}
			}
		}

		if (debug) {
			cout << "inital map:" << endl;
			foodMap.showGrid();
		}

		Vector2d<int> foodLastMap = foodMap; // what food what here before?
		auto foodMapCopy = foodMap;

		// make sure there are enough valid starting locations
		int clones = clonesPL->get(PT);;

		if (groupSize * (clones+1) > validSpaces.size()) {
			cout << "  In BerryWorld: world is too small or has too few alwaysStartOn foods\n  i.e. (WORLD_HARVEST-groupSize * WORLD_HARVEST-clones) > valid starting locations\n  Please correct and try again. Exitting." << endl;
			exit(1);
		}

		int moveOutput, eatOutput;
		string visualizeData;
		// now evaluate each evalGroup
		for (auto evalGroup : evalGroups) {

			foodMap = foodMapCopy;
			foodLastMap = foodMapCopy;

			vector<shared_ptr<Harvester>> harvesters;
			auto tempValidSpaces = validSpaces; // make tempValidSpaces so we can pull elements from it to select unque locations.
			auto tempStartFacing = startFacing;
												// for each org in this group, create a harvester and pick a location and faceing direction
			int IDCount = 0;
			for (auto org : evalGroup) {
				auto newHarvester = make_shared<Harvester>(); // make a new container
				newHarvester->ID = IDCount++;
				newHarvester->cloneID = newHarvester->ID;
				newHarvester->org = org; // provide access to org though harvester
				newHarvester->brain = org->brains[brainNameSpacePL->get(PT)];
				newHarvester->brain->resetBrain();
				// set inital location
				auto pick = Random::getIndex(tempValidSpaces.size()); // get a random index
				newHarvester->loc = tempValidSpaces[pick]; // assign location
				newHarvester->loc.x += .5; // place in center of location
				newHarvester->loc.y += .5;
				tempValidSpaces[pick] = tempValidSpaces.back(); // copy last location in tempValidSpaces to pick location
				tempValidSpaces.pop_back(); // remove last location in tempValidSpaces


				// set inital facing direction
				newHarvester->face = tempStartFacing[pick] == 1 ? Random::getIndex(rotationResolution) : (tempStartFacing[pick] - 2) * (double(rotationResolution) / 8.0); // if startFacing is 1 then pick random, 2 is up, 3 is up right, etc...
				tempStartFacing[pick] = tempStartFacing.back();
				tempStartFacing.pop_back();

				newHarvester->foodCollected.resize(foodTypes + 1);
				newHarvester->poisonTotals.resize(foodTypes + 1);

				// add to harvesters
				harvesters.push_back(newHarvester);

				// place harvester in world
				foodMap(newHarvester->loc) += 10; // set this location to occupied in foodMap - locations in map are 0 if empty, 1->8 if food, 9 if wall, 10 if occupied with no food, 11->18 if occupied with food
				if (debug) {
					cout << "placed ID:" << newHarvester->org->ID << " @ " << newHarvester->loc.x << "," << newHarvester->loc.y << "  " << newHarvester->face << endl;
				}
			} // end create harvesters for loop

			// now add clones to harvesters
			for (int i = 0; i < groupSize; i++) {
				for (int j = 0; j < clones; j++) {
					auto newHarvester = make_shared<Harvester>(); // make a new container
					newHarvester->ID = IDCount++;
					newHarvester->cloneID = harvesters[i]->ID;
					newHarvester->org = harvesters[i]->org; // provide access to org though harvester
					newHarvester->brain = harvesters[i]->brain->makeCopy();
					// set inital location
					auto pick = Random::getIndex(tempValidSpaces.size()); // get a random index
					newHarvester->loc = tempValidSpaces[pick]; // assign location
					newHarvester->loc.x += .5; // place in center of location
					newHarvester->loc.y += .5;
					tempValidSpaces[pick] = tempValidSpaces.back(); // copy last location in tempValidSpaces to pick location
					tempValidSpaces.pop_back(); // remove last location in tempValidSpaces

					// set inital facing direction
					newHarvester->face = tempStartFacing[pick] == 1 ? Random::getIndex(rotationResolution) : (tempStartFacing[pick] - 2) * (double(rotationResolution) / 8.0); // if startFacing is 1 then pick random, 2 is up, 3 is up right, etc...
					tempStartFacing[pick] = tempStartFacing.back();
					tempStartFacing.pop_back();

					newHarvester->foodCollected.resize(foodTypes + 1);
					newHarvester->poisonTotals.resize(foodTypes + 1);

					// add to harvesters
					harvesters.push_back(newHarvester);
					harvesters[i]->clones.push_back(newHarvester);
					// place harvester in world
					foodMap(newHarvester->loc) += 10; // set this location to occupied in foodMap - locations in map are 0 if empty, 1->8 if food, 9 if wall, 10 if occupied with no food, 11->18 if occupied with food
					if (debug) {
						cout << "placed clone of ID:" << newHarvester->org->ID << " @ " << newHarvester->loc.x << "," << newHarvester->loc.y << "  " << newHarvester->face << endl;
					}
				}
			}

			if (visualize) {  // save state inital world and Harvester locations
				visualizeData = "**InitializeWorld**\n";
				visualizeData += to_string(rotationResolution) + "," + to_string(worldX) + "," + to_string(worldY) + "," + to_string(groupSize + (groupSize*clones)) + "\n";
				// save the map
				for (int y = 0; y < worldY; y++) {
					for (int x = 0; x < worldX; x++) {
						visualizeData += to_string(foodMap(x, y) % 10);
						if (x % worldX == worldX - 1) {
							visualizeData += "\n";
						}
						else {
							visualizeData += ",";
						}
					}
				}
				visualizeData += "-\n**InitializeHarvesters**\n";
				for (auto harvester : harvesters) {
					visualizeData += to_string(harvester->ID) + "," + to_string(harvester->loc.x) + "," + to_string(harvester->loc.y) + "," + to_string(harvester->face) + "," + to_string(harvester->cloneID) + "\n";
				}
				visualizeData += "-";
				FileManager::writeToFile("HarvestWorldData.txt", visualizeData);
			}


			// init generatorEvents
			generators.clear();
			for (auto g : savedGenerators) {
				generators.push_back(g);
			}

			generatorEvents.clear();
			for (int i = 0; i < (int)generators.size(); i++) {
				// for each generator, find out next time that generator will fire and add that to generatorEvents
				// generatorEvents[time][generatorIndex]
				generatorEvents[generators[i].nextEvent()].push_back(i);
			}

			// run evaluation
			for (int t = 0; t < evalTime; t++) {
				if (visualize) {
					visualizeData = "U," + to_string(t) + "\n";
				}
				// check to see if there is any inflow
				if(generatorEvents.find(t)!=generatorEvents.end()){ // if there are generator events at this time?
					while (generatorEvents[t].size() > 0) {
						int genIndex = generatorEvents[t].back(); // get id of last generator in list
						generatorEvents[t].pop_back();
						Point2d genLoc = generators[genIndex].getLocation();
						auto replacement = generators[genIndex].getNextResource(foodMap((int)genLoc.x, (int)genLoc.y));
						if (replacement >= 0) {
							foodMap(genLoc) = replacement;
							if (visualize) {
								visualizeData += "I," + to_string((int)genLoc.x) + "," + to_string((int)genLoc.y) + "," + to_string(replacement) + "\n";
							}

						}
						int nextT = generators[genIndex].nextEvent() + t;
						//cout << to_string((int)genLoc.x) << "," << to_string((int)genLoc.y) << "   t  = " << t << "    " << nextT << endl;
						generatorEvents[nextT].push_back(genIndex);
					}
				}

				auto tempHarvesters = harvesters; // make a copy of the current harverster group
				shared_ptr<Harvester> harvester; // this will point to the harvester being updated
				while (tempHarvesters.size() > 0) {
					auto pick = Random::getIndex(tempHarvesters.size()); // get a random index
					harvester = tempHarvesters[pick];
					tempHarvesters[pick] = tempHarvesters.back(); // move last in tempHarvesters to pick location
					tempHarvesters.pop_back(); // remove last harvester in tempHarvesters

					// get a pointer to the brain. if clone, then use harvester local brain
					shared_ptr<AbstractBrain> brain;
					brain = harvester->brain;

					int localTime = 0; // localTime is used to make turns cheaper. If all actions cost 1 world update then turns are discuraged.
									   // in each world update organisms have some number of localTime updates... turns cost one localUpdate, but move costs more.
									   // the number of localTime actions avalible is set by the maxTurn parameter
					while (localTime < (int)((double)rotationResolution /(1.0 /  (double)maxTurn))) {
						/////////////////
						// set the inputs
						/////////////////
						int inputCounter = 0;  // This counter is used while setting brain inputs

						vector<int> sensorValues(19);

						// for each sensor, collect data and set inputs
						int sensorFacing;

						int locX = (int)harvester->loc.x;
						int locY = (int)harvester->loc.y;

						for (int i = 0; i < visionSensorCount; i++) { // set inputs for vision sensors
							sensorFacing = loopMod(harvester->face + visionSensorDirections[i], rotationResolution);
							if (wallsBlockVisonSensors) {
								visionSensor.senseTotals(foodMap, locX, locY, sensorFacing, sensorValues, WALL, true); // load what sensor sees into sensorValues
							}
							else {
								visionSensor.senseTotals(foodMap, locX, locY, sensorFacing, sensorValues, -1, true); // load what sensor sees into sensorValues
							}
							if (seeFoodPL->get(PT)) {
								for (int food = 1; food <= foodTypes; food++) {
									brain->setInput(inputCounter++, sensorValues[food] + sensorValues[food + 10]);
								}
							}
							if (seeOtherPL->get(PT)) {
								int others = 0;
								for (int val = 10; val < 19; val++) {
									others += sensorValues[val];
								}
								brain->setInput(inputCounter++, others); // set occupied
							}
							if (seeWallsPL->get(PT)) {
								brain->setInput(inputCounter++, sensorValues[WALL]); // set wall
							}
						}

						for (int i = 0; i < smellSensorCount; i++) { // set inputs for smell sensors
							sensorFacing = loopMod(harvester->face + smellSensorDirections[i], rotationResolution);
							if (wallsBlockSmellSensors) {
								smellSensor.senseTotals(foodMap, locX, locY, sensorFacing, sensorValues, WALL, true); // load what sensor sees into sensorValues
							}
							else {
								smellSensor.senseTotals(foodMap, locX, locY, sensorFacing, sensorValues, -1, true); // load what sensor sees into sensorValues
							}
							if (smellFoodPL->get(PT)) {
								for (int food = 1; food <= foodTypes; food++) {
									brain->setInput(inputCounter++, sensorValues[food] + sensorValues[food + 10]);
								}
							}
							if (smellOtherPL->get(PT)) {
								int others = 0;
								for (int val = 10; val < 19; val++) {
									others += sensorValues[val];
								}
								brain->setInput(inputCounter++, others); // set occupied
							}
							if (smellWallsPL->get(PT)) {
								brain->setInput(inputCounter++, sensorValues[WALL]); // set wall
							}

						}

						//// uncomment to see perfect sensors
						/*
						for (auto sensor : perfectSensorSites) {
							Vector2d<int> test(11, 11, 0);
							int lineCount = 1;
							for (auto line : sensor) {
								for (auto p : line) {
									test(p.x+5, p.y+5) = lineCount;
								}
								lineCount++;
							}
							test.showGrid();
							cout << endl;
						}
						exit(1);
						*/

						// set inputs for perfect Sensor
						if (usePerfectSensorPL->get(PT)) {
							for (auto line : perfectSensorSites[harvester->face]) { // for all the lines (senson inputs) in the perfect sensor for the current dirrection
								fill(sensorValues.begin(), sensorValues.end(), 0);
								for (auto p : line) { // for each location in the current line (sensor input) get the information at that location
									sensorValues[foodMap(loopMod(locX + p.x, worldX), loopMod(locY + p.y, worldY))]++;
								}
								if (perfectDetectsFoodPL->get(PT)) { // for each type of food, set a brain input
									for (int food = 1; food <= foodTypes; food++) {
										brain->setInput(inputCounter++, sensorValues[food] + sensorValues[food + 10]);
									}
								}
								if (perfectDetectsOtherPL->get(PT)) {
									int others = 0;
									for (int val = 10; val < 19; val++) {
										others += sensorValues[val];
									}
									brain->setInput(inputCounter++, others); // set occupied
								}
								if (perfectDetectsWallsPL->get(PT)) {
									brain->setInput(inputCounter++, sensorValues[WALL]); // set wall
								}
							}
						}

						// set inputs for down sensor
						if (useDownSensorPL->get(PT)) {
							for (int food = 1; food <= foodTypes; food++) {
								brain->setInput(inputCounter++, foodMap(harvester->loc) == (food + 10));
							}
						}

						if (debug) {
							cout << "\n----------------------------\n";
							cout << "\ngeneration update: " << Global::update << "  world update: " << t << "  local time: " << localTime << "\n";
							cout << "currentLocation: " << harvester->loc.x << "," << harvester->loc.y << "  :  " << harvester->face << "\n";
							cout << "inNodes: ";
							for (int i = 0; i < requiredInputs; i++) {
								cout << brain->readInput(i) << " ";
							}
							cout << "\nlast outNodes: ";
							for (int i = 0; i < requiredOutputs; i++) {
								cout << brain->readOutput(i) << " ";
							}
							cout << endl << endl;
							foodMap.showGrid();

							cout << "\n\n  -- brain update --\n\n";
						}

						/////////////////
						// update the brain
						/////////////////

						brain->update();

						/////////////////
						// read the outputs
						/////////////////
						// moveOutput has info about the first 2 output bits these [00 = 0 = no action, 10 = 2 = left, 01 = 1 = right, 11 = 3 = move]
						moveOutput = Bit(brain->readOutput(1)) + (Bit(brain->readOutput(0)) * 2);
						// eatOutput has info about the 3rd output bit (if !alwaysEat), which either does nothing, or causes an eat.
						if (alwaysEat) {
							eatOutput = 1;
						}
						else {
							eatOutput = Bit(brain->readOutput(2));
						}

						////////////////////////
						// update world --- food
						////////////////////////

						if (eatOutput) { // attempt to eat what's here
							localTime += rotationResolution;
							auto currentLoc = harvester->loc;
							Point2d currentSpace((int)currentLoc.x, (int)currentLoc.y);
							for (int f = 1; f <= foodTypes; f++) {
								if (foodMap(currentSpace) - 10 == f) { // there is a food here (subtract 10 because harvester is here)
									if (harvester->lastFoodCollected != f && harvester->lastFoodCollected != 0) { // if last food was 0, this is the first food collected, so no switch
										harvester->switches++;
									}
									harvester->lastFoodCollected = f;
									harvester->foodCollected[f]++;
									foodMap(currentSpace) = 10; // set map to occupied with no food
									if (visualize) {
										visualizeData += "E," + to_string((int)currentSpace.x) + "," + to_string((int)currentSpace.y) + "\n";
									}

								}
							}
						}

						///////////////////////////
						// update world --- turning
						///////////////////////////

						// see if harvester is turning
						if (!eatOutput || alwaysEat) { // if harvester did not eat or alwaysEat is set, then harvester may move
							if (moveOutput == 0) { // do nothing
								localTime += rotationResolution;;
							}
							if (moveOutput == 1) { // turn right
								localTime++;
								harvester->face = loopMod(harvester->face + 1, rotationResolution);
								//cout << "turned ID:" << harvester->org->ID << " @ " << harvester->loc.x << "," << harvester->loc.y << "  " << harvester->face << endl;
								if (visualize) {
									visualizeData += "TR," + to_string(harvester->ID) + "," + to_string(harvester->face) + "\n";
								}
							}
							if (moveOutput == 2) { // turn left
								localTime++;
								harvester->face = loopMod(harvester->face - 1, rotationResolution);
								//cout << "turned ID:" << harvester->org->ID << " @ " << harvester->loc.x << "," << harvester->loc.y << "  " << harvester->face << endl;
								if (visualize) {
									visualizeData += "TL," + to_string(harvester->ID) + "," + to_string(harvester->face) + "\n";
								}
							}
						}

						///////////////////////////
						// update world --- moving
						///////////////////////////
						// see if harvester is moving
						double moveDistance = (moveOutput == 3) ? moveDefault : moveMin;
						if ((!eatOutput || alwaysEat) && moveDistance > 0) {
							if (moveOutput == 3) {
								localTime += rotationResolution; // we are done with this localTime!
							}
							auto currentLoc = harvester->loc; // where are we now?
							Point2d currentSpace((int)currentLoc.x, (int)currentLoc.y); // which grid space is this?
							auto targetLoc = moveOnGrid(harvester, moveDistance); // where are we going (if we move)?
							Point2d targetSpace((int)targetLoc.x, (int)targetLoc.y); // which space will we move into (if we move?)
							if (currentSpace == targetSpace) { // if this mode does not change space, then just make the move
								if (snapToGrid) {
									harvester->loc.x = targetSpace.x + .5;
									harvester->loc.y = targetSpace.y + .5;
								}
								else {
									harvester->loc = targetLoc; // if harvester is not moving from current space, just update location
								}
								if (visualize) {
									visualizeData += "M," + to_string(harvester->ID) + "," + to_string(harvester->loc.x) + "," + to_string(harvester->loc.y) + "\n";
								}
							}
							else { // this move would change world location
								if (foodMap(targetLoc) < 9) { // if the proposed move is not a wall (9) and is not occupied by another org (>9)
									if (foodMap(currentSpace) - 10 != foodLastMap(currentSpace)) { // if the food here changed, repacment needed.
										auto newFoodPick = Random::getIndex(replaceRules[foodLastMap(currentSpace)].size()); // get new food
										auto newFood = replaceRules[foodLastMap(currentSpace)][newFoodPick] == -1 ? Random::getInt(1,foodTypes): replaceRules[foodLastMap(currentSpace)][newFoodPick];
										foodMap(currentSpace) = newFood;
										foodLastMap(currentSpace) = newFood;
										if (visualize) {
											visualizeData += "R," + to_string((int)currentSpace.x) + "," + to_string((int)currentSpace.y) + "," + to_string(newFood) + "\n";
										}
									}
									else {
										foodMap(currentSpace) -= 10; // food did not change and harvester is no longer here
									}

									if (snapToGrid) { // now move
										harvester->loc.x = targetSpace.x + .5;
										harvester->loc.y = targetSpace.y + .5;
									}
									else {
										harvester->loc = targetLoc; // if harvester is not moving from current space, just update location
									}

									if (visualize) {
										visualizeData += "M," + to_string(harvester->ID) + "," + to_string(harvester->loc.x) + "," + to_string(harvester->loc.y) + "\n";
									}
									harvester->poisonTotals[foodMap(targetSpace)]++; // update poision totals - later we will subtract poison if this food is in fact poison
									foodMap(targetSpace) += 10; // there is a harvester here, so add 10 to the current location value
									//cout << "moved ID:" << harvester->org->ID << " @ " << harvester->loc.x << "," << harvester->loc.y << "  " << harvester->face << endl;
								}
								else {
									// move is blocked... was it blocked by wall or other?
									if (foodMap(targetLoc) == 9) { // blocked by wall
										harvester->wallHits++;
									}
									else { // blocked by other
										harvester->otherHits++;
									}
									// move is blocked (by wall or other), no action needed.
								}
							} // end this move would change world location else statement
						} // end movement
					} // end localTime while loop
				} // end evaluate this harvester
				  // save map to visualize file
				if (visualize) {  // save state of world before we get started.
					FileManager::writeToFile("HarvestWorldData.txt", visualizeData);
				}

			} // end this evalGroup

			// now save data based on cloneScoreRule where: 0 = all, 1 = best, 2 = worst.
			// first score all harvesters
			for (auto harvester : harvesters) {
				// here we will do some math and save values to each harvesters dataMap
				for (int f = 1; f <= foodTypes; f++) {
					harvester->maxFood = max(harvester->foodCollected[f], harvester->maxFood);
					harvester->totalFood += harvester->foodCollected[f];
					harvester->foodScore += foodRewards[f] * harvester->foodCollected[f];
					if (poisonRules[f] != 0) {
						harvester->org->dataMap.append("poison" + to_string(f), harvester->poisonTotals[f]);
					}
				}

				harvester->score = 
					harvester->foodScore -
					((harvester->switches * switchCostPL->get(PT)) + 
					harvester->poisonCost +
					(harvester->wallHits * hitWallCostPL->get()) +
					(harvester->otherHits * hitOtherCostPL->get()));
			}
			 // for each organisms, figure out which clone (or clones to save)
			vector<shared_ptr<Harvester>> saveHarvesters; // this will be a list of harvesters which we save data for

			if (cloneScoreRule == 0) {
				saveHarvesters = harvesters;
			}
			else if (cloneScoreRule == 1) {
				for (int i = 0; i < groupSize; i++){ // for each primary clone
					int bestIndex = i;
					for (auto clone : harvesters[i]->clones) {
						if (clone->score > harvesters[bestIndex]->score) {
							bestIndex = clone->ID;
						}
					}
					saveHarvesters.push_back(harvesters[bestIndex]);
				}
			}
			else {
				for (int i = 0; i < groupSize; i++) { // for each primary clone
					int worstIndex = i;
					for (auto clone : harvesters[i]->clones) {
						if (clone->score < harvesters[worstIndex]->score) {
							worstIndex = clone->ID;
						}
					}
					saveHarvesters.push_back(harvesters[worstIndex]);
				}
			} // end select saveHarvesters

			// now save data to dataMaps for everytone in saveHarvesters
			for (auto harvester : saveHarvesters) {
				for (int f = 1; f <= foodTypes; f++) {
					harvester->org->dataMap.append("food" + to_string(f), harvester->foodCollected[f]);
					if (poisonRules[f] != 0) {
						harvester->org->dataMap.append("poison" + to_string(f), harvester->poisonTotals[f]);
					}
				}
				harvester->org->dataMap.append("switches", harvester->switches);
				harvester->org->dataMap.append("consumptionRatio", harvester->maxFood / (harvester->totalFood - harvester->maxFood + 1));
				harvester->org->dataMap.append("wallHits", harvester->wallHits);
				harvester->org->dataMap.append("otherHits", harvester->otherHits);
				harvester->org->dataMap.append("score", harvester->score);
			}

		} // end evaluations all evalGroups
	} // end current map
} // end HarvestWorld::evaluate

/*
harvester->org->dataMap.append("switches", harvester->switches);
harvester->org->dataMap.append("consumptionRatio", harvester->maxFood / (harvester->totalFood - harvester->maxFood + 1));
harvester->org->dataMap.append("wallHits", harvester->wallHits);
harvester->org->dataMap.append("otherHits", harvester->otherHits);
*/
unordered_map<string, unordered_set<string>> BerryWorld::requiredGroups() {
	return { 
		{ groupNameSpacePL->get(PT), { "B:" + brainNameSpacePL->get(PT) + "," + to_string(requiredInputs) + "," + to_string(requiredOutputs) } }
	};
}


