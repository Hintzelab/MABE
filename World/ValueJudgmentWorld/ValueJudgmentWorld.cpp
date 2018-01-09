//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "ValueJudgmentWorld.h"
//#include "limits.h"
//#define PI 3.14159265359
//#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include <cmath>

shared_ptr<ParameterLink<string>> ValueJudgmentWorld::groupNamePL = Parameters::register_parameter("WORLD_VALUEJUDGMENT_NAMES-groupName", (string)"root::", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> ValueJudgmentWorld::brainNamePL = Parameters::register_parameter("WORLD_VALUEJUDGMENT_NAMES-brainName", (string)"root::", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

ValueJudgmentWorld::ValueJudgmentWorld(shared_ptr<ParametersTable> _PT) :AbstractWorld(_PT) {
	
	groupName = groupNamePL->get(_PT);
	brainName = brainNamePL->get(_PT);
	
	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically because _VAR)
	
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void ValueJudgmentWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	auto brain = org->brains[brainName];
	double score=0.0;
    double p=0.55;
    for(int r=0;r<100;r++){
        int whereIsHigh=Random::getIndex(2);
        double P[2]={0.0,0.0};
        P[whereIsHigh]=p;
        P[1-whereIsHigh]=1.0-p;
        brain->resetBrain();
        bool gaveAnswer=false;
        for(int i=0;i<100;i++){
            for(int j=0;j<2;j++){
                if(Random::P(P[j]))
                    brain->setInput(j, 1.0);
                else
                    brain->setInput(j, 0.0);
            }
            brain->update();
            if(i>80){
                int action=Bit(brain->readOutput(0))+(2*Bit(brain->readOutput(1)));
                switch(action){
                    case 0: case 3:
                        break;
                    case 1:
                        gaveAnswer=true;
                        if(whereIsHigh==0)
                            score+=1.0;
                        i=100;
                        break;
                    case 2:
                        gaveAnswer=true;
                        if(whereIsHigh==1)
                            score+=1.0;
                        i=100;
                        break;
                }
            }
        }
    }
	org->dataMap.set("score",score);
	
}

