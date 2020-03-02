//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once


#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <cmath>
#include <sstream>
#include <set>
#include <vector>
#include "VectorNd.h"
#include "../../../Utilities/Utilities.h"


class SensorArc {
public:
	class Location {
	public:
		int index;
		int locationID;
		int x;
		int y;
		int blockedIndex;
		int clearIndex;
		double distance;
		bool blockingOnly;
		int locationsCount;

		double minWorkingAngle;
		double maxWorkingAngle;

	};

	std::vector<int> allLocations;
	std::vector<int> edgeLocations;
	std::vector<int> blockingOnlyIndexes;
	std::vector<Location> locationsTree;
	double distanceMin, distanceMax;
	double angle1, angle2;
	bool calculateBlocking;
	int locationsCount;

	inline double getworkingAngle(const double& x, const double& y) {
		double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
		return ((atan2(y, x) * (180 / pi)) + 180);
	}

	// find the angles from origin for all four corners of a 1 x 1 square centered on x,y
	// return the min and max angles (i.e. the arc this square covers)
	// if size > 1 then evaluate locations within size distance and combine arcs
	inline void getWorkingAngles(const double& x, const double& y, double& minWorkingAngle, double& maxWorkingAngle, int size = 1) {
		if (size != 1) {
			std::cout << " in getWorkingAngles : size has not been implemented yet..." << std::endl;
			exit(1);
			// the idea here is to allow for blobing, for locations that are farther away, if they are blocking, they will block more area. This should reduce the size
			// of the trees and allow for farther vision (which is less accurate at distance). i.e. size = some factor of distance.
		}
		double x1 = x - .5;
		double x2 = x + .5;
		double y1 = y - .5;
		double y2 = y + .5;
		double workingAngle1 = getworkingAngle(x1, y1); // find angle to all 4 corners
		double workingAngle2 = getworkingAngle(x1, y2);
		double workingAngle3 = getworkingAngle(x2, y1);
		double workingAngle4 = getworkingAngle(x2, y2);
		//std::cout << x1 << " " << x2 << " " << y1 << " " << y2 << "   " << workingAngle1 << " " << workingAngle2 << " " << workingAngle3 << " " << workingAngle4 << endl;

		minWorkingAngle = std::min(workingAngle1, std::min(workingAngle2, std::min(workingAngle3, workingAngle4)));
		maxWorkingAngle = std::max(workingAngle1, std::max(workingAngle2, std::max(workingAngle3, workingAngle4)));
		if (maxWorkingAngle - minWorkingAngle > 180) { // this space crosses 360/0
			std::vector<double> listOfNums = { workingAngle1, workingAngle2, workingAngle3, workingAngle4 };
			sort(listOfNums.begin(), listOfNums.end());
			minWorkingAngle = listOfNums[1];
			maxWorkingAngle = listOfNums[2];
		}
		//std::cout << x << "," << y << " === " << x1 << "," << y1 360 << "   " << x2 << "," << y2 << "   covers   " << minWorkingAngle << "," << maxWorkingAngle << endl;
	}

	void drawArc() {

		for (int y = -1 * (int)distanceMax; y <= (int)distanceMax; y++) {
			int Y = abs(y);
			if (Y < 10) {
				std::cout << "  " << Y << "  : ";
			}
			else if (Y < 100) {
				std::cout << " " << Y << "  : ";
			}
			else {
				std::cout << Y << "  : ";
			}

			for (int x = -1 * (int)distanceMax; x <= (int)distanceMax; x++) {
				int dist = -1;
				for (int i = 0; i < (int)allLocations.size() / 2; i++) {
					if (allLocations[i * 2] == x && allLocations[(i * 2) + 1] == y) {
						dist = i;
					}
				}
				if (dist == -1) {
					std::cout << " - ";
				}
				else if (dist < 10) {
					std::cout << " " << dist << " ";
				}
				else if (dist < 100) {
					std::cout << dist << " ";
				}
				else {
					std::cout << dist;
				}

			}
			std::cout << "\n" << "\n";
		}

		std::cout << "       ";
		for (int X = -1 * (int)distanceMax; X <= (int)distanceMax; X++) {
			int x = abs(X);
			if (x < 10) {
				std::cout << " " << x << " ";
			}
			else if (x < 100) {
				std::cout << x << " ";
			}
			else {
				std::cout << x;
			}

		}
		std::cout << "\n";
	}

