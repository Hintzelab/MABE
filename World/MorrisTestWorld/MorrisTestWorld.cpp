//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "MorrisTestWorld.h"

shared_ptr<ParameterLink<int>> MorrisTestWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_MORRISTEST-worldUpdates", 1000, "world updates per evaluation");
shared_ptr<ParameterLink<int>> MorrisTestWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_MORRISTEST-evaluationsPerGeneration", 3, "Number of times to test each Genome per generation (useful with non-deterministic brains)");

shared_ptr<ParameterLink<bool>> MorrisTestWorld::senseDistancesPL = Parameters::register_parameter("WORLD_MORRISTEST-senseDistances", false, "if true (1) then brain will sense distance to all markers (including platform if visable)");

shared_ptr<ParameterLink<bool>> MorrisTestWorld::bitBrainsPL = Parameters::register_parameter("WORLD_MORRISTEST-setupForBitBrains", false, "0 = use continuous values for brain i/o, 1 = convert i/o to binary format");
shared_ptr<ParameterLink<int>> MorrisTestWorld::platformVisablePL = Parameters::register_parameter("WORLD_MORRISTEST-platformVisable", 0, "0 = not visable, 1 = visable (adds addtional inputs), 2 = visable, but only before first find of each evaluation");
shared_ptr<ParameterLink<int>> MorrisTestWorld::platformReliabilityPL = Parameters::register_parameter("WORLD_MORRISTEST-platformReliability", 0, "0 = move platform every time platform is found, 1 = move platform between evals, 2 = do not move platfroms (all in world evals with same location)");


shared_ptr<ParameterLink<int>> MorrisTestWorld::markerCountPL = Parameters::register_parameter("WORLD_MORRISTEST-markerCount", 3, "how many edge markers around the edge of the pool?");
shared_ptr<ParameterLink<int>> MorrisTestWorld::markerSpacingPL = Parameters::register_parameter("WORLD_MORRISTEST-markerSpacing", 9, "how regular is the marker placement, higher number = more regular");
shared_ptr<ParameterLink<double>>  MorrisTestWorld::worldRadiusPL = Parameters::register_parameter("WORLD_MORRISTEST-worldRadius", 25.0, "radius of pool");

shared_ptr<ParameterLink<double>> MorrisTestWorld::maxSpeedDeltaPL = Parameters::register_parameter("WORLD_MORRISTEST-speedDeltaMax", 0.5, "how quickly can the speed change (+/-)?");
shared_ptr<ParameterLink<double>> MorrisTestWorld::minSpeedPL = Parameters::register_parameter("WORLD_MORRISTEST-speedMin", .5, "slowest speed (if negitive, can move backwards)");
shared_ptr<ParameterLink<double>> MorrisTestWorld::maxSpeedPL = Parameters::register_parameter("WORLD_MORRISTEST-speedMax", 2.0, "fastest speed");

shared_ptr<ParameterLink<double>> MorrisTestWorld::minTurnRatePL = Parameters::register_parameter("WORLD_MORRISTEST-turnRateMin", -3.0, "fastest turn rate in the left direction");
shared_ptr<ParameterLink<double>> MorrisTestWorld::maxTurnRatePL = Parameters::register_parameter("WORLD_MORRISTEST-turnRateMax", 3.0, "fastest turn rate in the right direction");

shared_ptr<ParameterLink<double>> MorrisTestWorld::platformAngleMinPL = Parameters::register_parameter("WORLD_MORRISTEST-platformAngleMin", 270.0, "rotation to platform");
shared_ptr<ParameterLink<double>> MorrisTestWorld::platformAngleMaxPL = Parameters::register_parameter("WORLD_MORRISTEST-platformAngleMax", 270.0, "rotation to platform");
shared_ptr<ParameterLink<double>> MorrisTestWorld::platformOffsetFromCenterMinPL = Parameters::register_parameter("WORLD_MORRISTEST-platformOffsetFromCenterMin", 0.6, "where is platfrom relitive to edge and center");
shared_ptr<ParameterLink<double>> MorrisTestWorld::platformOffsetFromCenterMaxPL = Parameters::register_parameter("WORLD_MORRISTEST-platformOffsetFromCenterMax", 0.6, "where is platfrom relitive to edge and center");

