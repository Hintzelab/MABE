//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldUtilities__
#define __BasicMarkovBrainTemplate__WorldUtilities__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <vector>

#include "Utilities.h"

using namespace std;

class SensorArc {
public:
	class Location {
	public:
		int x;
		int y;
		int blockedIndex;
		int clearIndex;
		double distance;

		double minWorkingAngle;
		double maxWorkingAngle;

	};

	vector<Location> locationsTree;
	float workingAngle;

	// find the angles from origin for all four corners of a 1 x 1 square centered on x,y
	// return the min and max angles (i.e. the arc this square covers)
	inline void getWorkingAngles(const int& x, const int& y, double& minWorkingAngle, double& maxWorkingAngle) {
		double x1 = x - .5;
		double x2 = x + .5;
		double y1 = y - .5;
		double y2 = y + .5;
		double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
		double workingAngle1 = (atan2(y1, x1) * (180 / pi)) + 180; // find angle to all 4 corners
		double workingAngle2 = (atan2(y2, x1) * (180 / pi)) + 180;
		double workingAngle3 = (atan2(y1, x2) * (180 / pi)) + 180;
		double workingAngle4 = (atan2(y2, x2) * (180 / pi)) + 180;

		minWorkingAngle = min(workingAngle1, min(workingAngle2, min(workingAngle3, workingAngle4)));
		maxWorkingAngle = max(workingAngle1, max(workingAngle2, max(workingAngle3, workingAngle4)));
	}

	// create a SensorArc Binary Decision Tree
	// the arc will be from angle1 to angle2 (order matters) and will extend distance from the origin
	SensorArc(double angle1, double angle2, double distanceMax, double distanceMin) {
		vector<int> allLocations;
		int locationsCount;

		double distanceMaxSquared = pow(distanceMax, 2);
		double distanceMinSquared = pow(distanceMin, 2);

		double minWorkingAngle, maxWorkingAngle;

		bool check;

		if (angle1 == -180 && angle2 == 180){
			angle1 = 0;
			angle2 = 360;
		} else {
			// conversion to get values between 0 and 360 with 0 degrees up
			angle1 += 180 - 90;
			angle2 += 180 - 90;
			angle1 = loopMod(angle1, 360);
			angle2 = loopMod(angle2, 360);
		}

		// first get a list of locations

		for (int y = -1 * distanceMax; y <= distanceMax; y++) {
			for (int x = -1 * distanceMax; x <= distanceMax; x++) {
				check = false;

				getWorkingAngles(x, y, minWorkingAngle, maxWorkingAngle);

				// if any corner is in between angles, set check to true
				if (angle1 <= angle2) {
					if (minWorkingAngle >= angle1 && minWorkingAngle <= angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if (maxWorkingAngle >= angle1 && maxWorkingAngle <= angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if (minWorkingAngle <= angle1 && maxWorkingAngle >= angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle >= angle1 || maxWorkingAngle <= angle2) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					}
				} else { //angle1 > angle2
					if ((minWorkingAngle <= angle2 || minWorkingAngle >= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if ((maxWorkingAngle <= angle2 || maxWorkingAngle >= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if ((minWorkingAngle <= angle2 && maxWorkingAngle >= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle >= angle2 || maxWorkingAngle <= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					}
				}

				if (check) { // if any of the corners are between the angles, add that point to allLocations
					allLocations.push_back(x);
					allLocations.push_back(y);
				}

			}
		}

		locationsCount = allLocations.size() / 2;

		// now we have a list of locations, sort the list
		int closest_index;

		for (int i = 0; i < locationsCount; i++) { // for every location
			closest_index = i;
			for (int j = i + 1; j < locationsCount; j++) { // for every location after i
				if ((pow(allLocations[j * 2], 2) + pow(allLocations[j * 2 + 1], 2)) < (pow(allLocations[closest_index * 2], 2) + pow(allLocations[closest_index * 2 + 1], 2))) {
					closest_index = j; // if the location of j is closer, j is now closest
				}
			}
			// swap closest pair to current
			swap(allLocations[i * 2], allLocations[closest_index * 2]);
			swap(allLocations[i * 2 + 1], allLocations[closest_index * 2 + 1]);
		}

		for (int y = -1 * distanceMax; y <= distanceMax; y++) {
			int Y = abs(y);
			if (Y < 10) {
				cout << "  " << Y << "  : ";
			} else if (Y < 100) {
				cout << " " << Y << "  : ";
			} else {
				cout << Y << "  : ";
			}

			for (int x = -1 * distanceMax; x <= distanceMax; x++) {
				int dist = -1;
				for (int i = 0; i < (int) allLocations.size() / 2; i++) {
					if (allLocations[i * 2] == x && allLocations[(i * 2) + 1] == y) {
						dist = i;
					}
				}
				if (dist == -1) {
					cout << " - ";
				} else if (dist < 10) {
					cout << " " << dist << " ";
				} else if (dist < 100) {
					cout << dist << " ";
				} else {
					cout << dist;
				}

			}
			cout << endl << endl;
		}

		cout << "       ";
		for (int X = -1 * distanceMax; X <= distanceMax; X++) {
			int x = abs(X);
			if (x < 10) {
				cout << " " << x << " ";
			} else if (x < 100) {
				cout << x << " ";
			} else {
				cout << x;
			}

		}
		cout << endl;

		// now build the locationsTree!

		int allLocations_index = 0;

		vector<int> blockedLocations; // list with x and y blocked locations
		// each location to be tested will be tested vs this list

		locationsTree.clear();

		makeLocationsTree(allLocations, blockedLocations, allLocations_index);
	}

	void makeLocationsTree(const vector<int>& allLocations, vector<int> blockedLocations, int allLocations_index) {
		if (allLocations_index < (int) allLocations.size()) { // if we have not passed the last element in allLocations...
				// if not blocked, add current location to locationsTree
			Location newLocation;
			newLocation.x = allLocations[allLocations_index * 2];
			newLocation.y = allLocations[allLocations_index * 2 + 1];
			newLocation.distance = sqrt((pow(newLocation.x, 2) + pow(newLocation.y, 2)));

			int currentLocationTree_index = ((int) locationsTree.size()); // capture this index, we will need it later

			locationsTree.push_back(newLocation);

			// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.clearIndex = -1
			// else newLocation.clearIndex = currentLocationTree_index+1; // this is where to go if this location is clear
			// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location)

			// once we come back, if there are still locations in allLocations

			// add this location to blocked
			// (yes, this is a repeat)
			// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.blockIndex = -1
			// else newLocation.blockIndex = locationsTree.size(); // this is where to go if this location is blocked, i.e. to the end of the locationTree
			// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location, locationsTree_index)

		}
	}

};

class sensorArray {

	vector<SensorArc> sensors;

};

#endif // __BasicMarkovBrainTemplate__WorldUtilities__
