//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "NSGAOptimizer.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <memory>


std::shared_ptr<ParameterLink<std::string>> NSGAOptimizer::optimizeFormulasPL =
    Parameters::register_parameter("OPTIMIZER_NSGA-optimizeFormulas",
	(std::string) "DM_AVE[score]",
    "values to optimize with NSGA selection (list of MTrees)\n"
	"example for BerryWorld: [DM_AVE[food1],DM_AVE[food2],(0-DM_AVE[switches])]");

std::shared_ptr<ParameterLink<std::string>> NSGAOptimizer::optimizeFormulaNamesPL =
Parameters::register_parameter("OPTIMIZER_NSGA-optimizeFormulaNames",
	(std::string) "default",
	"column names to associate with optimize formulas in data files."
	"\n'default' will auto generate names as optimizeValue_1, optimizeValue_2, ...");


NSGAOptimizer::NSGAOptimizer(std::shared_ptr<ParametersTable> PT_)
    : AbstractOptimizer(PT_) {


	// Read optimizer formula inputs from settings 
	std::vector<std::string> optimizeFormulasStrings;
	convertCSVListToVector(optimizeFormulasPL->get(PT), optimizeFormulasStrings);

	for (auto s : optimizeFormulasStrings) {
		optimizeFormulasMTs.push_back(stringToMTree(s));
	}

	// Fetch names to use with scores
	if (optimizeFormulaNamesPL->get(PT) == "default") {
		// user has not defined names, auto generate names
		for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
			scoreNames.push_back("optimizeValue_" + std::to_string(fIndex));
		}
	}
	else {
		// user has defined names, use those
		convertCSVListToVector(optimizeFormulaNamesPL->get(PT), scoreNames);
	}

	//optimizeFormula = optimizeFormulasMTs[0];

	popFileColumns.clear();
	for (auto &name : scoreNames) {
		popFileColumns.push_back(name);
	}

}



/**
 * Optimize a given population according to the NSGA-II Procedure
 **/
void NSGAOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {

	std::cout << "initial pop size " << population.size() << std::endl;

	// Initialize score vectors
	std::vector<double> aveScores;
  	aveScores.reserve(optimizeFormulasMTs.size());
  	std::vector<double> maxScores;
  	maxScores.reserve(optimizeFormulasMTs.size());
  	std::vector<double> minScores;
  	minScores.reserve(optimizeFormulasMTs.size());

	// Evaluate individuals and log scores
  	scores.clear();
  	for (auto &opt_formula : optimizeFormulasMTs) {

    	std::vector<double> pop_scores;
    	pop_scores.reserve(population.size());

    	for (auto &org : population)
      		pop_scores.push_back(opt_formula->eval(org->dataMap, PT)[0]);

    	scores.push_back(pop_scores);

		std::vector<int> NDSscores;


    	aveScores.push_back(
        	std::accumulate(std::begin(pop_scores), std::end(pop_scores), 0.0) /
        	population.size());

    	auto const minmax =
        	std::minmax_element(std::begin(pop_scores), std::end(pop_scores));

    	minScores.push_back(*minmax.first);
    	maxScores.push_back(*minmax.second);
  	}

    for (size_t i = 0; i < population.size(); i++)
      for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++)
        population[i]->dataMap.set(scoreNames[fIndex], scores[fIndex][i]);


  	newPopulation.clear();
  	newPopulation.reserve(population.size());

	// If this is the first run, generate offspring via tournament selection and mutation
	// Children will not have scores so we can't run algorithm immediately
	if(oldPopulation.size() == 0){
	
		oldPopulation = population;
		auto selector = std::make_shared<TournamentSelector>(this);
		
		// now select parents for remainder of population
		std::vector<std::shared_ptr<Organism>> parents;
		int popSize = 0;
		while (popSize < population.size()) { 
				auto parent = population[selector->select()]; 
				newPopulation.push_back(parent->makeMutatedOffspringFrom(parent)); // push to population
			
			popSize++;
		}

		return;
	}


	// Create combined population and log scores
	combinedPopulation.insert(combinedPopulation.begin(), oldPopulation.begin(), oldPopulation.end());
	combinedPopulation.insert(combinedPopulation.end(), population.begin(), population.end());
	combinedScores = previousScores;
	for (int i = 0; i < combinedScores.size(); i++){
		combinedScores.at(i).insert(combinedScores.at(i).end(), scores.at(i).begin(), scores.at(i).end());
	}
	
	std::cout << "Combined population size " << combinedPopulation.size() <<std::endl;
	std::cout << "Combined scores size " << combinedScores.size() << std::endl;

	// Generate pareto-optimal fronts using fastNDS
	fastNDS(combinedPopulation);
	std::cout << "Number of Pareto Fronts " << paretoFronts.size() << std::endl;

	crowdingDistance(population);
	combinedCrowdingDistance.clear();
	combinedCrowdingDistance.insert(combinedCrowdingDistance.begin(), previousCrowdingDistance.begin(), previousCrowdingDistance.end());
	combinedCrowdingDistance.insert(combinedCrowdingDistance.end(), currentCrowdingDistance.begin(), currentCrowdingDistance.end());
	previousCrowdingDistance = currentCrowdingDistance;
	std::cout << "Combined crowding distance size " << combinedCrowdingDistance.size() << std::endl;


	
	std::vector<int> selected;
	
	// Fill newParent with initial fronts
	int frontNum = 0;
	int filled = 0;
	int index;
	newParent.clear();
	std::cout << "front 1 size: " << paretoFronts[0].size() << std::endl;
	while(filled + paretoFronts[frontNum].size() < population.size()){
		for(index=0; index < paretoFronts[frontNum].size(); index++){
			int popIndex = paretoFronts[frontNum][index];
			selected.push_back(popIndex);
			newParent.push_back(combinedPopulation[popIndex]);
			filled += 1;
		}
		frontNum += 1;
	}

	std::cout << "Filled initial fronts, current size: " << filled << std::endl;

	// We know this next front will overfill the new population
	// Create map of crowding distance for each entry of this front
	std::map<int, double>cds;
	for(int i=0; i< paretoFronts[frontNum].size(); i++){
		int loc = paretoFronts[frontNum][i];
		double cd = currentCrowdingDistance[loc];
		cds[loc] = cd;
		//std::cout << loc << " " << cd << std::endl;
	}

	std::cout << "Generated cds map of size " << cds.size() << std::endl;

	// Sort cds according to the second value (distance)
	// Will use a custom comparator and set to store the result
	typedef std::function<bool(std::pair<int,double>,std::pair<int,double>)>Comparator;

	Comparator cdComp = [](std::pair<int,double> l, std::pair<int,double> r){
		return l.second >= r.second;
	};

	std::set<std::pair<int,double>,Comparator> sortedCD(cds.begin(),cds.end(),cdComp);
	std::cout <<"SORTED CD LENGTH " << sortedCD.size() << std::endl;

	for(std::pair<int,double> entry: sortedCD){
		//std::cout << entry.first << " " << entry.second << std::endl;
			
		if(filled < population.size()){
			selected.push_back(entry.first);
			newParent.push_back(combinedPopulation[entry.first]);
			filled += 1;
		}
	}


	std::sort(selected.begin(),selected.end());
	int unique = std::unique(selected.begin(),selected.end()) - selected.begin();
	std::cout << "Number of unique selected indeces" << unique << std::endl;
	//std::cout << "unique " << unique << std::endl;
	std::cout <<"Filled new population with size " << newParent.size() << std::endl;
	// Fill kill population with remaining organisms
	killPopulation.clear();
	for(int i=0; i < combinedPopulation.size(); ++i){
		if ((std::find(selected.begin(), selected.end(), i) == selected.end())) {
			killPopulation.push_back(combinedPopulation[i]);
		}
	}
	std::cout << "Number of selected individuals " << selected.size() << std::endl;
	std::cout << "Number of unselected individuals " << killPopulation.size() << std::endl;
	std::cout << "Generating offspring " << std::endl;

	// Generate offspring from newParent with crowding distance tournament selection


		auto selector = std::make_shared<TournamentSelector>(this);
		
		// now select parents for remainder of population
		std::vector<std::shared_ptr<Organism>> parents;
		int popSize = 0;
		while (popSize < population.size()) { 
				auto parent = newParent[selector->select()]; 
				newPopulation.push_back(parent->makeMutatedOffspringFrom(parent)); // push to population
			
			popSize++;
		}

	// Reason to store in population is because population and newPopulation are swapped in cleanup
	population = newParent;
	oldPopulation = population;

	for (size_t fIndex = 0; fIndex < optimizeFormulasMTs.size(); fIndex++) {
		std::cout << std::endl
				<< "   " << scoreNames[fIndex]
				<< ":  max = " << std::to_string(maxScores[fIndex])
				<< "   ave = " << std::to_string(aveScores[fIndex]) << std::flush;
	}
}




