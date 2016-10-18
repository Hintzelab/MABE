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

	inline double getworkingAngle(const double& x, const double& y) {
		double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
		return ((atan2(y, x) * (180 / pi)) + 180);
	}

	// find the angles from origin for all four corners of a 1 x 1 square centered on x,y
	// return the min and max angles (i.e. the arc this square covers)
	inline void getWorkingAngles(const double& x, const double& y, double& minWorkingAngle, double& maxWorkingAngle) {
		double x1 = x - .5;
		double x2 = x + .5;
		double y1 = y - .5;
		double y2 = y + .5;
		double workingAngle1 = getworkingAngle(x1, y1); // find angle to all 4 corners
		double workingAngle2 = getworkingAngle(x1, y2);
		double workingAngle3 = getworkingAngle(x2, y1);
		double workingAngle4 = getworkingAngle(x2, y2);
		//cout << x1 << " " << x2 << " " << y1 << " " << y2 << "   " << workingAngle1 << " " << workingAngle2 << " " << workingAngle3 << " " << workingAngle4 << endl;

		minWorkingAngle = min(workingAngle1, min(workingAngle2, min(workingAngle3, workingAngle4)));
		maxWorkingAngle = max(workingAngle1, max(workingAngle2, max(workingAngle3, workingAngle4)));
	}

	void drawArc(const double& distanceMax, vector<int> allLocations) {

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

		if (angle1 == -180 && angle2 == 180) {
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
					//cout << minWorkingAngle << "  " << maxWorkingAngle << "    " << angle1 << "  " << angle2 << endl;
					if (minWorkingAngle > angle1 && minWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
					//cout << "min" << endl;
						check = true;
					} else if (maxWorkingAngle > angle1 && maxWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
					//cout << "max" << endl;
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if (minWorkingAngle < angle1 && maxWorkingAngle > angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							//cout << "mid" << endl;
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle1 || maxWorkingAngle < angle2) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							//cout << "outside" << endl;
							check = true;
						}
					}
				} else { //angle1 > angle2
					if ((minWorkingAngle < angle2 || minWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if ((maxWorkingAngle < angle2 || maxWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if ((minWorkingAngle < angle2 && maxWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle2 || maxWorkingAngle < angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
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
		int search_index;

		for (int i = 0; i < locationsCount; i++) { // for every location
			search_index = i;
			for (int j = i + 1; j < locationsCount; j++) { // for every location after i
				if ((pow(allLocations[j * 2], 2) + pow(allLocations[j * 2 + 1], 2)) < (pow(allLocations[search_index * 2], 2) + pow(allLocations[search_index * 2 + 1], 2))) {
					search_index = j; // if the location of j is closer, j is now closest
				}
			}
			// swap closest pair to current
			swap(allLocations[i * 2], allLocations[search_index * 2]);
			swap(allLocations[i * 2 + 1], allLocations[search_index * 2 + 1]);
		}

		drawArc(distanceMax, allLocations);

		// now build the locationsTree!

		int allLocations_index = 0;

		vector<int> blockingLocations; // list with x and y blocked locations
		// each location to be tested will be tested vs this list

		locationsTree.clear();

		blockingLocations = {0,-1,2,0,0,1,10,-8,-1,1};

		int blockingCount = blockingLocations.size() / 2;

		drawArc(distanceMax, blockingLocations);

		//makeVisableArcs(distanceMax,allLocations,blockingLocations);

		vector<double> visableArcs, tempBlockedArcs, blockedArcs;

		// get list with working angles for all blocking locations
		for (int i = 0; i < blockingCount; i++) {
			getWorkingAngles(blockingLocations[i * 2], blockingLocations[(i * 2) + 1], minWorkingAngle, maxWorkingAngle);
			if (maxWorkingAngle - minWorkingAngle < 180) {
				tempBlockedArcs.push_back(minWorkingAngle);
				tempBlockedArcs.push_back(maxWorkingAngle);
			} else {
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
				if (tempBlockedArcs[j * 2] < tempBlockedArcs[i * 2]) {
					search_index = j;
				}
			}
			swap(tempBlockedArcs[search_index * 2], tempBlockedArcs[i * 2]);
			swap(tempBlockedArcs[(search_index * 2) + 1], tempBlockedArcs[(i * 2) + 1]);
		}

		//see if any blocking arcs can be combined or removed and define visable arcs
		if (blockingCount == 0) { // nothing blocking
			if (angle1 < angle2) {
				visableArcs = {angle1,angle2};
			} else {
				visableArcs = {0,angle2,angle1,360};
			}
		} else if (blockingCount == 1) { // there is one blocking arc
			if (angle1 < angle2) {
				if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] > angle2) { // everything is blocked!
					visableArcs.resize(0);
				} else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] > angle2) { // angle1 to tempBlockedArcs[0] is visible
					visableArcs = {angle1,tempBlockedArcs[0]};
				} else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] < angle2) { // there are two visible arcs
					visableArcs = {angle1,tempBlockedArcs[0],tempBlockedArcs[1],angle2};
				} else if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] < angle2) { // tempBlockedArcs[1] to angle2 is visible
					visableArcs = {tempBlockedArcs[1],angle2};
				} else { // blocking nothing
					visableArcs = {angle1,angle2};
				}
			} else { //(angle1 > angle2){
				if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] < angle1) { // blocking nothing
					visableArcs = {0,angle2,angle1,360};
				} else if (tempBlockedArcs[0] < angle2 && tempBlockedArcs[1] < angle1) {
					visableArcs = {0,tempBlockedArcs[0],angle1,360};
				} else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = {0,angle2,tempBlockedArcs[1],360};
				} else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = {0,tempBlockedArcs[0],tempBlockedArcs[1],360};
				}
				vector<double> temp = {0,0,360,360};
				if (visableArcs == temp) { // if actually empty!
					visableArcs.resize(0);
				}
			}

		} else { // there is more then one blocking arc! build blockedArcs
			blockedArcs.push_back(tempBlockedArcs[0]);

			int j = 1;
			for (int i = 1; i < blockingCount; i = j) {
				while (j < blockingCount && tempBlockedArcs[(i*2)-1] >= tempBlockedArcs[(j*2)+1]) { // if the max of i - 1 is > the max of j, skip j
					j++;
				}
				if (j == blockingCount) { // i - 1 covers all arcs
					blockedArcs.push_back(tempBlockedArcs[(i*2)-1]);// push max of i - 1
				} else if (tempBlockedArcs[(i*2)-1] >= tempBlockedArcs[(j*2)]) { // arc j overlaps arc i - 1, new arc is min i - 1 to max j
					//do nothing, we need to see if j max is the end of an arc, or if it is in j+1
				} else { // no overlap, push max of i - 1 (to end last arc, and min of j to start new arc
					blockedArcs.push_back(tempBlockedArcs[(i*2)-1]);// push max of i - 1
					blockedArcs.push_back(tempBlockedArcs[(j*2)]);// push min of j
				}
				j++;

				if (j == blockingCount) { // if we happen to be on the last one, push the last value
					blockedArcs.push_back(tempBlockedArcs[(j*2)-1]);// push max of j
				}
			}

			for (auto v : blockedArcs) {
				cout << v << "  ";
			}
			cout << endl;

			// blockingCount now counts number of arcs!
			blockingCount = blockedArcs.size() / 2;

			int tempVisableArcsCount;
			vector<double> tempVisableArcs;
			if (angle1 < angle2) {
				tempVisableArcs = {angle1,angle2};
				tempVisableArcsCount = 1;
			} else {
				tempVisableArcs = {0,angle2,angle1,360};
				tempVisableArcsCount = 2;
			}

			// Now, start counters v and b at 0, march though visableArcs and blockedArcs
			int v = 0;
			int b = 0;

			while (blockedArcs[(b*2) + 1] < tempVisableArcs[v*2]) { // get past any blocking arcs before the visible arcs
				b+=2;
			}
			while (v < tempVisableArcsCount || b < blockingCount) {
				if(blockedArcs[b*2] < tempVisableArcs[v*2]) { // if the min of the blocked arc is < the min of the visible arc then start at the max of the blocked arc
					visableArcs.push_back(blockedArcs[b*2]+1);
					b+=2;
				} else if (blockedArcs[b*2] < tempVisableArcs[(v*2)+1]) { // else if the min of the blocked arc is in the visible arc, start visible arc and then put in blockd arc
					visableArcs.push_back(tempVisableArcs[v*2]);
					visableArcs.push_back(blockedArcs[b*2]);
					visableArcs.push_back(blockedArcs[b*2]+1);
					b+=2;
				} else { // min of blocked arc is past max of visible arc. push max of visible arc
					visableArcs.push_back(tempVisableArcs[(v*2)+1]);
					v+=2;
				}
				if (v < tempVisableArcsCount) {
					while (b < blockingCount && blockedArcs[(b*2) + 1] < tempVisableArcs[v*2]) { // get past any blocking arcs before the visible arcs
						b+=2;
					}
				}
			}

			for (auto v : tempVisableArcs) {
				cout << v << "  ";
			}
			cout << endl;

			for (auto v : visableArcs) {
				cout << v << "  ";
			}
			cout << endl;

//			int visableArcsCount = 1;

		}

		makeLocationsTree(allLocations, blockingLocations, allLocations_index);
	}

	void makeLocationsTree(const vector<int>& allLocations, vector<int> blockedLocations, int allLocations_index) {
		if (allLocations_index < (int) allLocations.size()) { // if we have not passed the last element in allLocations...
				// if not blocked, add current location to locationsTree
			Location newLocation;
			newLocation.x = allLocations[allLocations_index * 2];
			newLocation.y = allLocations[allLocations_index * 2 + 1];
			newLocation.distance = sqrt((pow(newLocation.x, 2) + pow(newLocation.y, 2)));

			//int currentLocationTree_index = ((int) locationsTree.size()); // capture this index, we will need it later

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
