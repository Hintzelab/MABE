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
#include "Graph.h"
#include "../../Global.h"
#include <stdlib.h>
#include <string>
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

	FILE* fp = fopen("/Users/dogadikbayir/Coursework/CSE845/MABE/World/DAGWorld/node_weights.txt", "r");
	if (fp == NULL) {
		std::cout << "FAILED LOADING NODE_WEIGHTS FILE" << std::endl;
    	exit(EXIT_FAILURE);
    }
    FILE* fp2 = fopen("/Users/dogadikbayir/Coursework/CSE845/MABE/World/DAGWorld/edge_weights.txt", "r");
	if (fp2 == NULL) {
		std::cout << "FAILED LOADING EDGE_WEIGHTS FILE" << std::endl;
    	exit(EXIT_FAILURE);
    }

    std::cout << "Loaded weights fileS" << std::endl;
    char* line = NULL;
	size_t len = 0;
	//LOAD THE NODE WEIGHTS (TASK EXECUTION TIMES INTO MEMORY)
	while ((getline(&line, &len, fp)) != -1) {
    	// using printf() in all tests for consistency
    	//printf("%s\n", line);
    	std::vector<std::string> tokens;
		//std::cout << "Here is the line: " << line << std::endl;
		std::stringstream st(line);
		std::string tok;
		while(getline(st, tok, ' ')) {
			//std::cout <<  << std::endl;
			tokens.push_back(tok);
		}
		
		node_weights[atoi(tokens[0].c_str())] = {};
		
		for (int i = 1; i < tokens.size(); ++i)
		{
			(node_weights[atoi(tokens[0].c_str())]).push_back(atoi(tokens[i].c_str())); 
		}
	}
	line = NULL;
	len = 0;
	//LOAD EDGE WEIGHTS INTO MEMORY (COMMUNICATION COST)
	while ((getline(&line, &len, fp2)) != -1) {
    	// using printf() in all tests for consistency
    	//printf("%s\n", line);
    	std::vector<std::string> tokens;
		//std::cout << "Here is the line: " << line << std::endl;
		std::stringstream st(line);
		std::string tok;
		while(getline(st, tok, ' ')) {
			//std::cout <<  << std::endl;
			tokens.push_back(tok);
		}
		
		edge_weights[tokens[0]] = atoi(tokens[1].c_str());
		
		
	}
	fclose(fp2);

	if (line)
    	free(line);

	

	std::cout << "Loaded weights file" << std::endl;
	int key, val;
	/*
	for(std::string str; getline(&line, &len, fp);) {
		//parse the line 
		std::vector<std::string> tokens;
		std::cout << "Here is the line: " << str << std::endl;
		std::stringstream st(str);
		std::string tok;
		while(getline(st, tok, ' ')) {
			//std::cout <<  << std::endl;
			tokens.push_back(tok);
		}
		
		weights[atoi(tokens[0].c_str())] = {};
		
		for (int i = 1; i < tokens.size(); ++i)
		{
			(weights[atoi(tokens[0].c_str())]).push_back(atoi(tokens[i].c_str())); 
		}

	}
	*/
	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the
										   // variance (performed automatically
										   // because _VAR)
}


void DAGWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
	int visualize, int debug, int update) {
		if (Global::update == 0) {
			//Read the weight-map
			std::cout <<  "Node 1: " << node_weights[0][0] + node_weights[0][1] << std::endl;
			std::cout <<  "Node 2: " << node_weights[1][0] + node_weights[1][1] << std::endl;
			std::cout << "Edge 0:1 : " << edge_weights["0:1"] << std::endl;
			// Define new genome
			// Constructor(alp_size, genome_size ,PT)
			org->genomes["mapping::"] = std::make_shared<CircularGenome<int>>(3, 6, PT);
			org->genomes["mapping::"]->fillConstant(0);
			org->genomes["order::"] = std::make_shared<CircularGenome<int>>(2, 2, PT);
			org->genomes["order::"]->fillConstant(0);
		}
		
		auto genome_map = std::dynamic_pointer_cast<CircularGenome<int>>(org->genomes["mapping::"]);
		//auto genome_order = std::dynamic_pointer_cast<CircularGenome<int>>(org->genomes["order::"]);
		

		//Get the genome size
		int numTasks = genome_map->size();

		std::cout << "NumTasks: " << numTasks << std::endl;
		//Get the task weights and mappings into vectors
		std::vector<int> taskWeights = {};
		std::vector<int> taskMapping = {};

		for(int i = 0; i < numTasks; i++) {
			taskWeights.push_back(node_weights[i][genome_map->sites[i]]);
			taskMapping.push_back(genome_map->sites[i]);
		}
		Graph g(numTasks, taskWeights, edge_weights);

		//Calculate score for genome and store in data map
		
		//int score_order = std::accumulate(genome_order->sites.begin(),genome_order->sites.end(),0.0);

		vector<int> res = g.longestPath(0, taskMapping);
		std::cout << "Longest Path: " << res[5] << std::endl;
		
		double score_map = res[5]; //get the distance from the Final node in the DAG
		double score = 1/score_map;
		
		org->dataMap.append("score", score);
		if(Global::update == 700) {
			std::cout << "Final Genome: " << std::endl;
			genome_map->printGenome();
		}
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
		  {"G:mapping::", "G:order::"} }};
	// requires a single genome
}



