//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// Evaluates agents on how many '1's they can output. This is a purely fixed
// task
// that requires to reactivity to stimuli.
// Each correct '1' confers 1.0 point to score, or the decimal output determined
// by 'mode'.

#include "DAGWorld.h"

std::shared_ptr<ParameterLink<int>> DAGWorld::modePL =
Parameters::register_parameter(
	"WORLD_DAG-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
std::shared_ptr<ParameterLink<int>> DAGWorld::numberOfOutputsPL =
Parameters::register_parameter("WORLD_DAG-numberOfOutputs", 10,
	"number of outputs in this world");
std::shared_ptr<ParameterLink<int>> DAGWorld::evaluationsPerGenerationPL =
Parameters::register_parameter("WORLD_DAG-evaluationsPerGeneration", 1,
	"Number of times to DAG each Genome per "
	"generation (useful with non-deterministic "
	"brains)");
std::shared_ptr<ParameterLink<std::string>> DAGWorld::groupNamePL =
Parameters::register_parameter("WORLD_DAG_NAMES-groupNameSpace",
(std::string) "root::",
"namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> DAGWorld::brainNamePL =
Parameters::register_parameter(
	"WORLD_DAG_NAMES-brainNameSpace", (std::string) "root::",
	"namespace for parameters used to define brain");

DAGWorld::DAGWorld(std::shared_ptr<ParametersTable> PT_)
	: AbstractWorld(PT_) {

	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the
										   // variance (performed automatically
										   // because _VAR)
}

void DAGWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
	int visualize, int debug) {
	auto brain = org->brains[brainNamePL->get(PT)];
	for (int r = 0; r < evaluationsPerGenerationPL->get(PT); r++) {
		brain->resetBrain();
		brain->setInput(0, 1); // give the brain a constant 1 (for wire brain)
		brain->update();
		double score = 0.0;
		for (int i = 0; i < brain->nrOutputValues; i++) {
			if (modePL->get(PT) == 0)
				score += Bit(brain->readOutput(i));
			else
				score += brain->readOutput(i);
		}
		if (score < 0.0)
			score = 0.0;
		org->dataMap.append("score", score);
		if (visualize)
			std::cout << "organism with ID " << org->ID << " scored " << score
			<< std::endl;
	}
}

void DAGWorld::evaluate(std::map<std::string, std::shared_ptr<Group>>& groups,
	int analyze, int visualize, int debug) {
	int popSize = groups[groupNamePL->get(PT)]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze,
			visualize, debug);
	}
}

std::unordered_map<std::string, std::unordered_set<std::string>>
DAGWorld::requiredGroups() {
	return { {groupNamePL->get(PT),
		  {"B:" + brainNamePL->get(PT) + ",1," +
			  std::to_string(numberOfOutputsPL->get(PT))}} };
	// requires a root group and a brain (in root namespace) and no addtional
	// genome,
	// the brain must have 1 input, and the variable numberOfOutputs outputs
}