	//return all of the visible arcs between angle1 and angle2 (i.e. removing arcs blocked by blocking locations.
	std::vector<double> makeVisableArcs(std::vector<int> blockingLocations) {

		int blockingCount = blockingLocations.size() / 2;

		std::vector<double> visableArcs, tempBlockedArcs, blockedArcs;

		int search_index;
		double minWorkingAngle, maxWorkingAngle;

		// get list with working angles for all blocking locations
		for (int i = 0; i < blockingCount; i++) {
			getWorkingAngles(blockingLocations[i * 2], blockingLocations[(i * 2) + 1], minWorkingAngle, maxWorkingAngle);
			if (maxWorkingAngle - minWorkingAngle < 180) {
				tempBlockedArcs.push_back(minWorkingAngle);
				tempBlockedArcs.push_back(maxWorkingAngle);
			}
			else {
				tempBlockedArcs.push_back(0);
				tempBlockedArcs.push_back(minWorkingAngle);
				tempBlockedArcs.push_back(maxWorkingAngle);
				tempBlockedArcs.push_back(360);
			}
		}

		// blockingCount now counts number of temp arcs!
		blockingCount = tempBlockedArcs.size() / 2;

		// sort list of blocking working angles by min value
		for (int i = 0; i < blockingCount; i++) {
			search_index = i;
			for (int j = i; j < blockingCount; j++) {
				if (tempBlockedArcs[j * 2] < tempBlockedArcs[search_index * 2]) {
					search_index = j;
				}
			}
			//std::cout << tempBlockedArcs[search_index * 2] << "," << tempBlockedArcs[i * 2] << endl;
			std::swap(tempBlockedArcs[search_index * 2], tempBlockedArcs[i * 2]);
			//std::cout << tempBlockedArcs[search_index * 2] << "," << tempBlockedArcs[i * 2] << endl;
			//std::cout << endl << endl << endl;
			std::swap(tempBlockedArcs[(search_index * 2) + 1], tempBlockedArcs[(i * 2) + 1]);
		}

		//see if any blocking arcs can be combined or removed and define visible arcs
		if (blockingCount == 0) { // nothing blocking
			if (angle1 < angle2) {
				visableArcs = { angle1,angle2 };
			}
			else {
				visableArcs = { 0,angle2,angle1,360 };
			}
		}
		else if (blockingCount == 1) { // there is one blocking arc
			if (angle1 < angle2) {
				if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] > angle2) { // everything is blocked!
					visableArcs.resize(0);
				}
				else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] > angle2) { // angle1 to tempBlockedArcs[0] is visible
					visableArcs = { angle1,tempBlockedArcs[0] };
				}
				else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] < angle2) { // there are two visible arcs
					visableArcs = { angle1,tempBlockedArcs[0],tempBlockedArcs[1],angle2 };
				}
				else if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] < angle2) { // tempBlockedArcs[1] to angle2 is visible
					visableArcs = { tempBlockedArcs[1],angle2 };
				}
				else { // blocking nothing
					visableArcs = { angle1,angle2 };
				}
			}
			else { //(angle1 > angle2){
				if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] < angle1) { // blocking nothing
					visableArcs = { 0,angle2,angle1,360 };
				}
				else if (tempBlockedArcs[0] < angle2 && tempBlockedArcs[1] < angle1) {
					visableArcs = { 0,tempBlockedArcs[0],angle1,360 };
				}
				else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = { 0,angle2,tempBlockedArcs[1],360 };
				}
				else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = { 0,tempBlockedArcs[0],tempBlockedArcs[1],360 };
				}
				std::vector<double> temp = { 0,0,360,360 };
				if (visableArcs == temp) { // if actually empty!
					visableArcs.resize(0);
				}
			}

		}
		else { // there is more then one blocking arc! build blockedArcs
		 //std::cout << "in more then one blocking arc!" << endl;
			blockedArcs.push_back(tempBlockedArcs[0]);

			int j = 1;
			for (int i = 1; i < blockingCount; i = j) {
				while (j < blockingCount && tempBlockedArcs[(i * 2) - 1] >= tempBlockedArcs[(j * 2) + 1]) { // if the max of i - 1 is > the max of j, skip j
					j++;
				}
				if (j == blockingCount) { // i - 1 covers all arcs
					blockedArcs.push_back(tempBlockedArcs[(i * 2) - 1]);// push max of i - 1
				}
				else if (tempBlockedArcs[(i * 2) - 1] >= tempBlockedArcs[(j * 2)]) { // arc j overlaps arc i - 1, new arc is min i - 1 to max j
				 //do nothing, we need to see if j max is the end of an arc, or if it is in j+1
				}
				else { // no overlap, push max of i - 1 (to end last arc, and min of j to start new arc
					blockedArcs.push_back(tempBlockedArcs[(i * 2) - 1]);// push max of i - 1
					blockedArcs.push_back(tempBlockedArcs[(j * 2)]);// push min of j
				}
				j++;

				if (j == blockingCount) { // if we happen to be on the last one, push the last value
					blockedArcs.push_back(tempBlockedArcs[(j * 2) - 1]);// push max of j
				}
			}

			//std::cout << "arcs have been combined!" << endl;

			// blockingCount now counts number elements (not arcs)
			blockingCount = blockedArcs.size();

			int tempVisableArcsCount;
			std::vector<double> tempVisableArcs;
			if (angle1 < angle2) {
				tempVisableArcs = { angle1,angle2 };
				tempVisableArcsCount = 2;
			}
			else {
				tempVisableArcs = { 0,angle2,angle1,360 };
				tempVisableArcsCount = 4;
			}

			//std::cout << "done creating tempVisibleArcs : " << tempVisableArcsCount << endl;

//			for (int i = 0; i < (int)tempVisableArcs.size(); i++){
//				std::cout << tempVisableArcs[i] << " ";
//			}
//			std::cout << endl;
//
//			for (int i = 0; i < (int)blockedArcs.size(); i++){
//				std::cout << blockedArcs[i] << " ";
//			}
//			std::cout << endl;

			// Now, start counters v and b at 0, march though visableArcs and blockedArcs
			int v = 0;
			int b = 0;

			bool inVisArc = false;

			double v1 = tempVisableArcs[v++];
			double v2 = tempVisableArcs[v];

			//  not needed, let the while loop below handle this...
			//			while (b < blockingCount && blockedArcs[b] < v1) {
			//				b++;
			//			}

			if (b == blockingCount || blockedArcs[0] > tempVisableArcs[tempVisableArcsCount - 1]) {
				visableArcs = tempVisableArcs;
			}
			else {

				while (v < tempVisableArcsCount && b < blockingCount) {

					if (!inVisArc) { // if we are between visible arcs
						if (blockedArcs[b] < v1) { // if this b is < the start of the next visible arc, skip it
							b++;
							//std::cout << "+"<<endl;
						}
						else if (blockedArcs[b] == v1 && b % 2 == 0) { // if this b is the start of the next visible arc, skip it and..
							b++;
							if (blockedArcs[b] > v1 && blockedArcs[b] < v2) { // if the next b is inside the next visible arc, this b closes an arc...
								visableArcs.push_back(blockedArcs[b]);// add this as a closer
								inVisArc = true;// this b opens a visible arc
								b++;
								//std::cout << "++"<<endl;
							}
							//std::cout << "+++"<<endl;
						}
						else if (b % 2 == 0) { // if b is in arc, and is a start of a blocking arc
							visableArcs.push_back(v1);// add v1 and set inVisArc true (we are now in an arc
							inVisArc = true;
							//std::cout << "++++"<<endl;
						}
						else if (blockedArcs[b] < v2) { // if b in arc and it is an end, set it (the start of the visible arc is hidden
							visableArcs.push_back(blockedArcs[b]);
							b++;
							inVisArc = true;
							//std::cout << "+++++"<<endl;
						}
						else if (b % 2 == 0) { // if the next blocking arc is past the end of this visible arc, add v1 and v2, the whole arc, and we are still !inVisArc
							visableArcs.push_back(v1);
							visableArcs.push_back(v2);
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
							//std::cout << "++++-+"<<endl;
						}
						else { // this ends the current block ... this whole visual arc is not seen
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
						}
						// << "++++--+"<<endl;
					}
					else { // inVisArc
//						std::cout << "++++---+"<<endl;
//						std::cout << "    " << b << " " << v1 << " " << v2 << endl;
//						std::cout << "    " << blockedArcs[b] << endl;
//						std::cout << endl;
//						for (int i = 0; i < (int)blockedArcs.size(); i++){
//							std::cout << blockedArcs[i] << " ";
//						}
//						std::cout << endl;
//						std::cout << endl;
						if (blockedArcs[b] <= v1) { // if b is not in the next vis arc, skip it
							b++;
							//std::cout << "aaa"<<endl;
						}
						else if (b % 2 == 0 && blockedArcs[b] < v2) { // if b is the start of a blocked arc and in this vis arc, add it
							visableArcs.push_back(blockedArcs[b]);
							b++;
							//std::cout << "bbb"<<endl;
						}
						else if (b % 2 == 1 && blockedArcs[b] < v2) { // if b is the end of a blocked arc and in this vis arc, add it
							visableArcs.push_back(blockedArcs[b]);
							b++;
						}
						else if (b % 2 == 0 && blockedArcs[b] >= v2) { // if b is the start of a blocked arc and past (or at!) the end of this vis arc...
							visableArcs.push_back(v2);// we are in the clear, just add v2...
							inVisArc = false;// we are now between vis arcs
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
								//std::cout << "ccc"<<endl;
							}
							//std::cout << "ddd"<<endl;
						}
						else if (b % 2 == 1 && blockedArcs[b] >= v2) { // if b is the end of a blocked arc and past the end of this vis arc...
							inVisArc = false;// then the last b closed this arc, skip v2. also, we are now between vis arcs
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
								//std::cout << "eee"<<endl;
							}
							//std::cout << "fff"<<endl;
						}
					}
				}
			}

			//std::cout << "merge of tempVis and blocking done" << endl;

			// now check to see if there are any viable arcs left and add if needed
			if (b == blockingCount) {
				if (inVisArc) { // if we are in an arc currently, we need to close it out and advance
					visableArcs.push_back(v2);
					v++;
					if (v < tempVisableArcsCount) { // if there are still arcs, advance and add
						v1 = tempVisableArcs[v++];
						v2 = tempVisableArcs[v];
					}
				}
				while (v < tempVisableArcsCount) { // while there are still arcs, advance and add
					visableArcs.push_back(v1);
					visableArcs.push_back(v2);
					v++;
					if (v < tempVisableArcsCount) {
						v1 = tempVisableArcs[v++];
						v2 = tempVisableArcs[v];
					}

				}
			}
			//std::cout << "check for extra vis arcs done" << endl;

		}
		return (visableArcs);
	}

	// given the x and y of a location (square), is that square visible (in visibleArcs)
	bool isLocationVisible(const std::vector<double>& visableArcs, const int& x, const int& y) {
		double minWorkingAngle, maxWorkingAngle, arc1, arc2;
		getWorkingAngles(x, y, minWorkingAngle, maxWorkingAngle);

		//		std::cout << x << "," << y << "   " << minWorkingAngle << "," << maxWorkingAngle << endl;
		//		for (int i = 0; i < (int)visableArcs.size();i++){
		//			std::cout << visableArcs[i] << " ";
		//		}
		//		std::cout << endl;

		bool visible = false;
		for (int i = 0; i < ((int)visableArcs.size()) / 2; i++) {
			arc1 = visableArcs[i * 2];
			arc2 = visableArcs[(i * 2) + 1];

			if ((minWorkingAngle > arc1 && minWorkingAngle < arc2) || (maxWorkingAngle > arc1 && maxWorkingAngle < arc2)) { // if either of the min or max are in this visible arc...
				visible = true;
			}
			else {
				if (maxWorkingAngle - minWorkingAngle <= 180) { // else if the min and max do not cross 0/360 and contain a visible arc...
					if (minWorkingAngle <= arc1 && maxWorkingAngle >= arc2) {
						visible = true;
					}
				}
				else { // if min and max do cross 360 and...
					if (minWorkingAngle + 360 > arc2 && maxWorkingAngle <= arc1) { // if the max to min + 360 (super arc?) contains a visible arc...
						visible = true;
					}
					else if (minWorkingAngle >= arc2 && maxWorkingAngle - 360 < arc1) { // if the max - 360 to min (super arc in the negative?) contains a visible arc...
						visible = true;
					}
				}
			}
		}
		//std::cout << "   " << visible << endl;
		return (visible);
	}

	// make binary decision tree
	void makeLocationsTree(std::vector<int> blockingLocations, int allLocations_index) {
		//std::cout << "+1+" << endl;
		if (allLocations_index < locationsCount) { // if we have not passed the last element in allLocations...
												   // if not blocked, add current location to locationsTree
			Location newLocation;
			int currentLocationTree_index = ((int)locationsTree.size()); // capture this index, we will need it later
			bool isABlockingOnlyLocation = (calculateBlocking) ? find(blockingOnlyIndexes.begin(), blockingOnlyIndexes.end(), allLocations_index) != blockingOnlyIndexes.end() : false;
			newLocation.index = currentLocationTree_index;
			newLocation.locationID = allLocations_index;
			newLocation.x = allLocations[allLocations_index * 2];
			newLocation.y = allLocations[(allLocations_index * 2) + 1];
			newLocation.distance = sqrt((pow(newLocation.x, 2) + pow(newLocation.y, 2)));
			newLocation.blockingOnly = isABlockingOnlyLocation;
			/////////////////
			locationsTree.push_back(newLocation);
			allLocations_index++;
			int tempAllLocations_index = allLocations_index; // capture this (address of next location) for use in blocking

			// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.clearIndex = -1
			// else newLocation.clearIndex = currentLocationTree_index+1; // this is where to go if this location is clear
			// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location)

			std::vector<double> visableArcs = makeVisableArcs(blockingLocations);

			//std::cout << allLocations.size() << endl;

			while (allLocations_index < locationsCount && // there are still locations
				!isLocationVisible(visableArcs, allLocations[allLocations_index * 2], allLocations[(allLocations_index * 2) + 1])) { // and this location is not blocked
			//calculateBlocking && isABlockingOnlyLocation))) { // or if this location is blocking only, but we are not calculating blocking
				allLocations_index++; // skip this location
				//std::cout << "index = " << allLocations_index << endl;
			}
			//std::cout << "+2+" << endl;

			// we should now either be done, or have a visible location

			if (allLocations_index == locationsCount) {
				locationsTree[currentLocationTree_index].clearIndex = -1; // with this location clear, there are no other clear locations, we are at a leaf node
			}
			else { // else we have a visible location, add it to the tree
				locationsTree[currentLocationTree_index].clearIndex = currentLocationTree_index + 1;
				makeLocationsTree(blockingLocations, allLocations_index);
			}
			//std::cout << "+3+" << endl;

			// once we come back ...

			if (!calculateBlocking) {
				//std::cout << "+XX+" << endl;
				if (allLocations_index < locationsCount) {
					locationsTree[currentLocationTree_index].blockedIndex = currentLocationTree_index + 1;
				}
				else {
					locationsTree[currentLocationTree_index].blockedIndex = -1;
				}
			}
			else { // continue here if we need to precalculate blocking
				  // add this location to blockingLocations
			 //std::cout << "+YY+" << endl;
				blockingLocations.push_back(locationsTree[currentLocationTree_index].x);
				blockingLocations.push_back(locationsTree[currentLocationTree_index].y);

				//std::cout << "here1?" << endl;
				visableArcs = makeVisableArcs(blockingLocations);
				//std::cout << "here2?" << endl;

//				std::cout << locationsTree[currentLocationTree_index].x << "   " << locationsTree[currentLocationTree_index].y << endl;
//				for (auto v : blockingLocations){
//					std::cout << v << " ";
//				}
//				std::cout << endl;
//
//				for (auto v : visableArcs){
//					std::cout << v << " ";
//				}
//				std::cout << endl;
//				std::cout << "+ZZ+" << endl;

				// (yes, this is a repeat)
				// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.blockIndex = -1
				// else newLocation.blockIndex = locationsTree.size(); // this is where to go if this location is blocked, i.e. to the end of the locationTree
				// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location, locationsTree_index)

				allLocations_index = tempAllLocations_index;
				while (allLocations_index < locationsCount && !isLocationVisible(visableArcs, allLocations[allLocations_index * 2], allLocations[(allLocations_index * 2) + 1])) {
					// while there are locations on allLocations and they are not visible...
					allLocations_index++;
				}
				//std::cout << "+AA+" << endl;

				// we should now either be done, or have a visible location

				if (allLocations_index == locationsCount) {
					//std::cout << "+BB+" << endl;
					locationsTree[currentLocationTree_index].blockedIndex = -1; // with this location clear, there are no other clear locations, we are at a leaf node
				}
				else { // else we have a visible location, add it to the tree
				 //std::cout << "+CC+" << endl;
					locationsTree[currentLocationTree_index].blockedIndex = (int)locationsTree.size();
					makeLocationsTree(blockingLocations, allLocations_index);
				}
			}
			//std::cout << "+4+" << endl;

		}
	}

	// create a SensorArc Binary Decision Tree
	// the arc will be from angle1 to angle2 (order matters) and will extend distance from the origin, locations closer then distanceMin are not seen
	// if noBlocking, then
	SensorArc(double _angle1, double _angle2, double _distanceMax, double _distanceMin, bool calculateBlocking) :
		angle1(_angle1), angle2(_angle2), calculateBlocking(calculateBlocking) {
		//std::vector<int> allLocations;

		distanceMax = _distanceMax;
		distanceMin = _distanceMin;

		double distanceMaxSquared = pow(distanceMax, 2);
		double distanceMinSquared = pow(distanceMin, 2);

		double minWorkingAngle, maxWorkingAngle;

		bool check;

		//std::cout << "building sensor (arc: " << angle1 << "," << angle2 << "    distances: " << distanceMin << "," << distanceMax << "  blocking: " << calculateBlocking << ")" << endl;
		if (abs(angle1 - angle2) == 360) { // if arc is exactly 360 degrees
			angle1 = 0;
			angle2 = 360;
		}
		else {
			// conversion to get values between 0 and 360 with 0 degrees up
			angle1 += 90;
			angle2 += 90;
			angle1 = loopModDouble(angle1, 360);
			angle2 = loopModDouble(angle2, 360);
		}

		//std::cout << "adjusted angles " << angle1 << " " << angle2 << endl;
		// first get a list of locations
		//std::cout << "-2-" << endl;

		for (int y = -1 * (int)distanceMax; y <= (int)distanceMax; y++) {
			for (int x = -1 * (int)distanceMax; x <= (int)distanceMax; x++) {
				check = false;

				getWorkingAngles(x, y, minWorkingAngle, maxWorkingAngle);

				// if any corner is in between angles, set check to true
				if (angle1 <= angle2) {
					//std::cout << minWorkingAngle << "  " << maxWorkingAngle << "    " << angle1 << "  " << angle2 << endl;
					if (minWorkingAngle > angle1 && minWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
					//std::cout << "min" << endl;
						check = true;
					}
					else if (maxWorkingAngle > angle1 && maxWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
				 //std::cout << "max" << endl;
						check = true;
					}
					else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if (minWorkingAngle <= angle1 && maxWorkingAngle >= angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location contains the whole visible arc
						//std::cout << "mid" << endl;
							check = true;
						}
					}
					else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle1 || maxWorkingAngle < angle2) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							//std::cout << "outside" << endl;
							check = true;
						}
						else if (minWorkingAngle == angle1 && maxWorkingAngle == angle2) {
							check = true;
						}
					}
				}
				else { //angle1 > angle2
					if ((minWorkingAngle < angle2 || minWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					}
					else if ((maxWorkingAngle < angle2 || maxWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					}
					else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if ((minWorkingAngle <= angle2 && maxWorkingAngle >= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					}
					else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle2 && maxWorkingAngle < angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
						else if (minWorkingAngle == angle2 && maxWorkingAngle == angle1) {
							check = true;
						}
					}
				}

				if (check || (x == 0 && y == 0 && distanceMin == 0)) { // if any of the corners are between the angles, add that point to allLocations
					allLocations.push_back(x);
					allLocations.push_back(y);
				}

			}
		}
		//std::cout << "-3-" << endl;

		locationsCount = allLocations.size() / 2;

		// find out how far away the most distant location is (don't bother taking the sqrt)

		double maxDistance = 0;
		for (int i = 0; i < locationsCount; i++) { // for every location
			if ((pow(allLocations[i * 2], 2) + pow(allLocations[i * 2 + 1], 2)) > maxDistance) {
				maxDistance = pow(allLocations[i * 2], 2) + pow(allLocations[i * 2 + 1], 2); // if the location of j is closer, j is now closest
			}
		}

		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		// THIS CODE IS NOT NEEDED SINCE WE ARE GOING TO USE angle1 and angle2 when determining visible
		//		if (calculateBlocking) {
		//			// Now add the boarder locations - locations adjacent to arc locations and closer then the farthest location
		//
		//			vector<int> offsetsX = { 0, 1, 0, -1 };
		//			vector<int> offsetsY = { 1, 0, -1, 0 };
		//			int testX, testY;
		//			bool includeLocation;
		//			//std::cout << "-4-" << endl;
		//
		//			for (auto f : allLocations) {
		//				std::cout << f << " ";
		//			}
		//			std::cout << endl;
		//			edgeLocations.clear();
		//			for (int i = 0; i < locationsCount; i++) { // for every location
		//				for (int l = 0; l < 4; l++) {
		//					testX = allLocations[i * 2] + offsetsX[l];
		//					testY = allLocations[i * 2 + 1] + offsetsY[l];
		//					//std::cout << "+++++ " << testX <<","<< testY << endl;
		//					//std::cout << pow(testX, 2) + pow(testY, 2) <<  " " << maxDistance << " " << distanceMin << endl;
		//					includeLocation = true;
		//					if (pow(testX, 2) + pow(testY, 2) >= maxDistance || pow(testX, 2) + pow(testY, 2) < distanceMin) {
		//						includeLocation = false;
		//					} else {
		//						for (int j = 0; j < locationsCount; j++) { // for every location in allLocations
		//							if (testX == allLocations[j * 2] && testY == allLocations[j * 2 + 1]) {
		//								//std::cout << testX << "," << testY << "   " << allLocations[j * 2] << "," << allLocations[j * 2 + 1] << endl;
		//								includeLocation = false;
		//								//j = locationsCount;
		//							}
		//						}
		//						for (int j = 0; j < ((int) edgeLocations.size()) / 2; j++) { // for every location in extraLocations - the locations we are adding to insure proper blocking)
		//							if (testX == edgeLocations[j * 2] && testY == edgeLocations[j * 2 + 1]) {
		//								includeLocation = false;
		//								//j = (int) edgeLocations.size();
		//							}
		//						}
		//					}
		//					if (includeLocation) {
		//						edgeLocations.push_back(testX);
		//						edgeLocations.push_back(testY);
		//						//std::cout << "   ++ " << testX << "," << testY << endl;
		//					}
		//				}
		//			}
		//			//std::cout << "-5-" << endl;
		//
		//			//combine edge and all locations into all
		//			//std::cout << allLocations.size() / 2 << endl;
		//			//std::cout << edgeLocations.size() / 2 << endl;
		//
		//			allLocations.insert(allLocations.end(), edgeLocations.begin(), edgeLocations.end());
		//
		//		}
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////

		locationsCount = allLocations.size() / 2;
		//std::cout << locationsCount << endl;

		// now we have a list of locations, sort the list
		int search_index;

		for (int i = 0; i < locationsCount; i++) { // for every location
			search_index = i;
			for (int j = i + 1; j < locationsCount; j++) { // for every location after i
				if ((pow(allLocations[j * 2], 2) + pow(allLocations[j * 2 + 1], 2)) < (pow(allLocations[search_index * 2], 2) + pow(allLocations[search_index * 2 + 1], 2))) {
					search_index = j; // if the location of j is closer, j is now closest
				}
			}
			// swap closest pair to current
			std::swap(allLocations[i * 2], allLocations[search_index * 2]);
			std::swap(allLocations[i * 2 + 1], allLocations[search_index * 2 + 1]);
		}
		//std::cout << "-6-" << endl;

		// make blockingOnlyIndexes

		for (int i = 0; i < locationsCount; i++) { // for every location
			for (int j = 0; j < (int)edgeLocations.size() / 2; j++) { // for all edge locations
				if (allLocations[i * 2] == edgeLocations[j * 2] && allLocations[i * 2 + 1] == edgeLocations[j * 2 + 1]) {
					// this is an edge locations, add i (this index) to blockingOnlyIndexes
					//std::cout << "----   " << allLocations[i * 2] << "," << allLocations[i * 2 + 1] << endl;
					blockingOnlyIndexes.push_back(i);
				}
			}
		}
		//std::cout << "-7-" << endl;

		// make a new list of indexes for the edge locations

		// test to show allLocations and edgeLocations
		/*
		for (int y = -1 * (int)distanceMax; y <= (int)distanceMax; y++) {
			for (int x = -1 * (int)distanceMax; x <= (int)distanceMax; x++) {
				int type = 0;
				for (int i = 0; i < locationsCount; i++) {
					if (x == allLocations[i * 2] && y == allLocations[i * 2 + 1]) {
						type = 1;
					}
				}
				for (int i = 0; i < ((int)edgeLocations.size()) / 2; i++) { // for every location
					if (x == edgeLocations[i * 2] && y == edgeLocations[i * 2 + 1]) {
						type = 2;
					}
				}
				if (type == 0) {
					std::cout << "  ";
				}
				else if (type == 1) {
					std::cout << " +";
				}
				else {
					std::cout << " -";
				}
			}
			std::cout << endl;
		}
		*/
		// now build the locationsTree!

		int allLocations_index = 0;

		std::vector<int> blockingLocations;

		locationsTree.clear();
		blockingLocations.clear();

		//int allLocationsSize = allLocations.size() / 2;

		makeLocationsTree(blockingLocations, allLocations_index);
		//std::cout << "-8-" << endl;

	}

	int advanceIndex(int currentIndex, bool blocked = false) {
		if (blocked) {
			currentIndex = locationsTree[currentIndex].blockedIndex;
		}
		else {
			currentIndex = locationsTree[currentIndex].clearIndex;
		}
		return currentIndex;
	}

	int cX(const int& currentIndex) {
		return locationsTree[currentIndex].x;
	}

	int cY(const int& currentIndex) {
		return locationsTree[currentIndex].y;
	}

	bool isBlockingOnly(const int& currentIndex) {
		return locationsTree[currentIndex].blockingOnly;
	}
};

