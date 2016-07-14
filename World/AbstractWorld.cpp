//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "AbstractWorld.h"

#include <math.h>

#include "../Utilities/Data.h"

shared_ptr<ParameterLink<int>> AbstractWorld::repeatsPL = Parameters::register_parameter("WORLD-repeats", 3, "Number of times to test each Genome per generation");
shared_ptr<ParameterLink<bool>> AbstractWorld::debugPL = Parameters::register_parameter("WORLD-debug", false, "run world in debug mode (if available)");

shared_ptr<ParameterLink<bool>> AbstractWorld::groupEvaluationPL = Parameters::register_parameter("WORLD-groupEvaluation", false, "if true, evaluate population concurrently");
shared_ptr<ParameterLink<string>> AbstractWorld::worldTypePL = Parameters::register_parameter("WORLD-worldType", (string) "Berry", "type of world to use in evolution loop");

void AbstractWorld::evaluate(shared_ptr<Group> group, bool groupEvaluation, bool analyse, bool visualize, bool debug) {
	vector<double> scores(group->population.size(), 0);

	//for (size_t i = 0; i < group->population.size(); i++) {
	//	group->population[i]->dataMap.Clear("allscore");
	//}
	if (groupEvaluation && maxOrgsAllowed() == 1){
		cout << " ERROR! World only accepts at most one organism at a time, but groupEvaluation is on.\n Exiting."<<endl;
	}
	if (groupEvaluation) {
		for (int r = 0; r < repeatsPL->lookup(); r++) {
			runWorld(group, analyse, visualize, debug);
			for (size_t i = 0; i < group->population.size(); i++) {
				scores[i] += group->population[i]->score;
			}
		}
	} else {
		vector<shared_ptr<Organism>> soloPopulation;
		shared_ptr<Group> soloGroup = make_shared<Group>(soloPopulation, group->optimizer, group->archivist);
		for (size_t i = 0; i < group->population.size(); i++) {
			soloGroup->population.clear();
			soloGroup->population.push_back(group->population[i]);
			for (int r = 0; r < repeatsPL->lookup(); r++) {
				runWorld(soloGroup, analyse, visualize, debug);
				scores[i] += group->population[i]->score;
				//scores[i] += testIndividual(group->population[i],0,0);
				//group->population[i]->dataMap.Append("allscore",group->population[i]->score);
			}
		}
	}
	for (size_t i = 0; i < group->population.size(); i++) {
		group->population[i]->score = scores[i] / repeatsPL->lookup();
	}
}
