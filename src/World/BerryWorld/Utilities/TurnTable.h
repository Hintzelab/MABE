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

#include <vector>
#include <iostream>
#include "PointNd.h"

// creates a look up table of directions related to directions. The default is 360 degrees
// numDirections can be used to determine what value will be associated with 360 degrees
// numSlices determines the resolution of the lookup table (i.e. directions will be rounded to the nearest slice value)
class TurnTable {
public:
	double numDirections; // i.e. what is 360 degrees (for berry world, directions = 8 (i.e. 0,45,90,135,180,225,270,315))
						  //                                            so direction 4 would return the offset for 180 degrees
	double numSlices;     // how many entires into the lookup table

	double degPerSlice;

	std::vector<Point2d> moveDeltas; // lookup table with offset values keyed by slice index

	void show() {
		std::cout << "showing values in TurnTable:" << "\n";
		for (int i = 0; i < (int)numSlices; i++) {
			std::cout << "slice: " << i << " direction: " << (i / numSlices) * numDirections << " degree: " << i * degPerSlice << "  -> ";
			moveDeltas[i].show();
		}
	}

	void setup(double _numDirections, double _numSlices) {
		numDirections = _numDirections;
		numSlices = _numSlices;

		degPerSlice = (360.0 / numSlices);
		moveDeltas.clear();
		for (double s = 0; s < 360; s += degPerSlice) {
			double x = sin(s / 57.295779513225);
			double y = cos(s / 57.295779513225);
			if (abs(x) < .00001) {
				x = 0;
			}
			if (abs(y) < .00001) {
				y = 0;
			}
			moveDeltas.push_back(Point2d(x, y));
		}
	}

	// create a table with numSlice offset entries. numDirections is saved to be used in lookup operations
	TurnTable(double _numDirections = 360, double _numSlices = 360) {
		setup(_numDirections, _numSlices);
	}

	// pull the Point2d closest to direction in moveDeltas
	Point2d lookupOffset(double direction) {
		return moveDeltas[(int)((loopModDouble(direction, numDirections) / numDirections) * numSlices)];
	}

	// translate a point at 'source' in the direction indicated by 'direction' 'distance' units 
	Point2d movePoint(Point2d source, double direction, double distance = 1) {
		return (source + lookupOffset(direction).scale(distance));
	}
};


// test code:
/*
cout << "test!" << endl;
TurnTable turnTable(360,360000);
//turnTable.show();
TurnTable turnTable8(8,8);
//turnTable8.show();

Point2d p1(10.3, -14);
p1.show();

cout << " ----------------" << endl;

turnTable.movePoint(-89, p1, 1).show();
turnTable.movePoint(361, p1, 1).show();

cout << " ----------------" << endl;

turnTable8.movePoint(1, p1, 1).show();
turnTable8.movePoint(2, p1, 1).show();
turnTable8.movePoint(3, p1, 1).show();
turnTable8.movePoint(4, p1, 1).show();
turnTable8.movePoint(5, p1, 1).show();
turnTable8.movePoint(6, p1, 1).show();
turnTable8.movePoint(7, p1, 1).show();
turnTable8.movePoint(8, p1, 1).show();

exit(1);
*/