class Sensor {
public:
	int resolution; // how many angles to precalculate

	std::map<int, std::shared_ptr<SensorArc>> angles;

	Sensor() {
		resolution = 0;
	}

	Sensor(double angle1, double angle2, double distanceMax, double distanceMin, int _resolution, bool calculateBlocking) {
		resolution = _resolution;
		double resolutionOffset = 360.0 / resolution;
		//std::cout << "building sensor (arc: " << angle1 << "," << angle2 << "    distances: " << distanceMin << "," << distanceMax << "     resolution: " << resolution << "  blocking: " << calculateBlocking << ")" << endl;
		for (int i = 0; i < resolution; i++) {
			//std::cout << "   building arc # " << i << endl;
			angles[i] = std::make_shared<SensorArc>((i * resolutionOffset) + angle1, (i * resolutionOffset) + angle2, distanceMax, distanceMin, calculateBlocking);
		}
	}

	void senseTotals(Vector2d<int>& worldgrid, int& orgx, int& orgy, int& orgf, std::vector<int>& values, int blocker = -1, bool wrap = false) {

		bool blocked = false;
		int currentIndex = 0;

		fill(values.begin(), values.end(), 0);

		int currentX; // = loopMod(this->angles[orgf]->cX(currentIndex) + orgx,worldgrid.x());
		int currentY; // = loopMod(this->angles[orgf]->cY(currentIndex) + orgy,worldgrid.y());

		if (wrap) {

			while (currentIndex != -1) {
				//std::cout << currentIndex << "  :  " << this->angles[orgf]->locationsTree[currentIndex].locationID << "  :  " << this->angles[orgf]->cX(currentIndex) << "," << this->angles[orgf]->cY(currentIndex) << "  "
				//<< this->angles[orgf]->cX(currentIndex) + orgx << "," << this->angles[orgf]->cY(currentIndex) + orgy << "  <>  " << worldgrid(this->angles[orgf]->cX(currentIndex) + orgx, this->angles[orgf]->cY(currentIndex) + orgy) << endl;
				currentX = loopMod(this->angles[orgf]->cX(currentIndex) + orgx, worldgrid.x());
				currentY = loopMod(this->angles[orgf]->cY(currentIndex) + orgy, worldgrid.y());
				blocked = worldgrid(currentX, currentY) == blocker;
				//if (!this->angles[orgf]->isBlockingOnly(currentIndex)) {
				values[worldgrid(currentX, currentY)]++;
				//} //else {
				//std::cout << " isBlockingOnly" << endl;
				//}
				//std::cout << blocker << "    " << ((blocked) ? "blocked" : "open") << endl;
				currentIndex = this->angles[orgf]->advanceIndex(currentIndex, blocked);
				//std::cout << " nextIndex: " << currentIndex << endl;
			}
		}
		else {
			while (currentIndex != -1) {
				//std::cout << currentIndex << "  :  " << this->angles[orgf]->locationsTree[currentIndex].locationID << "  :  " << this->angles[orgf]->cX(currentIndex) << "," << this->angles[orgf]->cY(currentIndex) << "  "
				//<< this->angles[orgf]->cX(currentIndex) + orgx << "," << this->angles[orgf]->cY(currentIndex) + orgy << "  <>  " << worldgrid(this->angles[orgf]->cX(currentIndex) + orgx, this->angles[orgf]->cY(currentIndex) + orgy) << endl;
				currentX = this->angles[orgf]->cX(currentIndex) + orgx, worldgrid.x();
				currentY = this->angles[orgf]->cY(currentIndex) + orgy, worldgrid.y();
				blocked = worldgrid(currentX, currentY) == blocker;
				//if (!this->angles[orgf]->isBlockingOnly(currentIndex)) {
				values[worldgrid(currentX, currentY)]++;
				//} //else {
				//std::cout << " isBlockingOnly" << endl;
				//}
				//std::cout << blocker << "    " << ((blocked) ? "blocked" : "open") << endl;
				currentIndex = this->angles[orgf]->advanceIndex(currentIndex, blocked);
				//std::cout << " nextIndex: " << currentIndex << endl;
			}
		}
	}

};
