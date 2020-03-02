//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "{{MODULE_NAME}}{{MODULE_TYPE}}.h"

shared_ptr<ParameterLink<int>> {{MODULE_NAME}}World::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_{{MODULE_NAME_CAPS}}-evaluationsPerGeneration", 3, "how many times should this world be run to generate average scores/behavior");
shared_ptr<ParameterLink<string>> {{MODULE_NAME}}World::groupNameSpacePL = Parameters::register_parameter("WORLD_{{MODULE_NAME_CAPS}}-groupNameSpace", (string)"root::", "namespace of group to be evaluated");
shared_ptr<ParameterLink<string>> {{MODULE_NAME}}World::brainNameSpacePL = Parameters::register_parameter("WORLD_{{MODULE_NAME_CAPS}}-brainNameSpace", (string)"root::", "namespace for parameters used to define brain");

// Constructor
// This only gets called once per MABE, typically
{{MODULE_NAME}}World::{{MODULE_NAME}}World(shared_ptr<ParametersTable> PT_) : AbstractWorld(PT_) {
	
	// Required inputs and outputs
	requiredInputs = 2;
	requiredOutputs = 1;

	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the variance (performed automatically by DataMap because _VAR)
}

// evaluate()
// This gets passed a population, every generation.
// Its job is to evaluate organisms and assign scores
// that the rest of MABE uses to perform selection
// and replication
auto {{MODULE_NAME}}World::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {
	// solo evaluation
	int popSize = groups[groupNameSpacePL->get(PT)]->population.size(); 

  // simple version is to evaluate each organism individually
	for (int i = 0; i < popSize; i++) { // for each organism, run evaluateSolo.
    // create a shortcut to access the organisms brain
    auto org = groups[groupNameSpacePL->get(PT)]->population[i];
    auto brain = org->brains[brainNameSpacePL->get(PT)]; 

    // evaluate this organism evaluations Per Generation times
    for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
      brain->resetBrain(); // clear the brain (this function is defined by the brain, and will differ based on the brain being used)
      // set input based on agent and world state
      brain->setInput(0, 1.0);  // set the brains input 0 to 1
      brain->setInput(1, 1.0);  // set the brains input 0 to 1
      // let agent compute 1 tick (could do multiple times)
      brain->update(); // run a brain update (i.e. ask the brain to convert it's inputs into outputs)
      double brain_output = brain->readOutput(0);
      // update agent or world state based on output
      org->dataMap.append("score", 100.0); // add data to organisms data
    } // end of statical evaluations loop
	} // end of population loop
}

// required function that is called by the rest of MABE
// to know how to set up brains to work with this world/task
auto {{MODULE_NAME}}World::requiredGroups() -> unordered_map<string,unordered_set<string>> {
	return { 
		{ groupNameSpacePL->get(PT), { "B:" + brainNameSpacePL->get(PT) + "," + to_string(requiredInputs) + "," + to_string(requiredOutputs) } }
	};
}
