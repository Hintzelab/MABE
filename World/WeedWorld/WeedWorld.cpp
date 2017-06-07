///  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "WeedWorld.h"

shared_ptr<ParameterLink<int>> WeedWorld::numGrowthRegimesPL = Parameters::register_parameter("WORLD_WEED-numGrowthRegimes", 4, "number of growth regime a plant can average");
shared_ptr<ParameterLink<int>> WeedWorld::advanceDistancePL = Parameters::register_parameter("WORLD_WEED-advanceDistance", 100, "How much handler is advanced to read in genetic markers");

shared_ptr<ParameterLink<int>> WeedWorld::contributionFactorPL = Parameters::register_parameter("WORLD_WEED-contributionFactor", 3, "Multiplier of food that is given to children");
shared_ptr<ParameterLink<int>> WeedWorld::germinationCostPL = Parameters::register_parameter("WORLD_WEED-germinationCost", 7, "How much food a seed loses when it is planted");

shared_ptr<ParameterLink<int>> WeedWorld::worldSizePL = Parameters::register_parameter("WORLD_WEED-worldSize", 200, "size of world");
shared_ptr<ParameterLink<int>> WeedWorld::worldGridSizePL = Parameters::register_parameter("WORLD_WEED-worldGridSize", 50, "size of one world grid (weeds can only interact with neighbors at most one grid away");

shared_ptr<ParameterLink<double>> WeedWorld::leafWitherFactorPL = Parameters::register_parameter("WORLD_WEED-leafWitherFactor", 0.5, "percentage leaves wither with the plants run out of water");

shared_ptr<ParameterLink<double>> WeedWorld::rootSpreadMaxPL = Parameters::register_parameter("WORLD_WEED-rootSpreadMax", -1.0, "how large can rootSpread grow. must be < worldGridSize/2. if -1 then ((worldGridSize/2)-1)");
shared_ptr<ParameterLink<double>> WeedWorld::rootGirthMaxPL = Parameters::register_parameter("WORLD_WEED-rootGirthMax", 10.0, "how large can rootGirth grow");
shared_ptr<ParameterLink<double>> WeedWorld::leafCoverageMaxPL = Parameters::register_parameter("WORLD_WEED-leafCoverageMax", 10.0, "how large can leafCoverage grow");
shared_ptr<ParameterLink<double>> WeedWorld::stalkHeightMaxPL = Parameters::register_parameter("WORLD_WEED-stalkHeightMax", 50.0, "how large can stalkHeight grow");

shared_ptr<ParameterLink<double>> WeedWorld::rootSpreadUpkeepPL = Parameters::register_parameter("WORLD_WEED-rootSpreadUpkeep", 1.0, "cost to maintain rootSpread");
shared_ptr<ParameterLink<double>> WeedWorld::rootGirthUpkeepPL = Parameters::register_parameter("WORLD_WEED-rootGirthUpkeep", 1.0, "cost to maintain rootGirth");
shared_ptr<ParameterLink<double>> WeedWorld::leafCoverageUpkeepPL = Parameters::register_parameter("WORLD_WEED-leafCoverageUpkeep", 1.0, "cost to maintain leafCoverage");
shared_ptr<ParameterLink<double>> WeedWorld::stalkHeightUpkeepPL = Parameters::register_parameter("WORLD_WEED-stalkHeightUpkeep", 1.0, "cost to maintain stalkHeight");

shared_ptr<ParameterLink<double>> WeedWorld::rootSpreadWaterUpkeepPL = Parameters::register_parameter("WORLD_WEED-rootSpreadWaterUpkeep", 0.0, "water cost to maintain rootSpread default should be 0");
shared_ptr<ParameterLink<double>> WeedWorld::rootGirthWaterUpkeepPL = Parameters::register_parameter("WORLD_WEED-rootGirthWaterUpkeep", 0.0, "cost to maintain rootGirth default should be 0");
shared_ptr<ParameterLink<double>> WeedWorld::stalkHeightWaterUpkeepPL = Parameters::register_parameter("WORLD_WEED-stalkHeightWaterUpkeep", 0.0, "cost to maintain stalkHeight default should be 0");
shared_ptr<ParameterLink<double>> WeedWorld::leafCoverageWaterUpkeepPL = Parameters::register_parameter("WORLD_WEED-leafCoverageWaterUpkeep", 0.1, "cost to maintain leafCoverage. This one is the one that's different");

shared_ptr<ParameterLink<double>> WeedWorld::offspringMinDistancePL = Parameters::register_parameter("WORLD_WEED-offspringMinDistance", 1.0, "ratio of stalk height for minimum offspring distance\nif 0, offspring will be anywhere between 0 and stalkHeight\nif 1, offspring will be exactly at stalkHeight");

shared_ptr<ParameterLink<double>> WeedWorld::intialFoodPL = Parameters::register_parameter("WORLD_WEED-intialFood", 100.0, "weeds in the initial population start with this much food");

shared_ptr<ParameterLink<int>> WeedWorld::lifeTimeMinPL = Parameters::register_parameter("WORLD_WEED-lifeTimeMin", 90, "low range for weed life time (may die sooner from starvation)");
shared_ptr<ParameterLink<int>> WeedWorld::lifeTimeMaxPL = Parameters::register_parameter("WORLD_WEED-lifeTimeMax", 110, "high range for weed life time");

shared_ptr<ParameterLink<bool>> WeedWorld::sexPL = Parameters::register_parameter("WORLD_WEED-sex", false, "if true, weeds will reproduce with sex");
shared_ptr<ParameterLink<bool>> WeedWorld::sexWellMixedPL = Parameters::register_parameter("WORLD_WEED-sexWellMixed", false, "if true, a random mate will be selected from the populaiton,\notherwise, 'mate' will be picked from weeds within worldGridSize, and closer weeds will be prefered");
shared_ptr<ParameterLink<bool>> WeedWorld::sexEffectedByStalkHeightPL = Parameters::register_parameter("WORLD_WEED-sexEffectedByStalkHeight", true, "if true, plants will higher stalks will have an advantage");

