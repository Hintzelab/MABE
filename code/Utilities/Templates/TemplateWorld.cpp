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

// this is how you setup a parameter in MABE, the function Parameters::register_parameter()takes the
// name of the parameter (catagory-name), default value (which must conform with the type), a the useage message
shared_ptr<ParameterLink<int>> {{MODULE_NAME}}World::evaluationsPerGenerationPL =
    Parameters::register_parameter("WORLD_{{MODULE_NAME}}-evaluationsPerGeneration", 30,
    "how many times should each organism be tested in each generation?");

// the constructor gets called once when MABE starts up. use this to set things up
{{MODULE_NAME}}World::{{MODULE_NAME}}World(shared_ptr<ParametersTable> PT) : AbstractWorld(PT) {
    
    //localize a parameter value for faster access
    evaluationsPerGeneration = evaluationsPerGenerationPL->get(PT);
    
    // popFileColumns tell MABE what data should be saved to pop.csv files
	popFileColumns.clear();
    popFileColumns.push_back("score");
    popFileColumns.push_back("out0");
}

// the evaluate function gets called every generation. evaluate should set values on organisms datamaps
// that will be used by other parts of MABE for things like reproduction and archiving
auto {{MODULE_NAME}}World::evaluate(map<string, shared_ptr<Group>>& groups, int analyze, int visualize, int debug) -> void {

    int popSize = groups[groupName]->population.size(); 
    
    // in this world, organisms donot interact, so we can just iterate over the population
    for (int i = 0; i < popSize; i++) {

        // create a shortcut to access the organism and organisms brain
        auto org = groups[groupName]->population[i];
        auto brain = org->brains[brainName]; 

        double score = 0;

        // evaluate this organism some number of times based on evaluationsPerGeneration
        for (int t = 0; t < evaluationsPerGeneration; t++) {

            // clear the brain - resets brain state including memory
            brain->resetBrain();

            int in0 = t % 2;                // this will switch this input on and off
            int in1 = Random::getInt(0,1);  // get a random number in the range [0,1]

            // set input based on agent and world state (can be int or double)
            brain->setInput(0, in0); // set input 0 to in0
            brain->setInput(1, in1); // set input 1 to in1

            // run a brain update (i.e. ask the brain to convert it's inputs into outputs)
            brain->update();

            // read brain output 0
            double out0 = brain->readOutput(0);

            // update agent or world state here
            // brain should output 1 if in1 == in2, else output 0
            if ( (out0 > 0) == (in0 == in1) ) {
                score += 1;
            }

            // add output behavior to organisms data
            // this will create a list of out0 values in the dataMap the average of this list will be
            // used to create the pop file in other files, the list will be saved along with the average
            org->dataMap.append("out0", out0);
        }
        // add score to organisms data
        // it can be expensive to access dataMap too often. also, here we want score to be the sum of the correct answers
        org->dataMap.append("score", score);

    } // end of population loop
}

// the requiredGroups function lets MABE know how to set up populations of organisms that this world needs
auto {{MODULE_NAME}}World::requiredGroups() -> unordered_map<string,unordered_set<string>> {
	return { { groupName, { "B:"+brainName+",2,1" } } };
        
        // this tells MABE to make a group called "root::" with a brain called "root::" that takes 2 inputs and has 1 output
        // "root::" here also indicates the namespace for the parameters used to define these elements.
        // "root::" is the default namespace, so parameters defined without a namespace are "root::" parameters
}
