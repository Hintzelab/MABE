//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "TestWorld.h"

shared_ptr<ParameterLink<int>> TestWorld::modePL = Parameters::register_parameter("WORLD_TEST-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
shared_ptr<ParameterLink<int>> TestWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_TEST-numberOfOutputs", 10, "number of outputs in this world");
shared_ptr<ParameterLink<int>> TestWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_TEST-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");

TestWorld::TestWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	mode = (PT == nullptr) ? modePL->lookup() : PT->lookupInt("WORLD_TEST-mode");
	numberOfOutputs = (PT == nullptr) ? numberOfOutputsPL->lookup() : PT->lookupInt("WORLD_TEST-numberOfOutputs");
	evaluationsPerGeneration = (PT == nullptr) ? evaluationsPerGenerationPL->lookup() : PT->lookupInt("WORLD_TEST-evaluationsPerGeneration");

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void TestWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	//cout << "in test world evalSolo" << endl;
	for (int r = 0; r < evaluationsPerGeneration; r++) {
		org->brain->resetBrain();
		org->brain->setInput(0, 1);  // give the brain a constant 1 (for wire brain)
		org->brain->update();
		double score = 0.0;
		for (int i = 0; i < org->brain->nrOutputValues; i++) {
			if (mode == 0) {
				score += Bit(org->brain->readOutput(i));
			}
			else {
				score += org->brain->readOutput(i);
			}
		}
		if (score < 0.0) {
			score = 0.0;
		}
		org->dataMap.Append("score", score);
	}
}

int TestWorld::requiredInputs() {
	return 1;
}
int TestWorld::requiredOutputs() {
	return numberOfOutputs;
}
