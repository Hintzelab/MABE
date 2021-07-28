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

#include <World/AbstractWorld.h> // AbstractWorld defines all the basic function templates for worlds
#include <string>
#include <memory> // shared_ptr
#include <map>

#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

using std::shared_ptr;
using std::string;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::to_string;

#include <Analyze/neurocorrelates.h>
#include <Analyze/brainTools.h>
#include <Analyze/fragmentation.h>
#include <Analyze/smearedness.h>

class PathFollowWorld : public AbstractWorld {

public:
	// ParameterLink (PL) declarations
    static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL; // number of times to test every agent

    static shared_ptr<ParameterLink<int>> extraStepsPL; // how many extra steps do the agents get beyond the minimum neccasary
    static shared_ptr<ParameterLink<double>> emptySpaceCostPL; // cost for landing on an empty location
    static shared_ptr<ParameterLink<bool>> clearVistedPL; // are location input values altered when visited

    static shared_ptr<ParameterLink<int>> turnSymbolsCountPL; // turn symbols will be values from 1 to signValueMax
    static shared_ptr<ParameterLink<int>> randomTurnSymbolsPL; // if false, alway use 1 for left and 2 for right

    static shared_ptr<ParameterLink<std::string>> inputModePL; // single : 1 input: -1 (off), 0(forward), or [1,signValueMax](turn)
                                                               // mixed:   4 inputs: offPathBit,onPathBit,(0(not turn), or [1,signValueMax](turn))
                                                               // binary:  3+ inputs: offPathBit,onPathBit,onTurnBit, bits for turn(0 if not turn)

    static shared_ptr<ParameterLink<double>> swapSymbolsAfterPL; // after this % of the map, swap turn signals

    static shared_ptr<ParameterLink<std::string>> mapNamesPL; // names of map files
    static shared_ptr<ParameterLink<bool>> addFlippedMapsPL; // add a flipped version of each map


    static shared_ptr<ParameterLink<bool>> saveFragOverTimePL;
    static shared_ptr<ParameterLink<bool>> saveBrainStructureAndConnectomePL;
    static shared_ptr<ParameterLink<bool>> saveStateToStatePL;
    static shared_ptr<ParameterLink<bool>> save_R_FragMatrixPL;
    static shared_ptr<ParameterLink<bool>> saveFlowMatrixPL;
    static shared_ptr<ParameterLink<bool>> saveStatesPL;
    static shared_ptr<ParameterLink<bool>> saveVisualPL;


    // a local variable used for faster access to the ParameterLink values
    int evaluationsPerGeneration;

    int extraSteps;
    double emptySpaceCost;
    bool clearVisted;

    int turnSymbolsCount;
    int randomTurnSymbols;
    int signalListCount;

    std::string inputMode;

    bool swapSymbolsAfter;

    std::vector<string> mapNames;
    bool addFlippedMaps;
    
    bool saveFragOverTime;
    bool saveBrainStructureAndConnectome;
    bool saveStateToState;
    bool save_R_FragMatrix;
    bool saveFlowMatrix;
    bool saveStates;
    bool saveVisual;

    // point2d defines a 2d vector with addtion, subtraction, dot/scalar product(*)
    // and cross product
    // also included are distance functions and functions which return the signed
    // angle between 2 point2ds (relitive to 0,0)

    class Point2d { // a point class, useful for 2d worlds
    public:
        double x;
        double y;

        const double pi = atan(1.0) * 4;

        Point2d() {
            x = 0.0;
            y = 0.0;
        }
        Point2d(double _x, double _y) : x(_x), y(_y) {} // construct with x,y

        void set(double _x, double _y) {
            x = _x;
            y = _y;
        }

        void show() { // print x and y of this
            std::cout << x << "," << y << "\n";
        }

        Point2d operator=(Point2d other) { // scalar/dot product of this and other
            this->x = other.x;
            this->y = other.y;
            return *this;
        }

        double operator*(Point2d other) { // scalar/dot product of this and other
            return x * other.x + y * other.y;
        }

        Point2d scale(double mag) { // vector * scalar
            Point2d newVect(x * mag, y * mag);
            return newVect;
        }

        double cross_prod(Point2d other) // cross product of this and other
        {
            return x * other.y - y * other.x;
        }

        Point2d operator+(Point2d other) { // add this point and other
            Point2d newVect(x + other.x, y + other.y);
            return newVect;
        }

        Point2d operator-(Point2d other) { // subtract other from this point
            Point2d newVect;
            newVect.x = x - other.x;
            newVect.y = y - other.y;
            return newVect;
        }

        bool operator==(Point2d other) { // scalar/dot product of this and other
            if (x == other.x && y == other.y) {
                return true;
            }
            return false;
        }

        double dist() { // length between this point and 0,0
            return sqrt(x * x + y * y);
        }

        double dist(Point2d other) { // length between this point and other
            return (*this - other).dist();
        }

