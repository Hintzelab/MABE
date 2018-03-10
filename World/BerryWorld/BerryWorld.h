//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once					// directive to insure that this .h file is only included one time

#include <ctype.h>

#include "../../Utilities/Utilities.h"
#include "Utilities/VectorNd.h"
#include "Utilities/TurnTable.h"
#include "Utilities/SensorArcs.h"

#include "../../World/AbstractWorld.h" // AbstractWorld defines all the basic function templates for worlds

using namespace std;

class BerryWorld : public AbstractWorld {

public:
	// example ParameterLinks (PLs)
	//static shared_ptr<ParameterLink<int>> exampleIntPL;
	//static shared_ptr<ParameterLink<double>> exampleDoublePL;
	//static shared_ptr<ParameterLink<bool>> exampleBoolPL;
	//static shared_ptr<ParameterLink<string>> exampleStringPL;

	// Example code for MTree
	//   static shared_ptr<ParameterLink<string>> exampleMTreePL;
	// MTree to hold the MTree
	//   shared_ptr<Abstract_MTree> exampleMT;

	static shared_ptr<ParameterLink<int>> worldSizeXPL;
	static shared_ptr<ParameterLink<int>> worldSizeYPL;
	static shared_ptr<ParameterLink<bool>> worldHasWallPL;
	static shared_ptr<ParameterLink<int>> rotationResolutionPL;
	static shared_ptr<ParameterLink<double>> maxTurnPL;

	static shared_ptr<ParameterLink<int>> evalTimePL;
	static shared_ptr<ParameterLink<int>> foodTypesPL;

	static shared_ptr<ParameterLink<double>> switchCostPL;
	static shared_ptr<ParameterLink<double>> hitWallCostPL;
	static shared_ptr<ParameterLink<double>> hitOtherCostPL;

	static shared_ptr<ParameterLink<int>> alwaysStartOnPL;

	static shared_ptr<ParameterLink<string>> initialFoodDistributionPL;
	vector<int> initialFoodDistribution;
	static shared_ptr<ParameterLink<double>> reward1PL;
	static shared_ptr<ParameterLink<double>> reward2PL;
	static shared_ptr<ParameterLink<double>> reward3PL;
	static shared_ptr<ParameterLink<double>> reward4PL;
	static shared_ptr<ParameterLink<double>> reward5PL;
	static shared_ptr<ParameterLink<double>> reward6PL;
	static shared_ptr<ParameterLink<double>> reward7PL;
	static shared_ptr<ParameterLink<double>> reward8PL;
	static shared_ptr<ParameterLink<string>> replace1PL;
	static shared_ptr<ParameterLink<string>> replace2PL;
	static shared_ptr<ParameterLink<string>> replace3PL;
	static shared_ptr<ParameterLink<string>> replace4PL;
	static shared_ptr<ParameterLink<string>> replace5PL;
	static shared_ptr<ParameterLink<string>> replace6PL;
	static shared_ptr<ParameterLink<string>> replace7PL;
	static shared_ptr<ParameterLink<string>> replace8PL;

	static shared_ptr<ParameterLink<double>> poison0PL;
	static shared_ptr<ParameterLink<double>> poison1PL;
	static shared_ptr<ParameterLink<double>> poison2PL;
	static shared_ptr<ParameterLink<double>> poison3PL;
	static shared_ptr<ParameterLink<double>> poison4PL;
	static shared_ptr<ParameterLink<double>> poison5PL;
	static shared_ptr<ParameterLink<double>> poison6PL;
	static shared_ptr<ParameterLink<double>> poison7PL;
	static shared_ptr<ParameterLink<double>> poison8PL;

	static shared_ptr<ParameterLink<int>> visionSensorDistancePL;
	static shared_ptr<ParameterLink<int>> visionSensorArcSizePL;
	static shared_ptr<ParameterLink<string>> visionSensorDirectionsPL;

	static shared_ptr<ParameterLink<int>> smellSensorDistancePL;
	static shared_ptr<ParameterLink<int>> smellSensorArcSizePL;
	static shared_ptr<ParameterLink<string>> smellSensorDirectionsPL;
	
