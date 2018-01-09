//
//  ComplexiPhiWorld.cpp
//  ComplexiPhi
//
//  Created by Arend Hintze on 8/16/16.
//  Copyright © 2016 Arend Hintze. All rights reserved.
//

#include "ComplexiPhiWorld.h"
#include "../../Brain/MarkovBrain/MarkovBrain.h"

shared_ptr<ParameterLink<int>> ComplexiPhiWorld::nrOfLeftSensorsPL = Parameters::register_parameter("WORLD_COMPLEXIPHI-nrOfLeftSensors", 2, "number of sensors on the left side of the agent");
shared_ptr<ParameterLink<int>> ComplexiPhiWorld::blockShiftSpeedPL = Parameters::register_parameter("WORLD_COMPLEXIPHI-blockShiftSpeed", 1, "number of time ticks for each lateral block movement across the world (1 is every tick)");
shared_ptr<ParameterLink<int>> ComplexiPhiWorld::nrOfRightSensorsPL = Parameters::register_parameter("WORLD_COMPLEXIPHI-nrOfRightSensors", 2, "number of sensors on the right side of the agent");
shared_ptr<ParameterLink<int>> ComplexiPhiWorld::gapWidthPL = Parameters::register_parameter("WORLD_COMPLEXIPHI-gapWidth", 2, "the width of the gap between the left and right sensor/s");
shared_ptr<ParameterLink<string>> ComplexiPhiWorld::blockPatternsStringPL = Parameters::register_parameter("WORLD_COMPLEXIPHI-blockPatternsString", (string)"3,15", "a comma separted list of block patterns (as bitmasks), the odd patterns are to be caught, the even ones are to be avoided");

shared_ptr<ParameterLink<string>> ComplexiPhiWorld::groupNamePL = Parameters::register_parameter("WORLD_COMPLEXIPHI_NAMES-groupName", (string)"root::", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> ComplexiPhiWorld::brainNamePL = Parameters::register_parameter("WORLD_COMPLEXIPHI_NAMES-brainName", (string)"root::", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

ComplexiPhiWorld::ComplexiPhiWorld(shared_ptr<ParametersTable> _PT) : AbstractWorld(_PT) {
	groupName = groupNamePL->get(_PT);
	brainName = brainNamePL->get(_PT);
	nrOfLeftSensors = nrOfLeftSensorsPL->get(_PT);
    blockShiftSpeed = blockShiftSpeedPL->get(_PT);
    nrOfRightSensors = nrOfRightSensorsPL->get(_PT);
    gapWidth = gapWidthPL->get(_PT);
    blockPatternsString = blockPatternsStringPL->get(_PT);
    patterns.clear();
    convertCSVListToVector(blockPatternsString,patterns,',');
    printf("bot setup for ComplexiPhiWorld left:%i gap:%i right:%i\n",nrOfLeftSensors,gapWidth,nrOfRightSensors);
    printf("patterns:\n");
    for(int i=0;i<patterns.size();i++){
        if((i&1)==0)
            printf("\tcatch:%i\n",patterns[i]);
        else
            printf("\tavoid:%i\n",patterns[i]);
        
    }
    printf("\n");
    // columns to be added to ave file
    popFileColumns.clear();
    popFileColumns.push_back("score");
}



// score is number of outputs set to 1 (i.e. output > 0) squared
void ComplexiPhiWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    int correct=0;
    int incorrect=0;
    double fitness=1.0;
    int paddleWidth=nrOfRightSensors+nrOfLeftSensors+gapWidth;
	auto brain = org->brains[brainName];
    for(int i=0;i<patterns.size();i++){
        for(int j=-1;j<2;j+=2){
            for(int k=0;k<16;k++){
                int world=patterns[i];
                brain->resetBrain();
                int botPos=k;
                //loop the world
					 int height(20);
					 bool hit(false);
					 timeToCatch = 0;
                for(int l=0;l<34;l++){ // you only have 34 updates to figure this out
						  ++timeToCatch;
                    int currentSensor=0;
                    for(int ls=0;ls<nrOfLeftSensors;ls++){
                        brain->setInput(currentSensor,(world>>((botPos+ls)&15))&1);
                        currentSensor++;
                    }
                    for(int rs=0;rs<nrOfRightSensors;rs++){
                        brain->setInput(currentSensor,(world>>((botPos+nrOfLeftSensors+gapWidth+rs)&15))&1);
                        currentSensor++;
                    }
                    brain->resetOutputs();
					brain->update();
                    int action=Bit(brain->readOutput(0))+(Bit(brain->readOutput(1))<<1);
                    switch(action){
                        case 0:
                        case 3:// nothing!
                            break;
                        case 1:
                            botPos=(botPos+1)&15;
                            break;
                        case 2:
                            botPos=(botPos-1)&15;
                            break;
                    }
						  if(j==-1){
							  world=((world>>1)&65535)+((world&1)<<15);
						  } else {
							  world=((world<<1)&65535)+((world>>15)&1);
						  }
						  //check for hit
						  if (height == 0) { /// if block is on your same x-axis
							  hit=false;
							  for(int m=0;m<paddleWidth;m++)
								  if(((world>>((botPos+m)&15))&1)==1)
									  hit=true;
							  if((i&1)==0){
								  if(hit){
									  correct++;
									  fitness*=1.05;
									  break;
								  }
							  } else {
								  if(hit){
									  incorrect++;
									  fitness/=1.05;
									  break;
								  }
							  }
						  }
						  if ((Bit(brain->readOutput(2))&1) == 1) {
							  ++height;
						  } else {
							  --height;
						  }
                }
					 /// if time ran out and not caught the block
					 if ((i&1)==0){
						 if (not hit){
							 incorrect++;
							 fitness/=1.05;
						 }
					 } else {
						 if (not hit) {
							 correct++;
							 fitness*=1.05;
						 }
					 }
            }
        }
    }
    org->dataMap.set("score",fitness);
}