shared_ptr<ParameterLink<bool>> WeedWorld::saveLocationsPL = Parameters::register_parameter("WORLD_WEED-saveLocations", false, "if true, save weedLocations.txt");
shared_ptr<ParameterLink<int>> WeedWorld::saveLocationsStepPL = Parameters::register_parameter("WORLD_WEED-saveLocationsStep", 10, "if saveLocations = true, save when update % saveLocationsStep == 0");

shared_ptr<ParameterLink<bool>> WeedWorld::saveWorldDataPL = Parameters::register_parameter("WORLD_WEED-saveWorldData", true, "if true, save saveWorldData.csv");
shared_ptr<ParameterLink<int>> WeedWorld::saveWorldDataStepPL = Parameters::register_parameter("WORLD_WEED-saveWorldDataStep", 10, "if saveWorldData = true, save when update % saveWorldDataStep == 0");

shared_ptr<ParameterLink<string>> WeedWorld::mapFileListPL = Parameters::register_parameter("WORLD_WEED-mapFileList", (string) "[]", "list of worlds in which to evaluate organism. If empty, random world will be created");

shared_ptr<ParameterLink<double>> WeedWorld::sexSelfPercentPL = Parameters::register_parameter("WORLD_WEED-sexSelfPercent", 0.0, "if reproduction is by sex, percent chance to self, if negitive, % derived from genome");
shared_ptr<ParameterLink<bool>> WeedWorld::sexSelfIfNoMatePL = Parameters::register_parameter("WORLD_WEED-sexSelfIfNoMate", true, "if reproduction is by sex and no mate is not found, parent will self");


shared_ptr<ParameterLink<string>> WeedWorld::groupNamePL = Parameters::register_parameter("WORLD_WEED_NAMES-groupName", (string)"root", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> WeedWorld::brainNamePL = Parameters::register_parameter("WORLD_WEED_NAMES-brainName", (string)"root", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");


bool WeedWorld::WorldMap::loadMap(ifstream& FILE, const string _fileName, shared_ptr<ParametersTable> parentPT, Vector2d<double> grid, int worldSizeCourse) {
	fileName = _fileName;
	string parameterName, parameterValue;
	grid.reset(worldSizeCourse, worldSizeCourse);
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
				grid(sizeX, sizeY) = double(charBuffer);
				sizeX++; // set X, this X will be used for the rest of the map.
				ss >> charBuffer;
			}
			while (!atEOF && !done) {
				atEOF = loadLineToSS(FILE, rawLine, ss);
				ss >> charBuffer;
				if (charBuffer == '*') { // if the first char on the line is "-" then we are at the end of the mapx
					done = true;
				}
				else {
					sizeY++; // not yet at end of map, add a row
					for (int c = 0; c < sizeX; c++) {
						grid(c, sizeY) = double(charBuffer);
						ss >> charBuffer;
					}
				}
			}
			//PT->setParameter("WORLD_BERRY-worldX", sizeX);
			//PT->setParameter("WORLD_BERRY-worldY", sizeY);
		}
	}
	return done;
}

void WeedWorld::OpenMaps(vector<string> mapFileList, map<string, map<string, WorldMap>> worldMaps, Vector2d<double> grid) {

	if (mapFileList.size() > 0) {
		cout << "WeedWorld loading maps..." << endl;

		for (auto fileName : mapFileList) {
			worldMaps[fileName] = map<string, WorldMap>();
			ifstream FILE(fileName);
			bool goodRead = false;
			while (!FILE.eof()) {
				WorldMap newMap;
				goodRead = newMap.loadMap(FILE, fileName, PT, grid, worldSizeCourse);
				if (goodRead) {
					cout << "    \"" << newMap.mapName << "\" from file \"" << fileName << "\"" << endl;
					if (worldMaps[fileName].find(newMap.mapName) == worldMaps[fileName].end()) {
						worldMaps[fileName].insert({ newMap.mapName, newMap });
					}
					else {
						cout << "  ...while reading world maps for weed world, found multiple maps with the same name. Duplicate name \"" << newMap.fileName + "__" + newMap.mapName << "\" found in file \"" << fileName << "\".\n  Exiting." << endl;
						exit(1);
					}
				}
			}
			FILE.close();
		}
	}

}