shared_ptr<ParameterLink<double>> MorrisTestWorld::platformSizePL = Parameters::register_parameter("WORLD_MORRISTEST-platformSize", 1.0, "size of the platfrom");


shared_ptr<ParameterLink<double>> MorrisTestWorld::agentAngleMinPL = Parameters::register_parameter("WORLD_MORRISTEST-agentAngleMin", 0.0, "rotation to agent");
shared_ptr<ParameterLink<double>> MorrisTestWorld::agentAngleMaxPL = Parameters::register_parameter("WORLD_MORRISTEST-agentAngleMax", 180.0, "rotation to agent");
shared_ptr<ParameterLink<double>> MorrisTestWorld::agentOffsetFromCenterMinPL = Parameters::register_parameter("WORLD_MORRISTEST-agentOffsetFromCenterMin", 0.95, "where is agent relitive to edge and center");
shared_ptr<ParameterLink<double>> MorrisTestWorld::agentOffsetFromCenterMaxPL = Parameters::register_parameter("WORLD_MORRISTEST-agentOffsetFromCenterMax", 0.95, "where is agent relitive to edge and center");

shared_ptr<ParameterLink<string>> MorrisTestWorld::groupNamePL = Parameters::register_parameter("WORLD_MORRISTEST_NAMES-groupName", (string)"root", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> MorrisTestWorld::brainNamePL = Parameters::register_parameter("WORLD_MORRISTEST_NAMES-brainName", (string)"root", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

MorrisTestWorld::MorrisTestWorld(shared_ptr<ParametersTable> _PT) :
	AbstractWorld(_PT) {

	worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_MORRISTEST-worldUpdates");
	evaluationsPerGeneration = (PT == nullptr) ? evaluationsPerGenerationPL->lookup() : PT->lookupInt("WORLD_MORRISTEST-evaluationsPerGeneration");

	senseDistances = (PT == nullptr) ? senseDistancesPL->lookup() : PT->lookupBool("WORLD_MORRISTEST-senseDistances");


	bitBrains = (PT == nullptr) ? bitBrainsPL->lookup() : PT->lookupBool("WORLD_MORRISTEST-setupForBitBrains");
	platformVisable = (PT == nullptr) ? platformVisablePL->lookup() : PT->lookupInt("WORLD_MORRISTEST-platformVisable");
	platformReliability = (PT == nullptr) ? platformReliabilityPL->lookup() : PT->lookupInt("WORLD_MORRISTEST-platformReliability");

	markerCount = (PT == nullptr) ? markerCountPL->lookup() : PT->lookupInt("WORLD_MORRISTEST-markerCount");
	markerSpacing = (PT == nullptr) ? markerSpacingPL->lookup() : PT->lookupInt("WORLD_MORRISTEST-markerSpacing");
	worldRadius = (PT == nullptr) ? worldRadiusPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-worldRadius");

	maxSpeedDelta = (PT == nullptr) ? maxSpeedDeltaPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-speedDeltaMax");
	minSpeed = (PT == nullptr) ? minSpeedPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-speedMin");
	maxSpeed = (PT == nullptr) ? maxSpeedPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-speedMax");

	minTurnRate = (PT == nullptr) ? minTurnRatePL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-turnRateMin");
	maxTurnRate = (PT == nullptr) ? maxTurnRatePL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-turnRateMax");

	platformAngleMin = (PT == nullptr) ? platformAngleMinPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-platformAngleMin");
	platformAngleMax = (PT == nullptr) ? platformAngleMaxPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-platformAngleMax");
	platformOffsetFromCenterMin = (PT == nullptr) ? platformOffsetFromCenterMinPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-platformOffsetFromCenterMin");
	platformOffsetFromCenterMax = (PT == nullptr) ? platformOffsetFromCenterMaxPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-platformOffsetFromCenterMax");
	platformSize = (PT == nullptr) ? platformSizePL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-platformSize");

	agentAngleMin = (PT == nullptr) ? agentAngleMinPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-agentAngleMin");
	agentAngleMax = (PT == nullptr) ? agentAngleMaxPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-agentAngleMax");
	agentOffsetFromCenterMin = (PT == nullptr) ? agentOffsetFromCenterMinPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-agentOffsetFromCenterMin");
	agentOffsetFromCenterMax = (PT == nullptr) ? agentOffsetFromCenterMaxPL->lookup() : PT->lookupDouble("WORLD_MORRISTEST-agentOffsetFromCenterMax");


	inputCount = 1; // found bit (on, the world update after a find)

	if (bitBrains) { // continuous data will be converted to bits (imprecisely)
		inputCount += markerCount * 8; // add 8 bits for each marker
		//cout << inputCount << endl;
		if (senseDistances) {
			inputCount += markerCount * 8; // if org can sense distance, add more 8 bits for each marker
		}
		//cout << inputCount << endl;
		if (platformVisable > 0) { // platform is visable at least part of the time, so we must a sensor for it
			inputCount += 8 * ((senseDistances) ? 2 : 1); // add 16 bits if sence distance is on, and 8 bits if not
		}
		//cout << inputCount << endl;
	}
	else { // not bitBrains - continuous data is delivered directly to brain
		inputCount += (markerCount * (1 + senseDistances)); // add one double for each marker and one more for each marker if sense distance
		inputCount += ((platformVisable > 0) ? (1 + senseDistances) : 0); // if platform visable, add one double, (two if sense distance) 
	}

	outputCount = 2;

	turnTable.setup(360, 36000);

	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_MORRISTEST_NAMES-groupName");
	brainName = (PT == nullptr) ? brainNamePL->lookup() : PT->lookupString("WORLD_MORRISTEST_NAMES-brainName");

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("findCount");
}

void MorrisTestWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {

	auto brain = org->brains[brainName];

	//cout << "in test MorrisTestWorld::evaluateSolo : " << org->ID << endl;

	int findCount = 0;
	bool found = false;
	double score;

	Point2d currentFacingVector, compairVector; // working variables
	Point2d currentLocation;
	double currentFacing;

	Point2d worldCenter(0, 0); // location of center of world
	Point2d upVector(0, 1); // location of center of world

	double turnRate;
	double moveRate;

	Point2d platformCenter;

	vector<double> markerAngles;
	vector<Point2d> markerLocations;

	// save init data
	string visFileName = "MT_vis.txt";
	if (visualize) {
		// run information
		FileManager::writeToFile(visFileName,
			"Platform Reliability = " + to_string(platformReliability) +
			"  Platfrom Visable = " + to_string(platformVisable) +
			"  MarkerCount = " + to_string(markerCount) +
			"  MarkerSpacing = " + to_string(markerSpacing));
		// save world size
		FileManager::writeToFile(visFileName, "worldSize " + to_string(worldRadius));
		// save platform size
		FileManager::writeToFile(visFileName, "platformSize " + to_string(platformSize));
	}

	for (int r = 0; r < evaluationsPerGeneration; r++) {

		// if this is the first eval, or platfrom reliability is less then 2 (i.e. change atleast between every eval)
		if (platformReliability < 2 || r == 0) {
			platformCenter = turnTable.movePoint(worldCenter,
				Random::getDouble(platformAngleMin, platformAngleMax),
				Random::getDouble(platformOffsetFromCenterMin, platformOffsetFromCenterMax)*worldRadius);
		}

		// locate markers for this eval
		markerAngles.clear();
		markerLocations.clear();
		double sliceAngle = 360.0 / markerSpacing; // slice up 360 degrees into markerSpacing slices

		for (int i = 0; i < markerCount; i++) {
			double whichSlice = (markerSpacing / markerCount) * i; // which slice is this marker in?
			markerAngles.push_back(Random::getDouble(sliceAngle*whichSlice, sliceAngle * (whichSlice + 1))); // get random value in slice
			//markerAngles.push_back((i*10.0)+180); // get random value in slice
			markerLocations.push_back(turnTable.movePoint(worldCenter, markerAngles.back(), worldRadius)); // collect x,y locations (point on edge of world, determined by angle)
		}

		if (debug > 0) {
			cout << "set up for Repeat: " << r << endl;
			cout << "  currentLocation: ";
			currentLocation.show();
			cout << "  facing: " << to_string(currentFacing) << endl;
			cout << "  turnRate: " << to_string(turnRate) << endl;
			cout << "  moveRate:" << to_string(moveRate) << endl;
			cout << "    marker angles: " << markerAngles.size() << endl;
			for (size_t i = 0; i < markerAngles.size(); i++) {
				cout << i << "      fixed: " << markerAngles[i] << "  distance from center: " << markerLocations[i].dist(worldCenter) << "  location: ";
				markerLocations[i].show();
			}
		}

		// place organism in world (between angles (agentAngleMin, agentAngleMax) and distance from center as a ratio of worldRadius * (agentOffsetFromCenterMin, agentOffsetFromCenterMax)
		currentLocation = turnTable.movePoint(worldCenter,
			Random::getDouble(agentAngleMin, agentAngleMax),
			Random::getDouble(agentOffsetFromCenterMin, agentOffsetFromCenterMax)*worldRadius);
		currentFacing = (double)Random::getIndex(360);

		// org starts not currently turning and moving at 0 (or min if min > 0) speed.
		turnRate = 0.0; // not turning
		moveRate = max(0.0, minSpeed); // either not moving or moving at minSpeed

		// reset score for this evaluation
		score = 0;
		findCount = 0;

		// save init visualize data for this eval
		if (visualize) {
			FileManager::writeToFile(visFileName, "reset");
			// save platform location
			FileManager::writeToFile(visFileName, "platformLocation " + to_string(platformCenter.x) + " " + to_string(platformCenter.y));
			// save marker locations
			for (int i = 0; i < (int)markerLocations.size(); i++) {
				FileManager::writeToFile(visFileName, "markerLocation " + to_string(markerLocations[i].x) + " " + to_string(markerLocations[i].y));
			}
			// save current location
			FileManager::writeToFile(visFileName, "jump");
			FileManager::writeToFile(visFileName, "L " + to_string(currentLocation.x) + " " + to_string(currentLocation.y));
			// save current facing
			FileManager::writeToFile(visFileName, "F " + to_string(currentFacing));
		}


		brain->resetBrain();


		int inputCounter; //used while assigning inputs to brain

		for (int t = 0; t < worldUpdates; t++) {

			inputCounter = 0; // reset input counter

			// find angles to each marker
			currentFacingVector = turnTable.lookupOffset(currentFacing); // vector in the facing direction

			vector<double> markerRelativeAngles;
			vector<double> markerRelativeDistances;

			if (platformVisable > 0) {
				if (platformVisable == 2 && findCount != 0) {
					markerRelativeAngles.push_back(0); // after first find, so push in place holder 0s
					markerRelativeDistances.push_back(0);
				}
				else { // either platformVisable = 1 or 2 and this is before first find.
					// if platform visable, then first add angle of platfrom to list of angles that brain will see
					compairVector = platformCenter - currentLocation; // vector to platform relitive to current location
					markerRelativeAngles.push_back(currentFacingVector.angle_between_deg(compairVector)); // append angle between facing vector and vector to platform
					markerRelativeDistances.push_back(compairVector.dist());

				}
			}
			for (int i = 0; i < (int)markerLocations.size(); i++) {
				compairVector = markerLocations[i] - currentLocation; // vector to each marker[i] relitive to current location
				markerRelativeAngles.push_back(currentFacingVector.angle_between_deg(compairVector)); // append angle betwene facing vector and vector to marker[i]
				markerRelativeDistances.push_back(compairVector.dist());
			}

			if (found) { // was platfrom found last update?
				brain->setInput(inputCounter++, 1); // if the marker is in this slice, set input to 1
				//found = false;
			}
			else {
				brain->setInput(inputCounter++, 0); // else set input to 0
			}

			for (size_t i = 0; i < markerRelativeAngles.size(); i++) { // for each tracked angle (markers and maybe platfrom)
				if (bitBrains) {
					int whichOct = (int)(markerRelativeAngles[i] / 45.0); // devide 360 into 8 slices

					for (int j = 0; j < 8; j++) {
						//cout << j << " - " << inputCounter << " ";
						if (j == whichOct) {
							brain->setInput(inputCounter++, 1); // if the marker is in this slice, set input to 1
						}
						else {
							brain->setInput(inputCounter++, 0); // else set input to 0
						}
					}
					// set distance sensor
					if (senseDistances) {
						//cout << i << " + " << inputCounter << " ";
						for (double j = 0.0; j < 8; j++) {
							//cout << " (j / 8.0) " << j / 8 << "  (markerRelativeDistances[i] / (worldRadius*2)) " << markerRelativeDistances[i] / (worldRadius * 2) << "  ";
							//cout << " j: " << j / 8 << " i: " << markerRelativeDistances[i] / (worldRadius * 2) << "  ";
							if ((j / 8.0) < (markerRelativeDistances[i] / (worldRadius * 2))) {
								//cout << " . ";
								brain->setInput(inputCounter++, 1);
							}
							else {
								//cout << " 0 ";
								brain->setInput(inputCounter++, 0); // else set input to 0
							}
						}
					}
					// i.e. for a given marker, if it is at 98 degrees, that would be 3rd [2] slice, so
					// input for that marker would be 0 0 1 0 0 0 0 0 - three markers for example would gnerate 24 inputs
				}
				else { // not bit  brains
				 //cout << "not bitBrain" << endl;
					brain->setInput(inputCounter++, markerRelativeAngles[i]); // input is just angle to each marker
					if (senseDistances) {
						brain->setInput(inputCounter++, markerRelativeDistances[i]); // set input to distance to each marker
					}
				}
			}

			if (debug > 0) {
				cout << "update: " << t << endl;
				cout << "  currentLocation: ";
				currentLocation.show();
				cout << "  facing = " << currentFacing << " vector: ";
				currentFacingVector.show();
				cout << "  turnRate: " << to_string(turnRate) << endl;
				cout << "  moveRate:" << to_string(moveRate) << endl;
				cout << "  platform:" << endl;
				cout << "    fixed: " << platformCenter.angle_between_deg(upVector) << "  relative to current:  " << currentFacingVector.angle_between_deg(platformCenter - currentLocation) << "  distance to current: " << platformCenter.dist(currentLocation) << "  location: ";
				platformCenter.show();
				cout << "  markers: " << markerAngles.size() << endl;
				for (size_t i = 0; i < markerAngles.size(); i++) {
					cout << i << "          fixed: " << markerAngles[i] << "  relative to current:  " << markerRelativeAngles[platformVisable ? i + 1 : i] << "  distance to current: " << markerLocations[i].dist(currentLocation) << "  location: ";
					markerLocations[i].show();
					cout << i << "  check   fixed: " << upVector.angle_between_deg(markerLocations[i]) << "  relative to current:  " << currentFacingVector.angle_between_deg(markerLocations[i] - currentLocation) << "  distance to current: " << markerLocations[i].dist(currentLocation) << "  location: ";
					markerLocations[i].show();
					cout << endl << endl;
				}
			}
			brain->update();

			if (found) { // was platfrom found last update?
				found = false;
				//reset org and place at new random location
				//brain->resetBrain(); // add reset on find parameter (if reset, inlife learning is not possible)
				currentLocation = turnTable.movePoint(worldCenter,
					Random::getDouble(agentAngleMin, agentAngleMax),
					Random::getDouble(agentOffsetFromCenterMin, agentOffsetFromCenterMax)*worldRadius);

				currentFacing = Random::getIndex(360);

				turnRate = 0.0; // not turning
				moveRate = max(0.0, minSpeed); // either not moving or moving at minSpeed

				if (visualize) {
					FileManager::writeToFile(visFileName, "jump");
				}

				if (platformReliability == 0) { // if move platfrom after each find
					platformCenter = turnTable.movePoint(worldCenter,
						Random::getDouble(platformAngleMin, platformAngleMax),
						Random::getDouble(platformOffsetFromCenterMin, platformOffsetFromCenterMax)*worldRadius);
					if (visualize) {
						FileManager::writeToFile(visFileName, "platformLocation " + to_string(platformCenter.x) + " " + to_string(platformCenter.y));
					}
				}
			}
			else {





				if (bitBrains) {
					// if bit brain, output is
					// 00 = do nothing (move minMove)
					// 11 = move farward (move maxMove)
					// 01 = turn left maxTurn
					// 10 = turn right maxTurn

					//turnRate = 0;
					if (brain->readOutput(0) > 0 && brain->readOutput(1) > 0) { // 1,1 move
						moveRate = max(min(moveRate + maxSpeedDelta, maxSpeed), minSpeed);
					}
					else if (brain->readOutput(0) > 0) { // turn more left
						turnRate = maxTurnRate;
					}
					else if (brain->readOutput(1) > 0) { // turn more right
						turnRate = minTurnRate;
					}
					else { // 00 slow down
						moveRate = max(min(moveRate - maxSpeedDelta, maxSpeed), minSpeed);
					}
				}
				else { // not bitBrain
					double speedDelta = brain->readOutput(0);
					if (abs(speedDelta) > maxSpeedDelta) {
						speedDelta = (speedDelta < 0) ? -1.0 * maxSpeedDelta : maxSpeedDelta;
					}
					moveRate = max(min(moveRate + speedDelta, maxSpeed), minSpeed);

					turnRate = brain->readOutput(1);
					turnRate = max(min(turnRate, maxTurnRate), minTurnRate);
				}
				/////cout << "z" << flush;
				// turn
				//if (Global::update > 1125) {
				//	cout << currentFacing << "  " << turnRate << " In MT In MT In MT In MT In MT In MT In MT In MT In MT In MT In MT " << endl;
				//}
				currentFacing = loopModDouble(currentFacing + turnRate, 360.0); // turn by turn rate
				/////cout << "m" << flush;

				// move
				//if (Global::update > 1125) {
				//	currentLocation.show();
				//	cout << currentFacing << "  " << moveRate << endl;
				//}
				Point2d newLocation = turnTable.movePoint(currentLocation, currentFacing, moveRate);
				double newDistFromCenter = worldCenter.dist(newLocation);
				if (newDistFromCenter < worldRadius) { // only move if we will still be in the world
					currentLocation = newLocation;
					//currentLocation.show();
					//cout << "   -> " << worldCenter.dist(newLocation) << endl;
				}
				/////cout << "n" << flush;

				// did you get to the platform?
				if (platformCenter.dist(currentLocation) < platformSize) {
					found = true;
					findCount++;
					score += 100;
					//reset org and place at new random location
					//brain->resetBrain(); // add reset on find parameter (if reset, inlife learning is not possible)

					//currentLocation = turnTable.movePoint(worldCenter,
					//	Random::getDouble(agentAngleMin,agentAngleMax),
					//	Random::getDouble(agentOffsetFromCenterMin, agentOffsetFromCenterMax)*worldRadius);

					//currentFacing = Random::getIndex(360);	

					//turnRate = 0.0; // not turning
					//moveRate = max(0.0, minSpeed); // either not moving or moving at minSpeed

					//if (visualize) {
					//	FileManager::writeToFile(visFileName, "jump");
					//}

					//if (platformReliability == 0) { // if move platfrom after each find
					//	platformCenter = turnTable.movePoint(worldCenter,
					//		Random::getDouble(platformAngleMin, platformAngleMax),
					//		Random::getDouble(platformOffsetFromCenterMin, platformOffsetFromCenterMax)*worldRadius);
					//	if (visualize) {
					//		FileManager::writeToFile(visFileName, "platformLocation " + to_string(platformCenter.x) + " " + to_string(platformCenter.y));
					//	}
					//}
				}
			}

			if (visualize) {
				FileManager::writeToFile(visFileName, "L " + to_string(currentLocation.x) + " " + to_string(currentLocation.y));
				FileManager::writeToFile(visFileName, "F " + to_string(currentFacing));
			}
		}
		org->dataMap.Append("score", score + (2 * worldRadius - (platformCenter.dist(currentLocation))));
		org->dataMap.Append("findCount", findCount);
	}
	//cout << "in test MorrisTestWorld:: Done with evaluateSolo : " << org->ID << endl;
}
