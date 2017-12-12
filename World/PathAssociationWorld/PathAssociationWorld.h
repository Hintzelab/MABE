//
//  PathAssociationWorld.h
//  ComplexiPhi
//
//  Created by Arend Hintze on 9/8/16.
//  Copyright © 2016 Arend Hintze. All rights reserved.
//

#pragma once

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

namespace PathAssociation {
    const int xm[]={0,1,1,1,0,-1,-1,-1};
    const int ym[]={-1,-1,0,1,1,1,0,-1};
}

class PathAssociationWorld : public AbstractWorld {
    
public:
    static shared_ptr<ParameterLink<double>> poisonValuePL;
    static shared_ptr<ParameterLink<double>> probabilitySignChangePL;
	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;

    vector<vector<int>> area,dist;
    int xDim,yDim;
    int startX,startY,startD;
    int intervallMin,intervallMax;
	 int LRTruth[2];
    int pathLength;
    double poisonValue;
    double hiddenNodeNoise, inputNodeNoise, outputNodeNoise;
	 bool useHiddenNodeNoise, useInputNodeNoise, useOutputNodeNoise;
	 double probabilitySignChange;
	string brainName;
    string groupName;
	
    PathAssociationWorld (shared_ptr<ParametersTable> _PT = nullptr);
    virtual ~PathAssociationWorld () = default;
    
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse, visualize, debug);
		}
	}
	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",4,2" } } }; // default requires a root group and a brain (in root namespace) and no genome
	}
	
    virtual void makeWorld();
    virtual void showWorld();
};
