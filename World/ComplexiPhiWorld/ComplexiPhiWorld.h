//
//  ComplexiPhiWorld.hpp
//  ComplexiPhi
//
//  Created by Arend Hintze on 8/16/16.
//  Copyright © 2016 Arend Hintze. All rights reserved.
//

#pragma once

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include "../../Genome/CircularGenome/CircularGenome.h"

using namespace std;

class ComplexiPhiWorld : public AbstractWorld {
    
public:
    static shared_ptr<ParameterLink<int>> nrOfLeftSensorsPL;
    static shared_ptr<ParameterLink<int>> nrOfRightSensorsPL;
    static shared_ptr<ParameterLink<int>> gapWidthPL;
    static shared_ptr<ParameterLink<int>> blockShiftSpeedPL;
    static shared_ptr<ParameterLink<string>> blockPatternsStringPL;
    static shared_ptr<ParameterLink<bool>> analyzeCognitiveNoiseRobustnessParameter;
    static shared_ptr<ParameterLink<bool>> analyzeGeneticNoiseRobustnessParameter;
	static shared_ptr<ParameterLink<string>> groupNamePL;
	static shared_ptr<ParameterLink<string>> brainNamePL;

    double hiddenNodeNoise, inputNodeNoise, outputNodeNoise;
	 bool useHiddenNodeNoise, useInputNodeNoise, useOutputNodeNoise;
    int nrOfLeftSensors,nrOfRightSensors,gapWidth,blockShiftSpeed;
    string blockPatternsString;
    vector<int> patterns;
	 int timeToCatch;
	string brainName;
	string groupName;
	
    ComplexiPhiWorld (shared_ptr<ParametersTable> _PT = nullptr);
    virtual ~ComplexiPhiWorld () = default;
	virtual void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;
	virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
		int popSize = groups[groupNamePL->get(PT)]->population.size();
		for (int i = 0; i < popSize; i++) {
			evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyse, visualize, debug);
		}
	}
	
	virtual unordered_map<string, unordered_set<string>> requiredGroups() override {
		return { { groupNamePL->get(PT),{ "B:" + brainNamePL->get(PT) + ",4,3"} } }; // default requires a root group and a brain (in root namespace) and no genome
	}

};