void NSGAOptimizer::cleanup(std::vector<std::shared_ptr<Organism>> &population) {
	for (auto org : killPopulation) {
		org->kill();
	}
	killPopulation.clear();
	population.swap(newPopulation);
	previousScores = scores;
	scores.clear();

	combinedPopulation.clear();
	combinedScores.clear();
}

// Generate vector of Pareto-optimal fronts from combined population  
void NSGAOptimizer::fastNDS(std::vector<std::shared_ptr<Organism>> &input){

	// Initialize Pareto Front storage
	paretoFronts.clear();
	std::vector<int> a;
	paretoFronts.push_back(a);
	

	// Number of organisms which dominate a given organism
	dominationCount.clear();
	// Set of organisms which a given organism dominates
	dominatingSet.clear();

	// Track number of organisms stored in Fronts
	int stored=0;

	// Compute dominationCount and dominatingSet for each organism 
	for (int i=0; i<input.size();i++){


		std::vector<int> empty;
		dominatingSet.push_back(empty);
		dominationCount.push_back(0);
		
		// Compare scores to every other organism 
		for (int j=0; j<input.size();j++){

			bool p_greater = false;
			bool q_greater = false;

			// Look for domination in each scoring category
      		for (int score_cat=0; score_cat<combinedScores.size(); score_cat++){
			
				 double p_score = combinedScores[score_cat][i];
				 double q_score = combinedScores[score_cat][j];
				 if(p_score > q_score){
					 p_greater = true;
				 }
				 else if(q_score > p_score){
					 q_greater = true;
				 }
			  }

			//if p dominates q
			if (p_greater == true && q_greater == false){
				dominatingSet[i].push_back(j);
			}

			//if q dominates p
			else if (q_greater == true && p_greater == false){
				dominationCount[i] += 1;

			}


		}


		
		// Non-dominated organisms go in the first pareto front
		if(dominationCount.at(i) == 0){
			paretoFronts.at(0).push_back(i);
			frontMap[i] = 0;
			stored += 1;
		}
	}
	
	// Pareto front number
	int k = 0;
	// Build pareto fronts until we can't anymore
	while(stored < input.size()){	
		std::vector<int> next;

		// Loop through each organism in previous pareto front
		for (int previous : paretoFronts[k]){
			// For each organism that was dominated, subtract 1 from their count
			for(int suboptimal : dominatingSet[previous]){
				dominationCount[suboptimal] -= 1;

				// If this organism is now not dominated, queue to go in next pareto front
				if(dominationCount[suboptimal] == 0){
					next.push_back(suboptimal);
					frontMap[suboptimal] = k+1;
				}
			}
			// If there is a front to add, make room for it and push back members
			if(next.size() != 0){
				std::vector<int> b;
				paretoFronts.push_back(b);
				for (auto front : next){
					paretoFronts[k+1].push_back(front);
					stored += 1;
				}
				next.clear();
			}
		}
		k += 1;

	}
}
	
// Compute crowding distance of current population
void NSGAOptimizer::crowdingDistance(std::vector<std::shared_ptr<Organism>> &population){
	currentCrowdingDistance.clear();
	std::vector<int> distance;
	for(int i=0;i<population.size();++i){
		//Find the front this organism belongs to
		int frontNum = frontMap[i];

		//Get individuals scores
		std::vector<double> myScores;
		std::vector<std::vector<double>> scores;
		std::vector<double> score;
		for(int j=0; j<combinedScores.size(); j++){
			myScores.push_back(combinedScores[j][i]);
			score.clear();
			for(auto index : paretoFronts[frontNum]){
				//std::cout << index << std::endl;
				score.push_back(combinedScores[j][index]);
			}
			scores.push_back(score);
		}

		// Sum of closest differences
		double sumDif = 0;
		for(int k=0;k<scores.size();k++){
			double difference = 10000000000000;
			double myScore = myScores[k];
			for (auto score: scores[k]){
				double diff = fabs(myScore-score);
				if ((diff>0) && (diff<difference)){
					difference = diff;
				}



			}
			sumDif += difference;
		}


		//Compute absolute normalized difference in scores
		double distance = (sumDif / scores.size());

		//std::cout << "Crowding Distance: " << distance << std::endl;
		currentCrowdingDistance.push_back(distance);
	}
}
