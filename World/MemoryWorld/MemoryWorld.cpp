//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "MemoryWorld.h"

shared_ptr<ParameterLink<string>> MemoryWorld::maskPL = Parameters::register_parameter("WORLD_MEMORY-mask",(string) "[1]", "The window that determines how the organism should output the values it reads.");
shared_ptr<ParameterLink<int>> MemoryWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_MEMORY-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains) - NOT CURRENTLY USED");
shared_ptr<ParameterLink<int>> MemoryWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_MEMORY-worldUpdates", 3, "number of world updates, (larger than Mask's length)");

shared_ptr<ParameterLink<string>> MemoryWorld::groupNamePL = Parameters::register_parameter("WORLD_MEMORY_NAMES-groupName", (string)"root", "name of group to be evaluated");
shared_ptr<ParameterLink<string>> MemoryWorld::brainNamePL = Parameters::register_parameter("WORLD_MEMORY_NAMES-brainName", (string)"root", "name of brains used to control organisms\nroot = use empty name space\nGROUP:: = use group name space\n\"name\" = use \"name\" namespace at root level\nGroup::\"name\" = use GROUP::\"name\" name space");

MemoryWorld::MemoryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {    
	convertCSVListToVector((PT == nullptr) ? maskPL->lookup() : PT->lookupString("WORLD_MEMORY-mask"),rawMask);
	evaluationsPerGeneration = (PT == nullptr) ? evaluationsPerGenerationPL->lookup() : PT->lookupInt("WORLD_MEMORY-evaluationsPerGeneration");
    worldUpdates = (PT == nullptr) ? worldUpdatesPL->lookup() : PT->lookupInt("WORLD_MEMORY-worldUpdates");
	groupName = (PT == nullptr) ? groupNamePL->lookup() : PT->lookupString("WORLD_MEMORY_NAMES-groupName");
	brainName = (PT == nullptr) ? brainNamePL->lookup() : PT->lookupString("WORLD_MEMORY_NAMES-brainName");

	cout << "mask: ";
	for (auto mv : rawMask) {
		cout << mv << " ";
	}
	cout << endl;

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
    for(size_t i=0; i<(int)rawMask.size(); i++){
		cout << "  rawMask["<<i<<"] = " << rawMask[i] << endl;
        if(rawMask[i]!=0){
			popFileColumns.push_back("Out" + to_string(i));
			cout << "out" << to_string(i) << "  added." << endl;
            nOut++;
        }
    }
}

//given a sequence and told to memorize it, then repeated back.
void MemoryWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
    vector<double> scores;
    scores.resize(rawMask.size(),0);

	int sub2s = (Random::getInt(0,1))?1:-1; // used to replace 2s in mask

	// config mask :: note this must be done here because of 2 and -2 (i.e. per evaluation randomness)
	// a) each 2 must be replaced with sub2s
	// b) each -2 must be replaces with a random expectation
	// nOut is the number of outputs expected from the brains. That is mask size - 0s in mask.
	
	bool makeNewPassword = false;
	bool makeNewMask = true;

	if (currentUpdate != Global::update) {
		// this is a new global update
		currentUpdate = Global::update;
		makeNewPassword = true;
		makeNewMask = true;
	}
	if (makeNewMask) {
		mask = rawMask;
		for (int i = 0; i < (int)mask.size(); i++) {
			if (abs(mask[i]) > 2) {
				cout << "  ERROR! in Memory world mask, found value " << mask[i] << " at position " << i << ".\n  Values in mask must be in range [-2,2].\nExiting..." << endl;
				exit(1);
			}
			if (mask[i] == 2) {
				mask[i] = sub2s; // replace each 2 with the same random expectation
			}
			if (mask[i] == -2) {
				mask[i] = (Random::getInt(0, 1)) ? 1 : -1; // replace each -2 with a random expectation
			}
		}
	}

	
	if (makeNewPassword) {
		password.clear();
		//fills password with worldUpdates+(outMask.size()-1) binary values (i.e. enough to run world updates + enough to program initial values in brain)
		for (int i = 0; i < worldUpdates + mask.size(); i++) {
			password.push_back(Random::getInt(0, 1));
		}
	}
    org->brain->resetBrain();

	// load initial values
	for (int i = 0; i<int(mask.size()) - 1; i++) {
		//org->brain->resetOutputs();
		org->brain->setInput(0, password[i]);
		org->brain->update();
	}

    //now asked to repeat the sequence given and is scored based on the mask, with each output incremented based on whether it's correct.

	for(int update = 0; update < worldUpdates; update++){
		int pw_index = update + (int)mask.size() - 1; // position pw_index (password index) at next value in password to input to brain
        //org->brain->resetOutputs();
        org->brain->setInput(0, password[pw_index]);
        org->brain->update();
        int brainOutputIndex = 0; // this will index into brain output - only moved forward for mask values != 0
		for (int mask_index = 0; mask_index < (int)mask.size(); mask_index++) {
            int maskValue = mask[mask_index];
			//cout << "mask[" << mask_index << "]: " << maskValue;
			if (maskValue != 0) { // if the position in mask is being observed...
				int passWordValue=password[pw_index - mask_index];
				int guess = Bit(org->brain->readOutput(brainOutputIndex++));
				//cout << "   checking... passWordValue = " << passWordValue << "  brainOutput[" << brainOutputIndex - 1 << "] = " << guess;
				if ((maskValue==1 && passWordValue==guess) || (maskValue == 1 && passWordValue == guess)){
                    scores[mask_index]++;
					//cout << "  Correct!";
                }
            }
			//cout << endl;
        }
    }

	double score = 0;
	//adds the scores of each output together and adds it to the dataMap
    for(int i=0; i<(int)mask.size(); i++){
		if (mask[i] != 0) {
			score += scores[i];
			org->dataMap.Append("Out" + to_string(i), scores[i] / (double)worldUpdates);
		}
    }
    org->dataMap.Append("score", 1 + (score / (worldUpdates*nOut)));
}

int MemoryWorld::requiredInputs() {
	return 1;
}
int MemoryWorld::requiredOutputs() {
    return nOut;
}