WeedWorld::WeedWorld(shared_ptr<ParametersTable> _PT) :
	AbstractWorld(_PT) {
	//WeedWorld::WeedWorld(shared_ptr<ParametersTable> _PT) {
	numGrowthRegimes = (PT == nullptr) ? numGrowthRegimesPL->lookup() : PT->lookupInt("WORLD_WEED-numGrowthRegimes");
	advanceDistance = (PT == nullptr) ? advanceDistancePL->lookup() : PT->lookupInt("WORLD_WEED-advanceDistance");

	contributionFactor = (PT == nullptr) ? contributionFactorPL->lookup() : PT->lookupInt("WORLD_WEED-contributionFactor");
	germinationCost = (PT == nullptr) ? germinationCostPL->lookup() : PT->lookupInt("WORLD_WEED-germinationCost");

	worldSize = (PT == nullptr) ? worldSizePL->lookup() : PT->lookupInt("WORLD_WEED-worldSize");
	worldGridSize = (PT == nullptr) ? worldGridSizePL->lookup() : PT->lookupInt("WORLD_WEED-worldGridSize");

	worldSizeCourse = worldSize / worldGridSize;
	influenceRadiusMax = worldGridSize / 2;

	rootSpreadMax = (PT == nullptr) ? rootSpreadMaxPL->lookup() : PT->lookupDouble("WORLD_WEED-rootSpreadMax");
	if (rootSpreadMax == -1) {
		rootSpreadMax = influenceRadiusMax - 1;
	}
	if (rootSpreadMax >= influenceRadiusMax) {
		cout << "In WeedWorld :: rootSpreadMax: " << rootSpreadMax << " > worldGridSize / 2: " << worldGridSize / 2 << "\n  Please correct this (or set rootSpreadMax = -1).\n  Exiting." << endl;
		exit(1);
	}
	rootGirthMax = (PT == nullptr) ? rootGirthMaxPL->lookup() : PT->lookupDouble("WORLD_WEED-rootGirthMax");
	leafCoverageMax = (PT == nullptr) ? leafCoverageMaxPL->lookup() : PT->lookupDouble("WORLD_WEED-leafCoverageMax");
	stalkHeightMax = (PT == nullptr) ? stalkHeightMaxPL->lookup() : PT->lookupDouble("WORLD_WEED-stalkHeightMax");

	rootSpreadUpkeep = (PT == nullptr) ? rootSpreadUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-rootSpreadUpkeep");
	rootGirthUpkeep = (PT == nullptr) ? rootGirthUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-rootGirthUpkeep");
	leafCoverageUpkeep = (PT == nullptr) ? leafCoverageUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-leafCoverageUpkeep");
	stalkHeightUpkeep = (PT == nullptr) ? stalkHeightUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-stalkHeightUpkeep");

	rootSpreadWaterUpkeep = (PT == nullptr) ? rootSpreadWaterUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-rootSpreadWaterUpkeep");
	rootGirthWaterUpkeep = (PT == nullptr) ? rootGirthWaterUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-rootGirthWaterUpkeep");
	leafCoverageWaterUpkeep = (PT == nullptr) ? leafCoverageWaterUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-leafCoverageWaterUpkeep");
	stalkHeightWaterUpkeep = (PT == nullptr) ? stalkHeightWaterUpkeepPL->lookup() : PT->lookupDouble("WORLD_WEED-stalkHeightWaterUpkeep");

	leafWitherFactor = (PT == nullptr) ? leafWitherFactorPL->lookup() : PT->lookupDouble("WORLD_WEED-leafWitherFactor");

	offspringMinDistance = (PT == nullptr) ? offspringMinDistancePL->lookup() : PT->lookupDouble("WORLD_WEED-offspringMinDistance");

	intialFood = (PT == nullptr) ? intialFoodPL->lookup() : PT->lookupDouble("WORLD_WEED-intialFood");

	lifeTimeMin = (PT == nullptr) ? lifeTimeMinPL->lookup() : PT->lookupInt("WORLD_WEED-lifeTimeMin");
	lifeTimeMax = (PT == nullptr) ? lifeTimeMaxPL->lookup() : PT->lookupInt("WORLD_WEED-lifeTimeMax");

	sex = (PT == nullptr) ? sexPL->lookup() : PT->lookupBool("WORLD_WEED-sex");
	sexWellMixed = (PT == nullptr) ? sexWellMixedPL->lookup() : PT->lookupBool("WORLD_WEED-sexWellMixed");
	sexEffectedByStalkHeight = (PT == nullptr) ? sexEffectedByStalkHeightPL->lookup() : PT->lookupBool("WORLD_WEED-sexEffectedByStalkHeight");

	saveLocations = (PT == nullptr) ? saveLocationsPL->lookup() : PT->lookupBool("WORLD_WEED-saveLocations");
	saveLocationsStep = (PT == nullptr) ? saveLocationsStepPL->lookup() : PT->lookupInt("WORLD_WEED-saveLocationsStep");
	saveWorldData = (PT == nullptr) ? saveWorldDataPL->lookup() : PT->lookupBool("WORLD_WEED-saveWorldData");
	saveWorldDataStep = (PT == nullptr) ? saveWorldDataStepPL->lookup() : PT->lookupInt("WORLD_WEED-saveWorldDataStep");

	sexSelfPercent = (PT == nullptr) ? sexSelfPercentPL->lookup() : PT->lookupDouble("WORLD_WEED-sexSelfPercent");
	sexSelfIfNoMate = (PT == nullptr) ? sexSelfIfNoMatePL->lookup() : PT->lookupBool("WORLD_WEED-sexSelfIfNoMate");

	//cout << "sexSelfPrecent: " << sexSelfPercent << endl;

	//OpenMaps(mapFileList, worldMaps, foodMap);
	//OpenMaps(mapFileList, worldMaps, waterMap);
	//OpenMaps(mapFileList, worldMaps, foodOut);
	//OpenMaps(mapFileList, worldMaps, waterOut);

	foodMap.reset(worldSizeCourse, worldSizeCourse);
	foodOut.reset(worldSizeCourse, worldSizeCourse);
	foodIn.reset(worldSizeCourse, worldSizeCourse);
	foodMax.reset(worldSizeCourse, worldSizeCourse);
	waterMap.reset(worldSizeCourse, worldSizeCourse);
	waterOut.reset(worldSizeCourse, worldSizeCourse);
	waterIn.reset(worldSizeCourse, worldSizeCourse);
	waterMax.reset(worldSizeCourse, worldSizeCourse);

	// TODO This just adds food to the left top quarter of the grid

	for (int x = 0; x < worldSizeCourse; x++)
	{
		for (int y = 0; y < worldSizeCourse; y++)
		{
			//foodMap(x, y) = (((double)x + (double)y) / ((double)worldSizeCourse * (double)worldSizeCourse)) * 10;
			foodMap(x, y) = ((double)y / (double)worldSizeCourse) * 10.0;
			foodIn(x, y) = ((double)y / (double)worldSizeCourse) * 15.0;
			foodMax(x, y) = ((double)y / (double)worldSizeCourse) * 300.0;

			waterMap(x, y) = ((double)x / (double)worldSizeCourse) * 10.0;
			waterIn(x, y) = ((double)x / (double)worldSizeCourse) * 15.0;
			waterMax(x, y) = ((double)x / (double)worldSizeCourse) * 300.0;
		}
	}

	//TODO this just adds water to the left top corner of the map

	weedIDMap.reset(worldSizeCourse, worldSizeCourse);

	turnTable.setup(360, 360); // 360 degrees, one slice per degree

	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_WEED_NAMES-groupName");
	brainName = (PT == nullptr) ? brainNamePL->lookup() : PT->lookupString("WORLD_WEED_NAMES-brainName");

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");

	popFileColumns.push_back("food");
	popFileColumns.push_back("water");
	popFileColumns.push_back("light");
	popFileColumns.push_back("energy");
	popFileColumns.push_back("numOffspring");
	popFileColumns.push_back("produceFailed");
	popFileColumns.push_back("stalkHeight");
	popFileColumns.push_back("rootGirth");
	popFileColumns.push_back("rootSpread");
	popFileColumns.push_back("leafCoverage");
	popFileColumns.push_back("stalkHeightInvestment");
	popFileColumns.push_back("rootGirthInvestment");
	popFileColumns.push_back("rootSpreadInvestment");
	popFileColumns.push_back("leafCoverageInvestment");
	popFileColumns.push_back("x");
	popFileColumns.push_back("y");

}