        double angle_between_radian(Point2d other) // return angle in radians between
                                                   // this point and other relative to
                                                   // origin (0,0)
        {
            if (abs(x - other.x) < .000001 &&
                abs(y - other.y) < .000001) { // vectors are effecvily the same
                return (0);
            }
            if (abs(x / other.x - y / other.y) <
                .000001) { // vectors are effecvily parallel
                if (((x > 0) == (other.x > 0)) &&
                    ((y > 0) == (other.y > 0))) { // and are pointing the same way
                    return (0);
                }
            }
            return (cross_prod(other) < 0 ? 1 : -1) *
                acos((*this) * other / (dist() * other.dist()));
        }

        double angle_between_deg(Point2d other) // return angle in degrees between
                                                // this point and other relative to
                                                // origin (0,0)
        {
            return angle_between_radian(other) / pi * 180;
        }
    };

    // Vector2d defines a vector that can be addressed as a 2d vector (i.e. with x,y)
    template <typename T> class Vector2d {
        std::vector<T> data;
        int R, C;

        // get index into data vector for a given x,y
        inline int getIndex(int r, int c) { return (r * C) + c; }

    public:
        Vector2d() {
            R = 0;
            C = 0;
        }
        // construct a vector of size x * y
        Vector2d(int x, int y) : R(y), C(x) { data.resize(R * C); }

        Vector2d(int x, int y, T value) : R(y), C(x) { data.resize(R * C, value); }

        void reset(int x, int y) {
            R = y;
            C = x;
            data.clear();
            data.resize(R * C);
        }

        void reset(int x, int y, T value) {
            R = y;
            C = x;
            data.clear();
            data.resize(R * C, value);
        }

        // overwrite this classes data (vector<T>) with data coppied from newData
        void assign(std::vector<T> newData) {
            if ((int)newData.size() != R * C) {
                std::cout << "  ERROR :: in Vector2d::assign() vector provided does not "
                    "fit. provided vector is size "
                    << newData.size() << " but Rows(" << R << ") * Columns(" << C
                    << ") == " << R * C << ". Exitting." << std::endl;
                exit(1);
            }
            data = newData;
        }

        // provides access to value x,y can be l-value or r-value (i.e. used for
        // lookup of assignment)
        T& operator()(int x, int y) { return data[getIndex(y, x)]; }

        T& operator()(double x, double y) {
            return data[getIndex((int)(y), (int)(x))];
        }

        T& operator()(std::pair<int, int> loc) {
            return data[getIndex(loc.second, loc.first)];
        }

        T& operator()(std::pair<double, double> loc) {
            return data[getIndex((int)(loc.second), (int)(loc.first))];
        }

        T& operator()(Point2d loc) { return data[getIndex((int)loc.y, (int)loc.x)]; }

        // show the contents of this Vector2d with index values, and x,y values
        void show() {
            for (int r = 0; r < R; r++) {
                for (int c = 0; c < C; c++) {
                    std::cout << getIndex(r, c) << " : " << c << "," << r << " : "
                        << data[getIndex(r, c)] << "\n";
                }
            }
        }

        // show the contents of this Vector2d in a grid
        void showGrid(int precision = -1) {
            if (precision < 0) {
                for (int r = 0; r < R; r++) {
                    for (int c = 0; c < C; c++) {
                        std::cout << data[getIndex(r, c)] << " ";
                    }
                    std::cout << "\n";
                }
            }
            else {
                for (int r = 0; r < R; r++) {
                    for (int c = 0; c < C; c++) {
                        if (data[getIndex(r, c)] == 0) {
                            std::cout << std::setfill(' ') << std::setw((precision * 2) + 2)
                                << " ";
                        }
                        else {
                            std::cout << std::setfill(' ') << std::setw((precision * 2) + 1)
                                << std::fixed << std::setprecision(precision)
                                << data[getIndex(r, c)] << " ";
                        }
                    }
                    std::cout << "\n";
                }
            }
        }
        int x() { return C; }

        int y() { return R; }
    };

    // dx and dy map facing directions to movement directions. i.e. direction 0 is (0,-1) or up
    std::array<int, 8> dx = {  0, 1, 1, 1, 0,-1,-1,-1 };
    std::array<int, 8> dy = { -1,-1, 0, 1, 1, 1, 0,-1 };

    int inputCount, outputCount;
    int outputsNeededForTurnSign = 0;
    std::vector<Vector2d<int>> maps;
    std::vector<std::pair<int, int>> mapSizes;
    std::vector<std::pair<int, int>> startLocations;
    std::vector<double> maxScores;
    std::vector<int> minSteps;
    std::vector<int> forwardCounts;
    std::vector<int> turnCounts;
    std::vector<int> initalDirections;
    std::vector<std::pair<int, int>> turnSignalPairs;

    PathFollowWorld(shared_ptr<ParametersTable> PT_);
	virtual ~PathFollowWorld() = default;

    void loadMaps(std::vector<string>& mapNames, std::vector<Vector2d<int>>& maps, std::vector<std::pair<int, int>>& mapSizes, std::vector<int>& initalDirections, std::vector<std::pair<int, int>>& startLocations);
    
	virtual auto evaluate(map<string, shared_ptr<Group>>& /*groups*/, int /*analyze*/, int /*visualize*/, int /*debug*/) -> void override;

	virtual auto requiredGroups() -> unordered_map<string,unordered_set<string>> override;
};

