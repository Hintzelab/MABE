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

#include <stdlib.h>
#include <thread>
#include <vector>
#include "../../Utilities/Data.h"
#include "../../Utilities/VectorNd.h"
#include "../../Utilities/PointNd.h"
#include "../../Utilities/SensorArcs.h"
#include "../../Utilities/TurnTable.h"

using namespace std;


//shared_ptr<ParameterLink<int>> MorrisTestWorld::numberOfDirectionsPL = Parameters::register_parameter("WORLD_BERRY_PLUS-numberOfDirections", 36, "number of rotational positions");
//shared_ptr<ParameterLink<int>> MorrisTestWorld::visionSensorDistanceMaxPL = Parameters::register_parameter("WORLD_BERRY_PLUS-visionSensorDistanceMax", 5, "how far can orgs see?");
//shared_ptr<ParameterLink<int>> MorrisTestWorld::visionSensorArcSizePL = Parameters::register_parameter("WORLD_BERRY_PLUS-visionSensorArcSize", 10, "how wide is a vision arc (degrees)");
//shared_ptr<ParameterLink<string>> v::visionSensorPositionsStringPL = Parameters::register_parameter("WORLD_BERRY_PLUS-visionSensorPositions", (string) "[-2,-1,0,1,2]", "what directions can org see?");


class MorrisTestWorld : public AbstractWorld {

public:
	TurnTable turnTable;

	int platformX, platformY;
	int orgX, orgY;

	vector<int> markerXLocations, markerYLocations;

	// Parameters

	static shared_ptr<ParameterLink<int>> worldUpdatesPL;
	int worldUpdates;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;
	int evaluationsPerGeneration;

	static shared_ptr<ParameterLink<double>>  worldRadiusPL;// = 25.0;
	double worldRadius;// = 25.0;

	static shared_ptr<ParameterLink<int>> markerCountPL;
	int markerCount; // how many markers

	static shared_ptr<ParameterLink<int>> platformVisablePL;
	int platformVisable; // how many markers

	static shared_ptr<ParameterLink<int>> markerSpacingPL;
	int markerSpacing; // how will the markers be spaced

	static shared_ptr<ParameterLink<bool>>  senseDistancesPL;
	bool senseDistances;

	static shared_ptr<ParameterLink<bool>> bitBrainsPL;
	bool bitBrains; //  if true, will set up inputs to be readable in bit formate
	static shared_ptr<ParameterLink<double>> maxSpeedDeltaPL;
	double maxSpeedDelta; //  greatest amount that speed can change by in a single world update
	static shared_ptr<ParameterLink<double>> minSpeedPL;
	double minSpeed; //  lowest speed avalible, if negitive, can move backwards
	static shared_ptr<ParameterLink<double>> maxSpeedPL;
	double maxSpeed; // greatest speed avalible

	static shared_ptr<ParameterLink<double>> minTurnRatePL;
	double minTurnRate; // highest rate of rotation in negitave direction avalible
	static shared_ptr<ParameterLink<double>> maxTurnRatePL;
	double maxTurnRate; // highest rate of rotation in the positive direction avalible

	static shared_ptr<ParameterLink<int>> platformReliabilityPL;
	double platformReliability; // does the platform move, per generation, per eval

	static shared_ptr<ParameterLink<double>> platformAngleMinPL;
	double platformAngleMin; // angle for platform (i.e. can set relitive to drop zone)
	static shared_ptr<ParameterLink<double>> platformAngleMaxPL;
	double platformAngleMax; // angle for platform (i.e. can set relitive to drop zone)
	static shared_ptr<ParameterLink<double>> platformOffsetFromCenterMinPL;
	double platformOffsetFromCenterMin; // where is platfrom relitive to edge and center
	static shared_ptr<ParameterLink<double>> platformOffsetFromCenterMaxPL;
	double platformOffsetFromCenterMax; // where is platfrom relitive to edge and center
	static shared_ptr<ParameterLink<double>> platformSizePL;
	double platformSize;;

	static shared_ptr<ParameterLink<double>> agentAngleMinPL;
	double agentAngleMin; // angle for platform (i.e. can set relitive to drop zone)
	static shared_ptr<ParameterLink<double>> agentAngleMaxPL;
	double agentAngleMax; // angle for platform (i.e. can set relitive to drop zone)
	static shared_ptr<ParameterLink<double>> agentOffsetFromCenterMinPL;
	double agentOffsetFromCenterMin; // where is platfrom relitive to edge and center
	static shared_ptr<ParameterLink<double>> agentOffsetFromCenterMaxPL;
	double agentOffsetFromCenterMax; // where is platfrom relitive to edge and center

	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;
	//string groupName;
	string brainName;


	int inputCount;
	int outputCount;

	MorrisTestWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~MorrisTestWorld() = default;

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);


	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupName,{ "B:" + brainName + "," + to_string(inputCount) + "," + to_string(outputCount) } } }; // default requires a root group and a brain (in root namespace) and no genome 
	}

};

