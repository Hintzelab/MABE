//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "MemoryWorld.h"

shared_ptr<ParameterLink<string>> MemoryWorld::maskPL = Parameters::register_parameter("WORLD_MEMORY-mask",(string) "[1]", "The window that determines how the organism should output the values it reads.");
shared_ptr<ParameterLink<int>> MemoryWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_MEMORY-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains) - NOT CURRENTLY USED");
shared_ptr<ParameterLink<int>> MemoryWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_MEMORY-worldUpdates", 3, "number of world updates, (larger than Mask's length)");

shared_ptr<ParameterLink<int>> MemoryWorld::refreshPasswordPL = Parameters::register_parameter("WORLD_MEMORY-refreshPassword", 1, "how often should the password be reset (0 = never, 1 = per generation, 2 = every eval)");
shared_ptr<ParameterLink<int>> MemoryWorld::refreshMaskPL = Parameters::register_parameter("WORLD_MEMORY-refreshMask", 1, "how often should the mask be reset (0 = never, 1 = per generation, 2 = every eval)");

shared_ptr<ParameterLink<string>> MemoryWorld::groupNamePL = Parameters::register_parameter("WORLD_MEMORY_NAMES-groupNameSpace", (string)"root::", "namespace of group to be evaluated");
shared_ptr<ParameterLink<string>> MemoryWorld::brainNamePL = Parameters::register_parameter("WORLD_MEMORY_NAMES-brainNameSpace", (string)"root::", "namespace for parameters used to define brain");


MemoryWorld::MemoryWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {    
	convertCSVListToVector(maskPL->get(PT),rawMask);
	evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);
	worldUpdates = worldUpdatesPL->get(PT);
	refreshPassword = refreshPasswordPL->get(PT);
	refreshMask = refreshMaskPL->get(PT);

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
	scores.resize(rawMask.size(), 0);

	int sub2s = (Random::getInt(0, 1)) ? 1 : -1; // used to replace 2s in mask

	// config mask :: note this must be done here because of 2 and -2 (i.e. per evaluation randomness)
	// a) each 2 must be replaced with sub2s
	// b) each -2 must be replaces with a random expectation
	// nOut is the number of outputs expected from the brains. That is mask size - 0s in mask.

	bool makeNewPassword = false;
	bool makeNewMask = false;

	if (password.size() == 0 || mask.size() == 0) { // if password is empty, then we need to make a password
		makeNewPassword = true;
		makeNewMask = true;
		currentUpdate = Global::update;
	}
	else if (currentUpdate != Global::update) {
		// this is a new global update
		currentUpdate = Global::update;
		if (refreshPassword > 0) { // if once per generation
			makeNewPassword = true;
		}
		if (refreshMask > 0) { // if once per generation
			makeNewMask = true;
		}
	}
	else {
		if (refreshPassword == 2) { // if refresh always
			makeNewPassword = true;
		}
		if (refreshMask == 2) { // if refresh always
			makeNewMask = true;
		}
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
		for (int i = 0; i < worldUpdates + (int)mask.size(); i++) {
			password.push_back(Random::getInt(0, 1));
		}
	}

	auto brain = org->brains[brainNamePL->get(PT)];
    brain->resetBrain();

	// load initial values
	for (int i = 0; i<int(mask.size()) - 1; i++) {
		brain->resetOutputs();
		brain->setInput(0, password[i]);
		brain->update();
	}

    // iterate over all remaining values in password one input per world update,
	// scoring agaist mask on each world update

	for(int update = 0; update < worldUpdates; update++){
		int pw_index = update + (int)mask.size() - 1; // position pw_index (password index) at next value in password to input to brain
        brain->resetOutputs();
        brain->setInput(0, password[pw_index]);
        brain->update();
        int brainOutputIndex = 0; // this will index into brain output - only moved forward for mask values != 0
		for (int mask_index = 0; mask_index < (int)mask.size(); mask_index++) {
            int maskValue = mask[mask_index];
			//cout << "mask[" << mask_index << "]: " << maskValue;
			if (maskValue != 0) { // if the position in mask is being observed...
				int passWordValue=password[pw_index - mask_index];
				int guess = Bit(brain->readOutput(brainOutputIndex++));
				//cout << "   checking... passWordValue = " << passWordValue << "  brainOutput[" << brainOutputIndex - 1 << "] = " << guess;
				if ((maskValue==1 && passWordValue==guess) || (maskValue == -1 && passWordValue != guess)){
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
			org->dataMap.append("Out" + to_string(i), scores[i] / (double)worldUpdates);
		}
    }
    org->dataMap.append("score", pow(2.0, 1 + (score / (double)(worldUpdates))));
}
