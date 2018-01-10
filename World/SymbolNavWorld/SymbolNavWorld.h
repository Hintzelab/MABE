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

#include "../../World/AbstractWorld.h" // AbstractWorld defines all the basic function templates for worlds
#include "../../Brain/MarkovBrain/MarkovBrain.h"

using namespace std;

struct Pos{
    int x,y;
    static Pos newPos(int X,int Y);
};

class SymbolNavWorld : public AbstractWorld {

public:
    static const int xDim;
    static const int yDim;
    static const int empty;
    static const int wall;
    static const int xm4[4];
    static const int ym4[4];
    static const int mapping[24][4];
    vector<vector<int> > area;
    vector<vector<int> > distMap;
    vector<vector<int> > dirMap;
    struct Point {
        int x,y;
    };
    vector<Point> startPositions;
    int targetX,targetY,startX,startY, currentMapID;
    int makeNumberNotBorder(int range);
    static int stepsToGoal;
    void makeMap();
    virtual string getAppliedPosFeedback(shared_ptr<MarkovBrain> brain);
    virtual string getAppliedNegFeedback(shared_ptr<MarkovBrain> brain);

	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	// parameters for group and brain namespaces
	static shared_ptr<ParameterLink<string>> groupNameSpacePL;
	static shared_ptr<ParameterLink<string>> brainNameSpacePL;
	static shared_ptr<ParameterLink<int>> stepsToGoalPL;
	
	SymbolNavWorld(shared_ptr<ParametersTable> _PT);
	virtual ~SymbolNavWorld() = default;

	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug);
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) override;

	virtual unordered_map<string, unordered_set<string>> requiredGroups() override;
};

