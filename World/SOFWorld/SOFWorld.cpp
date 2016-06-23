//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "SOFWorld.h"

shared_ptr<ParameterLink<string>> SOFWorld::scoreMapFilenamePL = Parameters::register_parameter("WORLD_SOF-scoreMapFilename", (string)"World/SOFWorld/scoreMap_20x20_2peaks.txt", "name of file containing score map.");


SOFWorld::SOFWorld(shared_ptr<ParametersTable> _PT) :
AbstractWorld(_PT) {
	x = y = 0;
	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("x");
	aveFileColumns.push_back("y");
	
	ifstream scoreMap;
	string rawLine;
	int temp;
	
	string filename = (PT == nullptr) ? scoreMapFilenamePL->lookup() : PT->lookupString("WORLD_SOF-scoreMapFilename");

	scoreMap.open(filename);
	
	if(scoreMap.is_open())
	{
		getline(scoreMap, rawLine);
		std::stringstream ss(rawLine);
		ss >> x;
		ss >> y;
		
		for(int i=0;i<y;i++){
			getline(scoreMap, rawLine);
			std::stringstream ss(rawLine);
			for(int j=0;j<x;j++){
				ss >> temp;
				
				//cout << temp << endl;
				scoreMatrix.push_back(temp);
			}
			
		}//close for loop
		
	} else{
		cout << "ERROR: could not find WORLD_SOF-scoreMapFilename with value \"" << filename << "\"." << endl;
		exit(1);
	}
	
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void SOFWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {
	
	
	org->brain->resetBrain();
	org->brain->setInput(0,1); // give the brain a constant 1 (for wire brain)
	org->brain->update();
	//double score = 0.0;
	
	double local_x = org->brain->readOutput(0);
	double local_y = org->brain->readOutput(1);
	
	if(local_x < 0){
		local_x = 0;
	}
	if(local_x >= x){
		local_x = x-1;
	}
	
	if(local_y < 0){
		local_y = 0;
	}
	if(local_y >= y){
		local_y = y-1;
	}
	
	double key = (x*local_y) + local_x;
	
	double score = scoreMatrix[key];
	org->score = score;
	org->dataMap.Append("allscore", score);
	org->dataMap.Append("allx", local_x);
	org->dataMap.Append("ally", local_y);
}

int SOFWorld::requiredInputs() {
	return 1;
}
int SOFWorld::requiredOutputs() {
	return 2;
}
int SOFWorld::maxOrgsAllowed()  {
	return 1;
}
int SOFWorld::minOrgsAllowed()  {
	return 1;
}