void WeedWorld::integrateWeed(int ID, shared_ptr<Weed> weed) {

	// evaluate distance between org and all orgs in 3x3 coarse neighborhood
	// if distance <= rootSpreadMax * 2, record distance and add to neighbor list
	// after this, all weeds will have a map of org ID -> distance, and org ID -> weed
	// this will be used to determine, food uptake, water uptake, and to find mates (note, it
	// may be necessary to add this same system for mates, but we can deal with that later...
	double dist;
	for (int row = -1; row < 2; row++) {
		for (int column = -1; column < 2; column++) {
			for (auto neighborID : weedIDMap(weed->coarseLocation.x + row, weed->coarseLocation.y + column)) {
				dist = weed->location.dist(weeds[neighborID]->location);

				if (dist <= (influenceRadiusMax * 2)) {
					// add distance between org and every org in this section of the weedIDMap
					////cout << " adding distance for ID: " << ID << " and neighborID: " << neighborID << "  dist: " << dist << endl;
					////cout << "   ";
					////weed->location.show();
					////cout << "   ";
					////weeds[neighborID]->location.show();

					weed->neighborsDistanceList[neighborID] = dist;
					// add a link to each org in this section of the weedIDMap
					weed->neighborsList[neighborID] = weeds[neighborID];

					// add same data to target 
					weeds[neighborID]->neighborsDistanceList[ID] = dist;
					weeds[neighborID]->neighborsList[ID] = weed;
				}
			}
		}
	}

	// TODO Deal with edges?
	double normalLocX = floor((weed->location.x + worldGridSize) / worldGridSize) - 1;
	double normalLocY = floor((weed->location.y + worldGridSize) / worldGridSize) - 1;
	weed->nearestGridPoints.push_back(Point2d(normalLocX, normalLocY));
	weed->nearestGridPoints.push_back(Point2d(normalLocX, normalLocY - 1));
	weed->nearestGridPoints.push_back(Point2d(normalLocX - 1, normalLocY));
	weed->nearestGridPoints.push_back(Point2d(normalLocX - 1, normalLocY - 1));

	double totalDist = 0.0;

	vector<double> pointDist;

	for (auto gridLoc : weed->nearestGridPoints)
	{
		auto x2x1 = (weed->location.x - gridLoc.x * worldGridSize + worldGridSize / 2);
		auto y2y1 = (weed->location.y - gridLoc.y * worldGridSize + worldGridSize / 2);

		totalDist += 1 / sqrt(x2x1 * x2x1 + y2y1 * y2y1);
		pointDist.push_back(1 / sqrt(x2x1 * x2x1 + y2y1 * y2y1));
	}

	for (auto dist : pointDist)
	{
		weed->gridPercents.push_back(dist / totalDist);
	}

	// now, add this weed to the weedIDMap (so that later weeds will see it)
	weedIDMap(int(weed->coarseLocation.x), int(weed->coarseLocation.y)).insert(ID);
	weeds[ID] = weed;


}

void WeedWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
	vector<shared_ptr<Organism>> population = groups[groupName]->population;
	vector<shared_ptr<Organism>> nextPopulation;
	int popSize = population.size();

	// some population level data collectors

	int deaths;
	int newWeeds;
	int newWeedsFail;

	// set up initial population
	Point2d newLocation, newCoarseLocation;
	double foodDelta;
	double waterDelta;

	double minX = worldSize;
	double minY = worldSize;
	double maxX = 0;
	double maxY = 0;

	// for each org in the origanl population...
	for (auto org : population) {

		// create a random location
		newLocation.set(Random::getDouble(worldGridSize + 1, worldSize - (worldGridSize + 1)), Random::getDouble(worldGridSize + 1, worldSize - (worldGridSize + 1)));

		// convert new location to a coarse location
		newCoarseLocation.set((int)(newLocation.x / (double)worldGridSize), (int)(newLocation.y / (double)worldGridSize));

		// make new weed
		shared_ptr<Weed> newWeed = make_shared<Weed>(org, newLocation, newCoarseLocation, intialFood, Global::update + Random::getInt(lifeTimeMin, lifeTimeMax), numGrowthRegimes, advanceDistance);

		// connect this weed to other close by weeds (used to determine food, water and mates)
		newWeed->colorR = Random::getDouble(1.0);
		newWeed->colorG = Random::getDouble(1.0);
		newWeed->colorB = Random::getDouble(1.0);
		integrateWeed(org->ID, newWeed);
	}


	while (!groups[groupName]->archivist->finished) {

		DataMap worldData;

		deaths = 0;
		newWeeds = 0;
		newWeedsFail = 0;

		// collect food and age
		for (auto weed : weeds) {
			worldData.Append("age", weed.second->age++);
			// determine amout of food this plant will get (assuming no neighbors)
			double foodDensity = 0.0;
			double waterDensity = 0.0;

			vector<double> percents;

			for (int i = 0; i < weed.second->nearestGridPoints.size(); i++) {
				//cout << "location: ";
				//weed.second->location.show();
				//cout << "weed.second->nearestGridPoints["<<i<<"]" << weed.second->nearestGridPoints[i].x << "," << weed.second->nearestGridPoints[i].y << endl;
				auto loc = weed.second->nearestGridPoints[i];
				//cout << "food here: " << foodMap(loc.x, loc.y) << endl;
				//cout << "water here: " << waterMap(loc.x, loc.y) << endl;
				foodDensity += foodMap(loc.x, loc.y) * weed.second->gridPercents[i];
				waterDensity += waterMap(loc.x, loc.y) * weed.second->gridPercents[i];

				//cout << "food: " << food << "   water: " << water << endl;
			}
			//cout << "after ---   " << foodDensity << " " << waterDensity << endl;
			//exit(1);

			double neighborPercentMultiplier = 1;
			//double maxTakenAway = 0.5; //dont want them to starve completely

			//subtract off the food that the neighbors would leech
			for (auto neighbor : weed.second->neighborsList) {

				double currentRootSpread = weed.second->rootSpread;
				double neighborRootSpread = neighbor.second->rootSpread;
				double neighborDistance = weed.second->neighborsDistanceList[neighbor.first];
				// if the roots actually overlap
				if (neighborRootSpread >= currentRootSpread) {
					double overlap = ((currentRootSpread + neighborRootSpread) - neighborDistance);
					//cout << "   currentRootSpread: " << currentRootSpread << endl;
					//cout << "   neighborRootSpread: " << neighborRootSpread << endl;
					//cout << "   neighborDistance: " << neighborDistance << endl;

					//cout << "     overlap: " << overlap << endl;
					if (overlap > 0) {
						//subtract off percentage of overlap
						neighborPercentMultiplier -= (overlap / (currentRootSpread)) * .25;
						//cout << "   neighborMultiplier: " << neighborPercentMultiplier << endl;
					}
				}
				else {
					double overlap = ((currentRootSpread + neighborRootSpread) - neighborDistance);
					//cout << "   currentRootSpread: " << currentRootSpread << endl;
					//cout << "   neighborRootSpread: " << neighborRootSpread << endl;
					//cout << "   neighborDistance: " << neighborDistance << endl;

					//cout << "     overlap: " << overlap << endl;
					if (overlap > 0) {
						//subtract off percentage of overlap
						neighborPercentMultiplier -= (overlap / (currentRootSpread)) * .0025;
						//cout << "   neighborMultiplier: " << neighborPercentMultiplier << endl;
					}
				}
			}

			//choose to use min amount of food given or what the neighbors take away
			neighborPercentMultiplier = max(0.0, neighborPercentMultiplier);
			// These need to be reduced based on neighbors

			//cout << "neighborPercentMultiplier: " << neighborPercentMultiplier << endl;

			foodDelta =(weed.second->rootSpread/rootSpreadMax) * (weed.second->rootGirth/rootGirthMax) * foodDensity * neighborPercentMultiplier;

			//cout << "feedDensity: " << foodDensity << endl;
			//cout << "foodDelta: " << foodDelta << endl;

			weed.second->food += foodDelta;
			waterDelta = (weed.second->rootSpread / rootSpreadMax) * (weed.second->rootGirth / rootGirthMax) * waterDensity * neighborPercentMultiplier;
			weed.second->water += waterDelta;



			for (auto loc : weed.second->nearestGridPoints) {
				foodOut(loc.x, loc.y) = foodOut(loc.x, loc.y) + 1;// weed.second->gridPercents[i];
				waterOut(loc.x, loc.y) = waterOut(loc.x, loc.y) + 1;// weed.second->gridPercents[i];
			}

			//cout << "foodDelta: " << foodDelta << "  " << weed.second->food << "   waterDelta: " << waterDelta << "  " << weed.second->water << endl;
		}

		//resource management
		for (int x = 0; x < worldSizeCourse; x++)
		{
			for (int y = 0; y < worldSizeCourse; y++) {
				bool foodPos = foodMap(x, y) > 0;
				foodMap(x, y) = min(foodMap(x, y) - foodOut(x, y) + foodIn(x, y), foodMax(x, y));
				waterMap(x, y) = min(waterMap(x, y) - waterOut(x, y) + waterIn(x, y), waterMax(x, y));
				//foodMap(x, y) = max(min(foodMap(x, y) - foodOut(x, y) + foodIn(x, y), foodMax(x, y)), 0.0);
				//waterMap(x, y) = max(min(waterMap(x, y) - waterOut(x, y) + waterIn(x, y), waterMax(x, y)), 0.0);
				if (foodPos && foodMap(x, y) < 0) {
					foodMap(x, y) = -100.0;
				}
				/*
				if (Global::update % 1 == 0) {
					foodMap(x, y) = min(foodMap(x, y) - foodOut(x, y) + foodIn(x, y), foodMax(x, y));
					waterMap(x, y) = min(waterMap(x, y) - waterOut(x, y) + waterIn(x, y), waterMax(x, y));
				}
				else {
					foodMap(x, y) = min(foodMap(x, y) - foodOut(x, y), foodMax(x, y));
					waterMap(x, y) = min(waterMap(x, y) - waterOut(x, y), waterMax(x, y));
				}
				*/
				foodOut(x, y) = 0;
				waterOut(x, y) = 0;
			}
		}

		for (auto weed : weeds) {
			auto brain = weed.second->org->brains[brainName];
			//set investments to be 0 since we have to write to the data map
			//even if the weed doesnt update
			float rootSpreadInvestment, rootGirthInvestment, stalkHeightInvestment, leafCoverageInvestment;
			rootSpreadInvestment = rootGirthInvestment = stalkHeightInvestment = leafCoverageInvestment = 0;


			//upkeep parts of the plant
			//this needs to happen before we check for death

			//confirm that weed has enough water to maintain its leaves
			//otherwise wither the leaves
			//including other, usually 0, upkeeps here for consistency
			//if (weed.second->water < 0){ //// ALWAYS PAY WATER UPKEEP!
			weed.second->water -= weed.second->leafCoverage * leafCoverageWaterUpkeep; // this is the one we care about
			weed.second->water -= weed.second->rootGirth * rootGirthWaterUpkeep;
			weed.second->water -= weed.second->rootSpread * rootSpreadWaterUpkeep;
			weed.second->water -= weed.second->stalkHeight * stalkHeightWaterUpkeep;
			//}

			if (weed.second->water <= 0) {
				weed.second->leafCoverage *= leafWitherFactor;
			}

			//how much farther your food will go if you have leaves (by being more efficent)
			//0.5 is the max they can possibly save on food (the plants still have to spend food)
			float foodReducer = 1.0 - (0.5 * (weed.second->leafCoverage / leafCoverageMax));

			weed.second->food -= weed.second->rootSpread * rootSpreadUpkeep * foodReducer;
			weed.second->food -= weed.second->rootGirth * rootGirthUpkeep * foodReducer;
			weed.second->food -= weed.second->leafCoverage * leafCoverageUpkeep * foodReducer;
			weed.second->food -= weed.second->stalkHeight * stalkHeightUpkeep * foodReducer;

			//evaluate death (starvation/old age)
			if (weed.second->timeOfDeath <= Global::update || weed.second->food < 1) {
				weed.second->die = true;
				worldData.Append("ageAtDeath", weed.second->age);
				deaths++;
			}
			else if (true && weed.second->seedContributionRate * contributionFactor >= 1) {
				//if a weed will die when it creates a seed, kill it automatically
				//I'm not sure where best to put this but it works here
				weed.second->die = true;
				worldData.Append("ageAtDeath", weed.second->age);
				deaths++;
			}
			else {
				//siganl to plants how much food they have.
				brain->setInput(0, weed.second->food > 3);
				brain->setInput(1, weed.second->food > 60);
				brain->setInput(2, weed.second->food > 90);
				brain->setInput(3, weed.second->food > 120);

				brain->update();

				double produce = Bit(brain->readOutput(0));

				//weeds can EITHER grow or produce.
				if (produce) {
					if (weed.second->stalkHeight > weed.second->rootSpread) {
						double seedContribution = weed.second->food * weed.second->seedContributionRate;
						//cout << "seedContributionRate: " << weeds[o->ID]->seedContributionRate << "  seedContribution: " << seedContribution << endl;
						//cout << weeds[o->ID]->food;
						if (weed.second->food > seedContribution * 5) {
							weed.second->produce = true;
						}
						else {
							weed.second->food *= .75; // if too little food, pay a 25% penalty!
						}
					}
				}
				else {

					for (int i = 0; i < numGrowthRegimes; i++) {
						//average the growth regimes that the weeds have chosen
						bool current = Bit(brain->readOutput(i + 1)); //index 0 is used for grow 
						if (current) {
							rootSpreadInvestment += weed.second->growthPatterns[i].rootSpreadInvestment * .01;
							rootGirthInvestment += weed.second->growthPatterns[i].rootGirthInvestment * .01;
							stalkHeightInvestment += weed.second->growthPatterns[i].stalkHeightInvestment * .01;
							leafCoverageInvestment += weed.second->growthPatterns[i].leafCoverageInvestment * .01;
						}
					} //end for loop getting averages

					if (rootSpreadInvestment || rootGirthInvestment || stalkHeightInvestment || leafCoverageInvestment) {
						//not everything was zero (could happen if nothing was on)
						//
						//average all of the totals
						float total = rootSpreadInvestment + rootGirthInvestment + stalkHeightInvestment + leafCoverageInvestment;
						rootSpreadInvestment /= total;
						rootGirthInvestment /= total;
						stalkHeightInvestment /= total;
						leafCoverageInvestment /= total;

						float rootSpreadDiff, rootGirthDiff, stalkHeightDiff, leafCoverageDiff;

						//get update values
						//Here is where we would only grow things based on whether or not they have water
						rootSpreadDiff = min(rootSpreadMax, weed.second->rootSpread + rootSpreadInvestment * (rootSpreadMax / lifeTimeMin) * 2) - weed.second->rootSpread;
						rootGirthDiff = min(rootGirthMax, weed.second->rootGirth + rootGirthInvestment * (rootGirthMax / lifeTimeMin) * 2) - weed.second->rootGirth;
						stalkHeightDiff = min(stalkHeightMax, weed.second->stalkHeight + stalkHeightInvestment * (stalkHeightMax / lifeTimeMin) * 2) - weed.second->stalkHeight;
						leafCoverageDiff = min(leafCoverageMax, weed.second->leafCoverage + leafCoverageInvestment * (leafCoverageMax / lifeTimeMin) * 2) - weed.second->leafCoverage;

						//check to see if they have water enough for stalk and leaves
						float proposedStalkHeight = weed.second->stalkHeight + stalkHeightDiff;
						float proposedLeafCoverage = weed.second->leafCoverage + leafCoverageDiff;
						float totalWaterProposed = proposedStalkHeight + proposedLeafCoverage;
						if (weed.second->water >= totalWaterProposed) {
							weed.second->water -= totalWaterProposed;
						}
						else {
							leafCoverageDiff = 0;
							stalkHeightDiff = 0;
						}

						//actually grow the plant
						weed.second->rootSpread += rootSpreadDiff;
						weed.second->rootGirth += rootGirthDiff;
						weed.second->stalkHeight += stalkHeightDiff;
						weed.second->leafCoverage += leafCoverageDiff;

						float foodReducer = 1.0 - (0.5 * (weed.second->leafCoverage / leafCoverageMax));
						if (rootSpreadDiff > 0) {
							weed.second->food -= weed.second->rootSpread  * foodReducer;
						}
						if (rootGirthDiff > 0) {
							weed.second->food -= weed.second->rootGirth  * foodReducer;
						}
						if (stalkHeightDiff > 0) {
							weed.second->food -= weed.second->stalkHeight  * foodReducer;
						}
						if (leafCoverageDiff > 0) {
							weed.second->food -= weed.second->leafCoverage  * foodReducer;
						}
					}// end of it they want to grow things

					//do things not get penalize if they dont grow?
		  //.2 is the cost they pay every time they grow.
		  //if they dont grow at all (try to grow past the max)
		  //they will still pay this penalty
					//weed.second->food -= (weed.second->rootSpread + weed.second->rootGirth + weed.second->stalkHeight + weed.second->leafCoverage);
					weed.second->food = max(weed.second->food, 0.0);

				}//end of grow clause
			}//end if not dead clause

			//update DataMaps
			weed.second->org->dataMap.Set("score", weed.second->numOffspring);
			weed.second->org->dataMap.Set("food", weed.second->food);
			weed.second->org->dataMap.Set("water", weed.second->water);
			weed.second->org->dataMap.Set("light", weed.second->light);
			weed.second->org->dataMap.Set("energy", weed.second->energy);
			weed.second->org->dataMap.Set("numOffspring", weed.second->numOffspring);
			weed.second->org->dataMap.Set("produceFailed", weed.second->produceFailed);
			weed.second->org->dataMap.Set("stalkHeight", weed.second->stalkHeight);
			weed.second->org->dataMap.Set("rootGirth", weed.second->rootGirth);
			weed.second->org->dataMap.Set("rootSpread", weed.second->rootSpread);
			weed.second->org->dataMap.Set("leafCoverage", weed.second->leafCoverage);
			weed.second->org->dataMap.Set("x", weed.second->location.x);
			weed.second->org->dataMap.Set("y", weed.second->location.y);

			//had to be updated outside of the loop where we average out the results 
			//because this is no longer a single member variable
			weed.second->org->dataMap.Set("stalkHeightInvestment", stalkHeightInvestment);
			weed.second->org->dataMap.Set("rootGirthInvestment", rootGirthInvestment);
			weed.second->org->dataMap.Set("rootSpreadInvestment", rootSpreadInvestment);
			weed.second->org->dataMap.Set("leafCoverageInvestment", leafCoverageInvestment);
		}
		if (population.size() < 1) {
			cout << " populaiton has dropped to 0" << endl;
			groups[groupName]->archivist->finished = true;
		}
		else {
			//cout << "calling archive" << endl;
			groups[groupName]->archive();
			//cout << "done? " << groups[groupName]->archivist->finished << endl;
			if (saveLocations && Global::update % saveLocationsStep == 0) {
				cout << "saving locations" << endl;
				string weedData = "";

				for (auto weed : weeds) {
					weedData += to_string(weed.second->location.x) + " " + to_string(weed.second->location.y) + " " +
						to_string(weed.second->rootSpread) + " " + to_string(weed.second->rootGirth) + " " +
						to_string(weed.second->stalkHeight) + " " + to_string(weed.second->leafCoverage) + " " +
						to_string(weed.second->colorR) + " " + to_string(weed.second->colorG) + " " + to_string(weed.second->colorB) + "\n";
				}
				weedData += "stop\n";
				FileManager::writeToFile("weedLocations.txt", weedData);
				FileManager::closeFile("weedLocations.txt");

			}


			if (saveWorldData && (Global::update % saveWorldDataStep == 0)) {
				minX = worldSize;
				minY = worldSize;
				maxX = 0;
				maxY = 0;

				for (auto o : population) {
					if (weeds[o->ID]->location.x < minX) {
						minX = weeds[o->ID]->location.x;
					}
					if (weeds[o->ID]->location.y < minY) {
						minY = weeds[o->ID]->location.y;
					}
					if (weeds[o->ID]->location.x > maxX) {
						maxX = weeds[o->ID]->location.x;
					}
					if (weeds[o->ID]->location.y > maxY) {
						maxY = weeds[o->ID]->location.y;
					}
				}
			}

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (1){// && (Global::update % saveWorldDataStep == 0)) { // save food and water maps
				string foodData = "";
				string waterData = "";
				for (int y = 0; y < worldSizeCourse; y++)
				{
					for (int x = 0; x < worldSizeCourse; x++) {
						foodData += to_string(foodMap(x, y)) + " ";
						waterData += to_string(waterMap(x, y)) + " ";
					}
					foodData += "\n";
					waterData += "\n";
				}
				foodData += waterData;
				FileManager::writeToFile("resource.txt", foodData,to_string(worldSizeCourse));
				FileManager::closeFile("resource.txt");

			
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			Global::update++;
			cout << endl;
			cout << "update: " << Global::update << flush;
			cout << "  popSize: " << population.size() << flush;
			cout << "  weedsSize: " << weeds.size() << endl;
		}

		nextPopulation.clear();

		for (auto o : population) {
			// survive?
			if (weeds[o->ID]->die) {
				////cout << "killing: " << o->ID << endl;
				for (auto neighborWeed : weeds[o->ID]->neighborsList) {
					neighborWeed.second->neighborsList.erase(o->ID);
					neighborWeed.second->neighborsDistanceList.erase(o->ID);
				}
				weedIDMap(weeds[o->ID]->coarseLocation.x, weeds[o->ID]->coarseLocation.y).erase(o->ID);
				weeds.erase(o->ID);
				o->kill();
			}
			else {
				nextPopulation.push_back(o);


				// birth
				bool canProduce = true;
				double newColorR = 0.0;
				double newColorG = 0.0;
				double newColorB = 0.0;

				if (weeds[o->ID]->produce) {
					weeds[o->ID]->produce = false;

					shared_ptr<Organism> newOrg;
					shared_ptr<Organism> otherOrg;
					if (sex) {
						if (Random::P(sexSelfPercent)) {
							newOrg = o->makeMutatedOffspringFromMany({ o, o });
							newColorR = min(1.0, max(0.0, weeds[o->ID]->colorR * Random::getDouble(.9, 1.1)));
							newColorG = min(1.0, max(0.0, weeds[o->ID]->colorG * Random::getDouble(.9, 1.1)));
							newColorB = min(1.0, max(0.0, weeds[o->ID]->colorB * Random::getDouble(.9, 1.1)));
						}
						else {
							//cout << "in sex" << endl;
							if (sexWellMixed) { // select a random 'mate'
								otherOrg = population[Random::getIndex(population.size())];
							} // end well mixed
							else { // not well mixed
								double allDistance = 0;
								vector<double> distances;

								if (sexEffectedByStalkHeight) {
									//cout << "sexEffectedByStalkHeight" << endl;
									for (auto dist : weeds[o->ID]->neighborsDistanceList) {
										distances.push_back(((influenceRadiusMax * 2) - dist.second) * weeds[dist.first]->stalkHeight);
										////cout << dist.second << " * " << weeds[dist.first]->stalkHeight << " = " << distances.back() << endl;
										allDistance += distances.back();
									}
								}
								else { // sex not effected by Stalk Height
									for (auto dist : weeds[o->ID]->neighborsDistanceList) {
										distances.push_back(((influenceRadiusMax * 2) - dist.second));
										allDistance += distances.back();
									}
								}

								double whichDist = Random::getDouble(allDistance);
								////for (auto x : distances) {
								////	cout << " " << x;
								////}
								////cout << "\n  whichDist: " << whichDist << endl;
								allDistance = 0;
								int distIndex = 0;
								bool found = false;
								for (auto dist : weeds[o->ID]->neighborsDistanceList) {
									allDistance += distances[distIndex++];
									//cout << allDistance << "  " << whichDist << " --- " << endl;
									if (allDistance > whichDist && !found) {
										found = true;
										otherOrg = weeds[dist.first]->org;
									}
								}
								if (allDistance == 0) {
									//cout << "no mate found!" << endl;
									weeds[o->ID]->produceFailed++;
									newWeedsFail++;
									// if no mate found, self
									if (sexSelfIfNoMate) {
										otherOrg = o;
									}
									else {
										canProduce = false;
									}
								}
								newOrg = o->makeMutatedOffspringFromMany({ o, otherOrg });
								newColorR = min(1.0, max(0.0, ((weeds[o->ID]->colorR + weeds[otherOrg->ID]->colorR) / 2)* Random::getDouble(.97, 1.03)));
								newColorG = min(1.0, max(0.0, ((weeds[o->ID]->colorG + weeds[otherOrg->ID]->colorG) / 2)* Random::getDouble(.97, 1.03)));
								newColorB = min(1.0, max(0.0, ((weeds[o->ID]->colorB + weeds[otherOrg->ID]->colorB) / 2)* Random::getDouble(.97, 1.03)));

							} // end not well mixed
						}
					}
					else { // asexual reproduction
						newOrg = o->makeMutatedOffspringFrom(o);
						newColorR = min(1.0, max(0.0, weeds[o->ID]->colorR * Random::getDouble(.97, 1.03)));
						newColorG = min(1.0, max(0.0, weeds[o->ID]->colorG * Random::getDouble(.97, 1.03)));
						newColorB = min(1.0, max(0.0, weeds[o->ID]->colorB * Random::getDouble(.97, 1.03)));
					}

					if (canProduce) {
						newLocation = turnTable.movePoint(weeds[o->ID]->location, Random::getIndex(360), Random::getDouble(offspringMinDistance * weeds[o->ID]->stalkHeight, weeds[o->ID]->stalkHeight));
						if (newLocation.x >= (double)(worldGridSize)+1.0 && newLocation.x <= worldSize - ((double)(worldGridSize)+1.0) &&
							newLocation.y >= (double)(worldGridSize)+1.0 && newLocation.y <= worldSize - ((double)(worldGridSize)+1.0)) {
							newWeeds++;
							weeds[o->ID]->numOffspring++;
							nextPopulation.push_back(newOrg);
							// get a new location in an area around parent based on stalkHeight.
							newLocation = turnTable.movePoint(weeds[o->ID]->location, Random::getIndex(360), Random::getDouble(offspringMinDistance * weeds[o->ID]->stalkHeight, weeds[o->ID]->stalkHeight));
							////cout << "from: " << o->ID << "  at: " << weeds[o->ID]->location.x << "," << weeds[o->ID]->location.y;
							// make sure that new point is at least influenceRadiusMax from all edges
							newLocation.x = min(max((double)(worldGridSize)+1.0, newLocation.x), worldSize - ((double)(worldGridSize)+1.0));
							newLocation.y = min(max((double)(worldGridSize)+1.0, newLocation.y), worldSize - ((double)(worldGridSize)+1.0));

							newCoarseLocation.set((int)(newLocation.x / (double)worldGridSize), (int)(newLocation.y / (double)worldGridSize));
							////cout << " to: " << newOrg->ID << " at: " << newLocation.x << "," << newLocation.y << endl;
							double seedContribution = weeds[o->ID]->food * weeds[o->ID]->seedContributionRate;
							//cout << "seedContributionRate: " << weeds[o->ID]->seedContributionRate << "  seedContribution: " << seedContribution << endl;
							//cout << weeds[o->ID]->food;
							//contribution factor is how much each weed is penalized from its food
							//when it makes an offspring
							weeds[o->ID]->food -= seedContribution * contributionFactor;
							//cout << "  -->>  " << weeds[o->ID]->food << "    " << seedContribution << endl;
							shared_ptr<Weed> newWeed = make_shared<Weed>(newOrg, newLocation, newCoarseLocation, seedContribution - germinationCost, Global::update + Random::getInt(lifeTimeMin, lifeTimeMax), numGrowthRegimes, advanceDistance);
							newWeed->colorR = newColorR;
							newWeed->colorG = newColorG;
							newWeed->colorB = newColorB;
							integrateWeed(newOrg->ID, newWeed);
						}
						else { // offspring off edge of world :(
							weeds[o->ID]->produceFailed++;
							newWeedsFail++;
						}

					} // end can produce

				} // end produce

			}
		}
		if (newWeedsFail > 0) {
			cout << newWeedsFail << " weeds failed to find mates." << endl;
		}
		// save worldData
		//cout << "saveWorldData: " << saveWorldData << "   " << endl;
		//cout << "Global::update: " << Global::update << endl;
		//cout << "saveWorldDataStep: " << saveWorldDataStep << endl;
		//cout << "Global::update % saveWorldDataStep: " << Global::update % saveWorldDataStep << endl;


		if (saveWorldData && (Global::update % saveWorldDataStep == 0)) {
			worldData.Set("minX", minX);
			worldData.Set("minY", minY);
			worldData.Set("maxX", maxX);
			worldData.Set("maxY", maxY);

			worldData.Set("update", Global::update);
			worldData.Set("population", (int)population.size());
			worldData.Set("deaths", deaths);
			if (deaths == 0) {
				worldData.Append("ageAtDeath", 0);
			}
			worldData.Set("newWeeds", newWeeds);
			worldData.Set("newWeedsFail", newWeedsFail);
			worldData.setOutputBehavior("ageAtDeath", DataMap::AVE);
			worldData.setOutputBehavior("age", DataMap::AVE);
			worldData.writeToFile("worldData.csv");
		}
		population = nextPopulation;
		groups[groupName]->population = population;
	}
}
