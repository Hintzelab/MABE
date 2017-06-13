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

#include "../../Utilities/PointNd.h"
#include "../../Utilities/VectorNd.h"
#include "../../Utilities/TurnTable.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class WeedWorld : public AbstractWorld {

public:

    class WorldMap {
    public:
        string fileName;
        string mapName;
        shared_ptr<ParametersTable> PT;
        bool loadMap(ifstream& ss, const string fileNamei, shared_ptr<ParametersTable> parentPT, Vector2d<double> grid, int worldSizeCourse);
    };

	class Weed {

		struct Investment { //class to store growth growth regime
			double stalkHeightInvestment;
			double rootGirthInvestment;
			double rootSpreadInvestment;
			double leafCoverageInvestment;
		};

	public:
		//list all N growth patterns
		map<int, Investment> growthPatterns;

		double stalkHeight;
		double rootGirth;
		double rootSpread;
		double leafCoverage;
		double seedContributionRate;

		int timeOfDeath;
		int age;

		int numOffspring;
		int produceFailed;

		Point2d location;
		Point2d coarseLocation;

		shared_ptr<Organism> org;

		map<int, double> neighborsDistanceList; // org ID -> distance
		map<int, shared_ptr<Weed>> neighborsList; // org ID -> Weed

		bool die = false;
		bool produce = false;
		int numGrowthRegimes;

		double food;
		double water;
		double light;
		double energy;

		double precentSelf;

		vector<Point2d> nearestGridPoints;
		vector<double> gridPercents;

		double colorR, colorG, colorB;


		Weed(shared_ptr<Organism> _org, Point2d loc, Point2d coarseLoc, double _food, int _timeOfDeath, int _numGrowthRegimes, int advanceDistance) {
			org = _org;
			numGrowthRegimes = _numGrowthRegimes;

			colorR = 0;// Random::getDouble(1.0);
			colorG = 0;// Random::getDouble(1.0);
			colorB = 0;// Random::getDouble(1.0);

			auto handler = org->genome->newHandler(org->genome);

			handler->advanceIndex(advanceDistance);
			seedContributionRate = handler->readDouble(0.0, 1.0);

			handler->advanceIndex(advanceDistance);
			precentSelf = handler->readDouble(0.0, 1.0);

			float stalkHeightInvestment, rootGirthInvestment, rootSpreadInvestment, leafCoverageInvestment, totalInvestment;

			/*
			for (int i = 0; i < numGrowthRegimes; i++) {
				//generate all N growth patterns
				handler->advanceIndex(advanceDistance);
				stalkHeightInvestment = handler->readDouble(0.0, 1.0);

				handler->advanceIndex(advanceDistance);
				rootGirthInvestment = handler->readDouble(0.0, 1.0);

				handler->advanceIndex(advanceDistance);
				rootSpreadInvestment = handler->readDouble(0.0, 1.0);

				handler->advanceIndex(advanceDistance);
				leafCoverageInvestment = handler->readDouble(0.0, 1.0);

				//investment per location is percentage based
				totalInvestment = stalkHeightInvestment + rootGirthInvestment + rootSpreadInvestment + leafCoverageInvestment;

				Investment entry;
				entry.stalkHeightInvestment = stalkHeightInvestment / totalInvestment;
				entry.rootGirthInvestment = rootGirthInvestment / totalInvestment;
				entry.rootSpreadInvestment = rootSpreadInvestment / totalInvestment;
				entry.leafCoverageInvestment = leafCoverageInvestment / totalInvestment;

				growthPatterns[i] = entry;
			}
			*/
			growthPatterns[0].stalkHeightInvestment = 1.0;
			growthPatterns[0].rootGirthInvestment = 0.0;
			growthPatterns[0].rootSpreadInvestment = 0.0;
			growthPatterns[0].leafCoverageInvestment = 0.0;

			growthPatterns[1].stalkHeightInvestment = 0.0;
			growthPatterns[1].rootGirthInvestment = 1.0;
			growthPatterns[1].rootSpreadInvestment = 0.0;
			growthPatterns[1].leafCoverageInvestment = 0.0;

			growthPatterns[2].stalkHeightInvestment = 0.0;
			growthPatterns[2].rootGirthInvestment = 0.0;
			growthPatterns[2].rootSpreadInvestment = 1.0;
			growthPatterns[2].leafCoverageInvestment = 0.0;

			growthPatterns[3].stalkHeightInvestment = 0.0;
			growthPatterns[3].rootGirthInvestment = 0.0;
			growthPatterns[3].rootSpreadInvestment = 0.0;
			growthPatterns[3].leafCoverageInvestment = 1.0;

			//init starting values
			stalkHeight = .0;
			rootGirth = 1.0;
			rootSpread = 5.0;
			leafCoverage = .0;

			location = loc;
			coarseLocation = coarseLoc;

			food = _food;
			water = 1.0;
			light = 1.0;
			energy = 0.0;

			numOffspring = 0;

			age = 0;
			timeOfDeath = _timeOfDeath;
		}


	}; //end Weed class

    static shared_ptr<ParameterLink<string>> mapFileListPL;

	static shared_ptr<ParameterLink<int>> numGrowthRegimesPL; // number of growth regimes
	static shared_ptr<ParameterLink<int>> advanceDistancePL; // number to advance read head for growth regime
	static shared_ptr<ParameterLink<int>> contributionFactorPL; // How much the parent has to pays from thier food when making offspring
	static shared_ptr<ParameterLink<int>> germinationCostPL; // How much the parent has to pays from thier food when making offspring

	static shared_ptr<ParameterLink<int>> worldSizePL; // resolution of the world
	static shared_ptr<ParameterLink<int>> worldGridSizePL; // course resolution of the world

	static shared_ptr<ParameterLink<double>> rootSpreadMaxPL;
	static shared_ptr<ParameterLink<double>> rootGirthMaxPL;
	static shared_ptr<ParameterLink<double>> leafCoverageMaxPL;
	static shared_ptr<ParameterLink<double>> stalkHeightMaxPL;

	static shared_ptr<ParameterLink<double>> leafWitherFactorPL;

	static shared_ptr<ParameterLink<double>> rootSpreadUpkeepPL;
	static shared_ptr<ParameterLink<double>> rootGirthUpkeepPL;
	static shared_ptr<ParameterLink<double>> leafCoverageUpkeepPL;
	static shared_ptr<ParameterLink<double>> stalkHeightUpkeepPL;

	static shared_ptr<ParameterLink<double>> rootSpreadWaterUpkeepPL;
	static shared_ptr<ParameterLink<double>> rootGirthWaterUpkeepPL;
	static shared_ptr<ParameterLink<double>> leafCoverageWaterUpkeepPL;
	static shared_ptr<ParameterLink<double>> stalkHeightWaterUpkeepPL;

	static shared_ptr<ParameterLink<double>> offspringMinDistancePL;

	static shared_ptr<ParameterLink<double>> intialFoodPL;

	static shared_ptr<ParameterLink<int>> lifeTimeMinPL;
	static shared_ptr<ParameterLink<int>> lifeTimeMaxPL;

	static shared_ptr<ParameterLink<bool>> sexPL;
	static shared_ptr<ParameterLink<bool>> sexWellMixedPL;
	static shared_ptr<ParameterLink<bool>> sexEffectedByStalkHeightPL;

	static shared_ptr<ParameterLink<bool>> saveLocationsPL;
	static shared_ptr<ParameterLink<int>> saveLocationsStepPL;

	static shared_ptr<ParameterLink<bool>> saveWorldDataPL;
	static shared_ptr<ParameterLink<int>> saveWorldDataStepPL;

	static shared_ptr<ParameterLink<double>> sexSelfPercentPL;
	double sexSelfPercent;
	static shared_ptr<ParameterLink<bool>> sexSelfIfNoMatePL;
	bool sexSelfIfNoMate;

    vector<string> mapFileList;
    map<string,map<string,WorldMap>> worldMaps; // [fileName][mapName]

	bool saveLocations;
	int saveLocationsStep;

	bool saveWorldData;
	int saveWorldDataStep;

	int influenceRadiusMax; // defines max area that a weed can effect, basied on worldGridSize
	int worldSize;
	int worldGridSize;
	int worldSizeCourse;

	float leafWitherFactor;

	int rootSpreadUpkeep;
	int rootGirthUpkeep;
	int leafCoverageUpkeep;
	int stalkHeightUpkeep;

	int rootSpreadWaterUpkeep;
	int rootGirthWaterUpkeep;
	int leafCoverageWaterUpkeep;
	int stalkHeightWaterUpkeep;

	double offspringMinDistance;

	double intialFood; // food that inital population starts with
	int lifeTimeMin;
	int lifeTimeMax;

	bool sex;
	bool sexWellMixed;
	bool sexEffectedByStalkHeight;

	int numGrowthRegimes, advanceDistance;

	int contributionFactor; //how much food an organism spends to have a child
	int germinationCost; //how much food a seed loses when it is planted

	double rootSpreadMax, rootGirthMax, leafCoverageMax, stalkHeightMax;

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;

	TurnTable turnTable;

	map<int, shared_ptr<Weed>> weeds; // org ID maps to weeds	
	Vector2d<unordered_set<int>> weedIDMap; // lists which plants are in each loction

	Vector2d<double> foodMap; // where is there food?
	Vector2d<double> waterMap; // where is there water?
	Vector2d<double> foodOut; // How much food is taken?
	Vector2d<double> waterOut; // How much water is taken?
	Vector2d<double> foodIn; // How fast does food flow in?
	Vector2d<double> waterIn; // How water does food flow in?
	Vector2d<double> foodMax; // How fast does food flow in?
	Vector2d<double> waterMax; // How water does food flow in?


	WeedWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~WeedWorld() = default;

    void OpenMaps(vector<string> mapFileList, map<string, map<string,WorldMap>> worldMaps, Vector2d<double> grid);

	virtual void integrateWeed(int ID, shared_ptr<Weed> weed);

	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) override;

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupName,{ "B:" + brainName + ",4," + to_string(1 + numGrowthRegimes) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

};
