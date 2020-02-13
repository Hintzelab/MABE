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
#include "../../Genome/CircularGenome/CircularGenome.h"

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
	int visualize, int debug, int update) {
		if (update == 0) {
			// Define new genome
			org->genomes["root::"] = std::make_shared<CircularGenome<int>>(2, 100, PT);
			org->genomes["root::"]->fillConstant(0);
		}
		
		auto genome = std::dynamic_pointer_cast<CircularGenome<int>>(org->genomes["root::"]);
		
		//Calculate score for genome and store in data map
		int score = std::accumulate(genome->sites.begin(),genome->sites.end(),0.0);

		org->dataMap.append("score", score);
		
		if (visualize)
			std::cout << "organism with ID " << org->ID << " scored " << score
			<< std::endl;
}

void DAGWorld::evaluate(std::map<std::string, std::shared_ptr<Group>>& groups,
	int analyze, int visualize, int debug) {
	int update = Global::update;
	int popSize = groups[groupNamePL->get(PT)]->population.size();
	for (int i = 0; i < popSize; i++) {
		evaluateSolo(groups[groupNamePL->get(PT)]->population[i], analyze,
			visualize, debug, update);
	}

}

std::unordered_map<std::string, std::unordered_set<std::string>>
DAGWorld::requiredGroups() {
	return { {groupNamePL->get(PT),
		  {"G:root::"}} };
	// requires a single genome
}



