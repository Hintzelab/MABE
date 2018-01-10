//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License
#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include "SymbolNavWorld.h"

shared_ptr<ParameterLink<int>> SymbolNavWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_SYMBOLNAV-evaluationsPerGeneration", 1, "how many times should this world be run to generate average scores/behavior");

const int SymbolNavWorld::xDim=64;
const int SymbolNavWorld::yDim=64;
const int SymbolNavWorld::empty=0;
const int SymbolNavWorld::wall=1;
const int SymbolNavWorld::xm4[4]={0,1,0,-1};
const int SymbolNavWorld::ym4[4]={-1,0,1,0};
const int SymbolNavWorld::mapping[24][4]={
    {0,1,2,3} , {1,0,2,3} , {2,1,0,3} , {3,1,2,0},
    {0,1,3,2} , {1,0,3,2} , {2,1,3,0} , {3,1,0,2},
    {0,2,1,3} , {1,2,0,3} , {2,0,1,3} , {3,2,1,0},
    {0,2,3,1} , {1,2,3,0} , {2,0,3,1} , {3,2,0,1},
    {0,3,1,2} , {1,3,0,2} , {2,3,1,0} , {3,0,1,2},
    {0,3,2,1} , {1,3,2,0} , {2,3,0,1} , {3,0,2,1}
};
int SymbolNavWorld::stepsToGoal=0;

Pos Pos::newPos(int X,int Y){
    Pos P;
    P.x=X;
    P.y=Y;
    return P;
}


shared_ptr<ParameterLink<string>> SymbolNavWorld::groupNameSpacePL = Parameters::register_parameter("WORLD_SYMBOLNAV-groupNameSpace", (string)"root::", "namespace of group to be evaluated");
shared_ptr<ParameterLink<string>> SymbolNavWorld::brainNameSpacePL = Parameters::register_parameter("WORLD_SYMBOLNAV-brainNameSpace", (string)"root::", "namespace for parameters used to define brain");
shared_ptr<ParameterLink<int>> SymbolNavWorld::stepsToGoalPL = Parameters::register_parameter("WORLD_SYMBOLNAV-stepsToGoal", xDim/2, "minimum steps to the goal.");

SymbolNavWorld::SymbolNavWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	
    SymbolNavWorld::stepsToGoal = stepsToGoalPL->get(_PT);

	requiredInputs = 4;
	requiredOutputs = 2;

	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically by DataMap because _VAR)

    area.resize(xDim, vector<int>(yDim));
    distMap.resize(xDim, vector<int>(yDim));
    dirMap.resize(xDim, vector<int>(yDim));
}

inline int SymbolNavWorld::makeNumberNotBorder(int range){
    return Random::getInt(1,range-1);
}

void SymbolNavWorld::makeMap(){
    vector<Pos> current,next;
    int cDist=1;
    int i,j,k;
    startPositions.clear();
    do{
        cDist=1;
        for(i=0;i<xDim;i++)
            for(j=0;j<yDim;j++){
                distMap[i][j]=-1;
                area[i][j]=-1;
                dirMap[i][j]=-1;
                if((i==0)||(i==(xDim-1))||(j==0)||(j==(yDim-1))||(Random::getInt(7)==0))
                    area[i][j]=0;
            }
        targetX=makeNumberNotBorder(xDim);
        targetY=makeNumberNotBorder(yDim);
        distMap[targetX][targetY]=0;
        current.clear();
        next.clear();
        current.push_back(Pos::newPos(targetX,targetY));
        while (current.size()!=0) {
            for (i=0; i<(signed)current.size(); i++) {
                for(j=0; j<4; j++) {
                    if ((area[current[i].x+xm4[j]][current[i].y+ym4[j]]!=0)
                       &&(distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]==-1)) {
                        distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]=cDist;
                        if (cDist == SymbolNavWorld::stepsToGoal) startPositions.push_back(SymbolNavWorld::Point{current[i].x+xm4[j],current[i].y+ym4[j]});
                        next.push_back(Pos::newPos(current[i].x+xm4[j],current[i].y+ym4[j]));
                    }
                }
            }
            current.clear();
            current=next;
            next.clear();
            cDist++;
        }
    } while(cDist<xDim/2);
    int whichStartPosition=Random::getIndex(startPositions.size());
    startX=startPositions[whichStartPosition].x;
    startY=startPositions[whichStartPosition].y;
    for (i=1; i<xDim-1; i++) {
        for (j=1; j<yDim-1; j++) {
            if (distMap[i][j]>0){
                int mD=distMap[i][j];
                dirMap[i][j]=0;
                for (k=0;k<4;k++) {
                    if (distMap[i+xm4[k]][j+ym4[k]]!=-1) {
                        if (distMap[i+xm4[k]][j+ym4[k]]<mD) {
                            mD=distMap[i+xm4[k]][j+ym4[k]];
                            dirMap[i][j]=k;
                        }
                    }
                }
            }
        }
    }
    dirMap[targetX][targetY]=Random::getInt(3);
}

void SymbolNavWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	// code to evaluating a single agent
    int xPos,yPos,dir;
    int t;
    int goalsReached=0;
    string timeDelta="";
    int steps=4*(xDim+yDim);
    int i;
	
	// create a shortcut to access the organisms brain
	auto brain = org->brains[brainNameSpacePL->get(PT)]; 
    org->dataMap.append("goalTimes"+to_string(currentMapID), 0);

	// evaluate this organism evaluations Per Generation times
	for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
		brain->resetBrain(); // clear the brain (this function is defined by the brain, and will differ based on the brain being used)
        xPos=startX;
        yPos=startY;
        dir=Random::getInt(3);
        double fitness=0.0;
        for(t=0; t<steps; t++) {
            for (i=0; i<4; i++) brain->setInput(i, 1);  // set the brains input 0 to 1
            int before, after;
            brain->setInput((dirMap[xPos][yPos]-dir)&3,1); // read the symbol from the floor
            brain->update();
            int action = Bit(brain->readOutput(0)) | Bit(brain->readOutput(1))<<1;
            switch (mapping[currentMapID][action]){
                case 0:break;
                case 1:dir=(dir-1)&3; break;
                case 2:dir=(dir+1)&3; break;
                case 3:if(distMap[xPos+xm4[dir]][yPos+ym4[dir]]!=-1) {
                           xPos=xPos+xm4[dir];
                           yPos=yPos+ym4[dir];
                       }
                       break;
            }
            fitness+=1.0/pow((double)(distMap[xPos][yPos]+1),2.0);
            if (distMap[xPos][yPos] == 0)
            {
                fitness+=1000.0;
                goalsReached++;
                int whichStartPosition=Random::getIndex(startPositions.size());
                xPos=startPositions[whichStartPosition].x;
                yPos=startPositions[whichStartPosition].y;
                dir=Random::getInt(3);
                org->dataMap.append("goalTimes"+to_string(currentMapID), t);
            }
        }	
        org->dataMap.set("goalReached"+to_string(currentMapID), goalsReached);
        shared_ptr<MarkovBrain> markovBrain = dynamic_pointer_cast<MarkovBrain>(brain);
        if (markovBrain) {
            if (Gate_Builder::usingFeedbackGatePL->get(PT) or Gate_Builder::usingDecomposableFeedbackGatePL->get(PT)) {
                org->dataMap.append("PositiveFB"+to_string(currentMapID), getAppliedPosFeedback(markovBrain));
                org->dataMap.append("NegativeFB"+to_string(currentMapID), getAppliedNegFeedback(markovBrain));
            }
        }
        org->dataMap.append("score", fitness);
	}
}
void SymbolNavWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse, int visualize, int debug) {
    makeMap();
	int popSize = groups[groupNameSpacePL->get(PT)]->population.size(); 
	for (int i = 0; i < popSize; i++) { // for each organism, run evaluateSolo.
        for (int m = 0; m < 24; m++) {
            currentMapID=m;
            evaluateSolo(groups[groupNameSpacePL->get(PT)]->population[i], analyse, visualize, debug);
        }
	}
}

unordered_map<string, unordered_set<string>> SymbolNavWorld::requiredGroups() {
	return { 
		{ groupNameSpacePL->get(PT), { "B:" + brainNameSpacePL->get(PT) + "," + to_string(requiredInputs) + "," + to_string(requiredOutputs) } }
	};
}

string SymbolNavWorld::getAppliedPosFeedback(shared_ptr<MarkovBrain> brain){
    int counter=0;
    string feedback="";
    for (int i=brain->gates.size()-1; i>=0; i--) {
        shared_ptr<FeedbackGate> feedbackGate = dynamic_pointer_cast<FeedbackGate>(brain->gates[i]);
        if (feedbackGate) {
            counter+=1;
            feedback+=" FeedbackGate"+ to_string(counter) + feedbackGate->getAppliedPosFeedback();
        } else {
            shared_ptr<DecomposableFeedbackGate> decoFeedbackGate = dynamic_pointer_cast<DecomposableFeedbackGate>(brain->gates[i]);
            if (decoFeedbackGate) {
                counter+=1;
                feedback+=" DecoFeedbackGate"+ to_string(counter) + decoFeedbackGate->getAppliedPosFeedback();
            }
        }
    }
    return feedback;
}

string SymbolNavWorld::getAppliedNegFeedback(shared_ptr<MarkovBrain> brain){
    int counter=0;
    string feedback="";
    for (int i=brain->gates.size()-1; i>=0; i--) {
        shared_ptr<FeedbackGate> feedbackGate = dynamic_pointer_cast<FeedbackGate>(brain->gates[i]);
        if (feedbackGate) {
            counter+=1;
            feedback+=" FeedbackGate"+ to_string(counter) + feedbackGate->getAppliedNegFeedback();
        } else {
            shared_ptr<DecomposableFeedbackGate> decoFeedbackGate = dynamic_pointer_cast<DecomposableFeedbackGate>(brain->gates[i]);
            if (decoFeedbackGate) {
                counter+=1;
                feedback+=" DecoFeedbackGate"+ to_string(counter) + decoFeedbackGate->getAppliedNegFeedback();
            }
        }
    }
    return feedback;
}


