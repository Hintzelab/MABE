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

#include "../../Global.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "../../Genome/CircularGenome/CircularGenome.h"



std::shared_ptr<ParameterLink<int>> DAGWorld::modePL =
Parameters::register_parameter(
	"WORLD_DAG-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
std::shared_ptr<ParameterLink<int>> DAGWorld::resCountPL =
Parameters::register_parameter(
	"WORLD_DAG-resCount", 2, "Number of available resources");
std::shared_ptr<ParameterLink<std::string>> DAGWorld::graphHeightPL =
Parameters::register_parameter(
	"WORLD_DAG-graphHeight", (std::string) "10", "Depth of the graph");
std::shared_ptr<ParameterLink<std::string>> DAGWorld::ccrPL =
Parameters::register_parameter(
	"WORLD_DAG-ccr", (std::string) "0.1", "Communication to computation ratio");
std::shared_ptr<ParameterLink<std::string>> DAGWorld::hetFacPL =
Parameters::register_parameter(
	"WORLD_DAG-hetFac", (std::string) "0.5", "Heterogenuity factor");
std::shared_ptr<ParameterLink<int>> DAGWorld::graphSizePL =
Parameters::register_parameter(
	"WORLD_DAG-graphSize", 50, "Number of nodes in the graph");
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

	//Read the DAG settings
	
	resCount = resCountPL->get(PT);
	graphHeight = graphHeightPL->get(PT);
	ccr = ccrPL->get(PT);
	hetFac = hetFacPL->get(PT);
	graphSize = graphSizePL->get(PT);
	
	std::string exp_folder_name = std::to_string(graphSize) + "_" + std::to_string(resCount) + "_" + graphHeight + "_" +ccr + "_" + hetFac;
	//std::ifstream settingsfile("./DAG_settings.cfg");
	int row_counter = 0;
	string line;
	size_t len = 0;
	std::string nw_filename = "../data/" + exp_folder_name + "/nw.csv";
	std::string ew_filename = "../data/" + exp_folder_name + "/ew.csv";
	std::string bw_filename = "../data/" + exp_folder_name + "/bw.csv";
        /*
	std::vector<std::string> filenames = {};
	int num_tasks = -1;

	while((std::getline(settingsfile, line))) {
		//cout << "Inside node wg loop" << endl;
		std::vector<std::string> row;
		std::stringstream st(line);
		std::string word;
		//cout << "Line: " << line << endl;
		while(getline(st, word, '=')) {
			//cout << "w: " << word << endl;
			row.push_back(word);
		}

		filenames.push_back(row[1]);
		
		row_counter++;
	}
	*/
	//nw_filename = nw_filename + filenames[0] + ".csv";
	int num_tasks = -1;
	std::ifstream nwfile(nw_filename); // ./data/node_weights.csv
	
	//ew_filename = ew_filename + filenames[1] + ".csv";
	std::ifstream ewfile(ew_filename); // ./data/edge_weights.csv

	//bw_filename = bw_filename + filenames[2] + ".csv";
	std::ifstream bwfile(bw_filename); // "./data/bandwidth.csv"

	num_tasks = graphSize;
	/*
	FILE* fp = fopen("./data/node_weights.csv", "r");
	if (fp == NULL) {
		std::cout << "FAILED LOADING NODE_WEIGHTS FILE" << std::endl;
    	exit(EXIT_FAILURE);
    }
    FILE* fp2 = fopen("./data/edge_weights.csv", "r");
if (fp2 == NULL) {
		std::cout << "FAILED LOADING EDGE_WEIGHTS FILE" << std::endl;
    	exit(EXIT_FAILURE);
    }
    FILE* fp3 = fopen("./data/bandwidth.csv", "r");
	if (fp3 == NULL) {
		std::cout << "FAILED LOADING BW FILE" << std::endl;
    	exit(EXIT_FAILURE);
    }
<<<<<<< HEAD
	*/
	 /*
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
			(node_weights[atoi(tokens[0].c_str())]).push_back(atof(tokens[i].c_str())); 
		}
	}
	line = NULL;
	len = 0;
	*/

	row_counter = 0;
	line;
	len = 0;

	while((std::getline(nwfile, line))) {
		//cout << "Inside node wg loop" << endl;
		std::vector<std::string> row;
		std::stringstream st(line);
		std::string word;
		cout << "Line: " << line << endl;
		while(getline(st, word, ',')) {
			//cout << "w: " << word << endl;
			row.push_back(word);
		}

		node_weights.push_back({});
		for(int i=0; i < row.size(); i++) {
			(node_weights[row_counter]).push_back(atof(row[i].c_str()));
		}
		row_counter++;
	}
	//cout << "Read node weights" << endl;
   	//cout << "Node Weights: " << endl;
   	for(int i=0; i < node_weights.size(); i++) {
   		for(int j=0; j < node_weights[i].size(); j++) {
   			//cout << "Weight/" << i << ": " << node_weights[i][j] << endl;
   		}
   	}
 
   	row_counter = 0;
    string line2;
	len = 0;
	while((std::getline(ewfile, line2))) {
		//cout << "inside edge loop" << endl;
		std::vector<std::string> row;
		std::stringstream st(line2);
		std::string word;
		while(getline(st, word, ',')) {
			row.push_back(word);
		}
		//cout << "rowedge: " << line2 << endl;
		string edge_root = std::to_string(row_counter) + ":";
		for(int i=0; i < row.size(); i++) {
    		if (atof(row[i].c_str()) != 0) {
    			edge_weights[edge_root + std::to_string(i)] = atof(row[i].c_str());
    		}
    	}
    	row_counter++;
		
	}

	//fclose(fp2);
	row_counter = 0;
	string line3;
	len = 0;
	while((std::getline(bwfile, line3))) {
		//cout << "Inside bw loop" << endl;
		std::vector<std::string> row;
		std::stringstream st(line3);
		std::string word;
		cout << "bwline: " << line3 << endl;
		while(getline(st, word, ',')) {

			row.push_back(word);
		}
		bwMat.push_back({});
		for(int i=0; i<row.size(); i++) {
    		bwMat[row_counter].push_back(atof(row[i].c_str())) ;
    	}
    	
    	row_counter++;
	}
   // fclose(fp3);
    //std::cout << "Loaded weights fileS" << std::endl;
    /*
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
			(node_weights[atoi(tokens[0].c_str())]).push_back(atof(tokens[i].c_str())); 
		}
	}
	line = NULL;
	len = 0;
	*/
	//LOAD EDGE WEIGHTS INTO MEMORY (COMMUNICATION COST)
	/*
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
		
		edge_weights[tokens[0]] = atof(tokens[1].c_str());
		
		
	}
	fclose(fp2);
	*/
	
	
    //fclose(fp3);


	int key, val;

	g = new Graph(num_tasks, node_weights, edge_weights, bwMat);
	g->compPreds();
	g->ranku();
	//g->scheduleLength();
	// columns to be added to ave file
	popFileColumns.clear();
	popFileColumns.push_back("SL");
	popFileColumns.push_back("score");
	popFileColumns.push_back("score_VAR"); // specifies to also record the
										   // variance (performed automatically
										   // because _VAR)
}


void DAGWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze,
	int visualize, int debug, int update) {

		//cout << "I'm in evalSolo" << endl;

		if (node_weights.empty()) {
			cout << "Bad Read" << endl;
		}
		if (Global::update == 0) {
			//Read the weight-map
			//std::cout <<  "Node 1: " << node_weights[0][0] << std::endl;
			//std::cout <<  "Node 2: " << node_weights[1][0] << std::endl;
			//std::cout << "Edge 0:1 : " << edge_weights["0:1"] << std::endl;
			// Define new genome
			// Constructor(alp_size, genome_size ,PT)
			org->genomes["mapping::"] = std::make_shared<CircularGenome<int>>(resCount, graphSize, PT);
			org->genomes["mapping::"]->fillConstant(0);
			
		}
		
		//cout << "Passed the init phase" << endl;

		auto genome_map = std::dynamic_pointer_cast<CircularGenome<int>>(org->genomes["mapping::"]);
		//auto genome_order = std::dynamic_pointer_cast<CircularGenome<int>>(org->genomes["order::"]);
		

		//Get the genome size
		int numTasks = genome_map->size();

		//std::cout << "NumTasks: " << numTasks << std::endl;
		//Get the task weights and mappings into vectors
		//std::vector<int> taskWeights = {};
		std::vector<int> taskMapping = {};

		for(int i = 0; i < numTasks; i++) {
			//taskWeights.push_back(node_weights[i][genome_map->sites[i]]);
			taskMapping.push_back(genome_map->sites[i]);
		}
		//Graph g(numTasks, node_weights, edge_weights, bwMat);

		//Calculate score for genome and store in data map
		
		//int score_order = std::accumulate(genome_order->sites.begin(),genome_order->sites.end(),0.0);
		//cout << "Just Before longestpath" << endl;
		vector<double> longest_paths = g->longestPath(0, taskMapping);
		//cout << "TaskMapping0" << taskMapping[0];
		//taskMapping = {0, 0, 1, 0, 1, 0, 1, 0, 0, 1 };
		g->setProcMap(taskMapping);
		double testSL = g->scheduleLength();
		
		//cout << "ST: " << testSL << endl; 

		double longestPath = longest_paths[longest_paths.size()-1]; //get the distance from the Final node in the DAG
		double score = 1/testSL + 1/longestPath;
		
		//cout << score << endl;
		org->dataMap.append("score", 1/testSL);
		org->dataMap.append("lp", 1/longestPath);
		org->dataMap.append("SL", testSL);
		
		//org->dataMap.append("lp", lp)
		if(Global::update % 1000 == 0 && org->ID == 150150) {
			std::cout << "Longest Path: " << longest_paths[graphSize-1] << std::endl;
			std::cout << "Schedule Length: " << std::endl;
			std::cout << testSL << std::endl;
			std::cout << "Schedule: " << std::endl;
			g->printPrcSchd();
			std::cout << "Final Genome: " << std::endl;
			genome_map->printGenome();
			std::cout << "Organism ID: " << org->ID << endl;

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