	static shared_ptr<ParameterLink<bool>> seeFoodPL;
	static shared_ptr<ParameterLink<bool>> smellFoodPL;
	static shared_ptr<ParameterLink<bool>> perfectDetectsFoodPL;

	static shared_ptr<ParameterLink<bool>> seeOtherPL;
	static shared_ptr<ParameterLink<bool>> smellOtherPL;
	static shared_ptr<ParameterLink<bool>> perfectDetectsOtherPL;

	static shared_ptr<ParameterLink<bool>> seeWallsPL;
	static shared_ptr<ParameterLink<bool>> smellWallsPL;
	static shared_ptr<ParameterLink<bool>> perfectDetectsWallsPL;

	static shared_ptr<ParameterLink<bool>> useDownSensorPL;
	static shared_ptr<ParameterLink<bool>> useSmellSensorPL;
	static shared_ptr<ParameterLink<bool>> useVisionSensorPL;
	static shared_ptr<ParameterLink<bool>> usePerfectSensorPL;
	static shared_ptr<ParameterLink<string>> perfectSensorFilePL;

	static shared_ptr<ParameterLink<bool>> wallsBlockVisonSensorsPL;
	static shared_ptr<ParameterLink<bool>> wallsBlockSmellSensorsPL;

	static shared_ptr<ParameterLink<bool>> allowMoveAndEatPL;
	static shared_ptr<ParameterLink<bool>> alwaysEatPL;

	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
	static shared_ptr<ParameterLink<int>> evaluateGroupSizePL;
	static shared_ptr<ParameterLink<string>> cloneScoreRulePL;
	static shared_ptr<ParameterLink<int>> clonesPL;


	// parameters for group and brain namespaces
	static shared_ptr<ParameterLink<string>> groupNameSpacePL;
	static shared_ptr<ParameterLink<string>> brainNameSpacePL;
	
	static shared_ptr<ParameterLink<double>> moveDefaultPL;
	static shared_ptr<ParameterLink<double>> moveMinPL;
	static shared_ptr<ParameterLink<bool>> snapToGridPL;

	static shared_ptr<ParameterLink<string>> mapFilesPL;
	static shared_ptr<ParameterLink<string>> whichMapsPL;


	class WorldMap {
	public:

		class ResourceGenerator {
		public:
			int geo = 0; // 0 is point generator, 1 is square generator
			int x, y, x1, y1; // x,y point location, x,y,x1,y1 square generator area
			vector<double> rate; // list of values which determin when generator fires. if value < 1, random % chance per update. if >=1 then inflow on this step
								// if more then one value then they fire in order
			int replaceRule = 3; // 0: : replace none, 1: replace food only, 2: replace walls only, 4 replace all (food and walls)
			vector<vector<double>> resourcePrograms; // on inflow, a random element is selected form resources. If this element contains more then one value then
										// the next value in the list is temporaraly the rate and the following value is the next resource (i.e. this allows
										// for a chain of replacement or in the case of [1,5,0] to drop a temporary resource
			int rateIndex = 0; // will iterate over rates
			int resourceIndex = Random::getIndex(resourcePrograms.size()); // which resource are we currently looking at, will choose randomly from resourcePrograms
			int programIndex = 0; // in the current resource, which step of the program are we on

			int val2Time(double val) {
				if (val == 0) {
					return 0;
				}
				if (val < 1) {
					return Random::getInt(1,(int)(1.0 / val));
				}
				return (int)val;
			}

			// find out the time of the next event from this generator
			// advance correct index to the next state
			int nextEvent() {
				if (programIndex > 0) { // currently in a program, get next element in program
					if (resourcePrograms[resourceIndex].size() > programIndex){ // advance program
						return val2Time(resourcePrograms[resourceIndex][programIndex++]);
					}
					else { // this program is over, set to -1 and advance rateIndex
						programIndex = 0;
						return nextEvent();
					}
				} else { // not in program
					resourceIndex = Random::getIndex(resourcePrograms.size()); // get a new resourceIndex
					if (rateIndex >= (int)rate.size()) {
						rateIndex = 0;
					} 
					return val2Time(rate[rateIndex++]);
				}
			}

