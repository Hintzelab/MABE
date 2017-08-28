//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "TestWorld.h"

shared_ptr<ParameterLink<int>> TestWorld::modePL = Parameters::register_parameter("WORLD_TEST-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
shared_ptr<ParameterLink<int>> TestWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_TEST-numberOfOutputs", 10, "number of outputs in this world");
shared_ptr<ParameterLink<int>> TestWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_TEST-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");
shared_ptr<ParameterLink<string>> TestWorld::groupNamePL = Parameters::register_parameter("WORLD_TEST_NAMES-groupNameSpace", (string)"root::", "namespace of group to be evaluated");
shared_ptr<ParameterLink<string>> TestWorld::brainNamePL = Parameters::register_parameter("WORLD_TEST_NAMES-brainNameSpace", (string)"root::", "namespace for parameters used to define brain");

TestWorld::TestWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically because _VAR)
}

void TestWorld::evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) {
	auto brain = org->brains[brainNamePL->get(PT)];
	for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
		brain->resetBrain();
		brain->setInput(0, 1);  // give the brain a constant 1 (for wire brain)
		brain->update();
		double score = 0.0;
		for (int i = 0; i < brain->nrOutputValues; i++) {
			if (modePL->get(PT) == 0) {
				score += Bit(brain->readOutput(i));
			}
			else {
				score += brain->readOutput(i);
			}
		}
		if (score < 0.0) {
			score = 0.0;
		}
		org->dataMap.append("score", score);
		
	}	
}