			int getNextResource(int currentVal = 0) {
				int nextResource = resourcePrograms[resourceIndex][programIndex++];
				if (nextResource == currentVal || currentVal > 9){
					return -1; // newfood = current val, or currentVal indicates that location is occupied... no change, ignore nextResource
				}
				else if (currentVal == WALL && (replaceRule == 0 || replaceRule == 1)) {
					return -1; // current val is wall, but replace rule does not replace walls... no change, ignore nextResource
				}
				else if ((currentVal > 0 && currentVal < 9) && (replaceRule == 0 || replaceRule == 2)) { 
					return -1; // current val is food, but replace rule does not replace food... no change, ignore nextResource
				}
				else {
					return nextResource;
				}
			}

			Point2d getLocation() { // return a location - if point mode, then x,y if square mode then random point in x,x1,y,y1
				if (geo == 0) {
					return Point2d(x, y);
				}
				else {
					return Point2d(Random::getInt(x, x1), Random::getInt(y, y1));
				}
			}
		};

		int worldX;
		int worldY;
		string file;
		string name;
		Vector2d<int> data;
		Vector2d<int> startData;
		vector<Point2d> startLocations;
		vector<int> startFacing;
		bool useStartMap = false;

		vector<ResourceGenerator> generators; // index will act as lookup key in generator events

		bool loadMap(ifstream& ss, const string fileName);
	};

	vector<WorldMap::ResourceGenerator> generators; // index will act as lookup key in generator events
	map<int, vector<int>> generatorEvents; // each vector<int> holds indexes for generators to run when world update (t) = key.

	enum mapValues { EMPTY = 0, WALL = 9 };


	class Harvester {
	public:
		shared_ptr<Organism> org;
		shared_ptr<AbstractBrain> brain;
		int ID;
		int cloneID;
		bool isClone = false;
		vector<shared_ptr<Harvester>> clones;
		
		Point2d loc; // location
		int face; // facing direction
		vector<int> foodCollected;
		vector<int> poisonTotals;
		int lastFoodCollected;
		int switches = 0; // how many times did this harverster switch foods?
		int wallHits = 0; // how many times did this harverster try to move into a wall?
		int otherHits = 0; // how many times did this harverster try to move into another harvester?

		// these vars are only used at the end to collect stats
		double score;
		int maxFood;
		int totalFood;
		double foodScore;
		double poisonCost = 0;

	};

	int rotationResolution;
	double maxTurn;
	Sensor visionSensor;
	Sensor smellSensor;
	int visionSensorCount;
	int smellSensorCount;

	int visionSensorDistance;
	int visionSensorArcSize;
	vector<int> visionSensorDirections;

	int smellSensorDistance;
	int smellSensorArcSize;
	vector<int> smellSensorDirections;

	vector<Point2d> moveDeltas;

	bool alwaysEat;
	bool allowMoveAndEat;

	vector<double> foodRewards;
	vector<vector<int>> replaceRules;
	vector<double> poisonRules;

	int worldX;
	int worldY;

	double moveDefault;
	double moveMin;

	bool snapToGrid;

	int foodTypes;

	int wallsBlockVisonSensors;
	int wallsBlockSmellSensors;

	int cloneScoreRule;

	map<string, map<string, WorldMap>> worldMaps; // [type][name]
	vector<string> mapFiles;
	map<string, vector<string>> mapNames;
	vector<string> whichMaps;

	vector<vector<vector<Point2d>>> perfectSensorSites;

	BerryWorld(shared_ptr<ParametersTable> _PT);
	virtual ~BerryWorld() = default;

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) override;
	void runWorld(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug);

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override;

	// takes x,y and updates them by moving one step in facing
	Point2d moveOnGrid(shared_ptr<Harvester> harvester,double distance) {
		Point2d newLoc;
		//cout << "deltas: " << moveDeltas[harvester->face].x << "," << moveDeltas[harvester->face].y << endl;
		newLoc.x = loopModDouble((harvester->loc.x + (moveDeltas[harvester->face].x * distance)), worldX);
		newLoc.y = loopModDouble((harvester->loc.y + (moveDeltas[harvester->face].y * distance)), worldY);
		return(newLoc);
	}


};

