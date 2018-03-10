//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "SimpleOptimizer.h"

using namespace std;


shared_ptr<ParameterLink<string>> SimpleOptimizer::selectionMethodPL = Parameters::register_parameter("OPTIMIZER_SIMPLE-selectionMethod", (string) "Roulette()", "how are parents selected? options: Roullette(),Tournament(size=VAL)\na list may be provided with the formatte:/n[S,S...] where the number of elements is exactly equal to the number of parnets.\nIn this case, the first S will apply to the selection of the first parent, the second S to the second, etc.");
shared_ptr<ParameterLink<int>> SimpleOptimizer::numberParentsPL = Parameters::register_parameter("OPTIMIZER_SIMPLE-numberParents", 1, "number of parents used to produce offspring");

shared_ptr<ParameterLink<string>> SimpleOptimizer::optimizeValuePL = Parameters::register_parameter("OPTIMIZER_SIMPLE-optimizeValue", (string) "DM_AVE[score]", "value to optimize (MTree)\na list may be provided with the formatte:/n[OV,OV...] where the number of elements is exactly equal to the number of parnets.\nIn this case, the first OV will apply to the selection of the first parent, the second OV to the second, etc.");
shared_ptr<ParameterLink<string>> SimpleOptimizer::surviveRatePL = Parameters::register_parameter("OPTIMIZER_SIMPLE-surviveRate", (string) "0", "value between 0 and 1, likelyhood that an organism will self (ignored if numberParents = 1) (MTree)");
shared_ptr<ParameterLink<string>> SimpleOptimizer::selfRatePL = Parameters::register_parameter("OPTIMIZER_SIMPLE-selfRate", (string) "0", "value between 0 and 1, likelyhood that an organism will survive (MTree)");
shared_ptr<ParameterLink<string>> SimpleOptimizer::elitismCountPL = Parameters::register_parameter("OPTIMIZER_SIMPLE-elitismCount", (string) "1", "number of mutated offspring added to next population for each elite organism (MTree)");
shared_ptr<ParameterLink<string>> SimpleOptimizer::elitismRangePL = Parameters::register_parameter("OPTIMIZER_SIMPLE-elitismRange", (string) "0", "number of elite organisms (i.e. if 5, then best 5) (MTree)");

shared_ptr<ParameterLink<string>> SimpleOptimizer::nextPopSizePL = Parameters::register_parameter("OPTIMIZER_SIMPLE-nextPopSize", (string)"-1", "size of population after optimization(MTree). -1 indicates use current population size");

SimpleOptimizer::SimpleOptimizer(shared_ptr<ParametersTable> _PT) : AbstractOptimizer(_PT) {


	numberParents = numberParentsPL->get(PT);

	if (selectionMethodPL->get(PT)[0] == '['){// this is a list of selection methods, break into list
		convertCSVListToVector(selectionMethodPL->get(PT), selectionMethods);
	}
	else { // else, it's just one element, put that in list
		selectionMethods.push_back(selectionMethodPL->get(PT));
	}
	if (selectionMethods.size() != 1 && selectionMethods.size() != numberParents) { // size must 1 (apply this to all), or parent size (apply to each parent in order)
		cout << "OPTIMIZER_SIMPLE-selectionMethod provided a list :\"" << selectionMethodPL->get(PT) << "\" with a bad size (must be either 1, or number of parents).\nexiting." << endl;
		exit(1);
	}

	vector<string> tmp;
	if (optimizeValuePL->get(PT)[0] == '[') {// this is a list of selection optimize values, break into list
		convertCSVListToVector(optimizeValuePL->get(PT), tmp);
	}
	else { // else, it's just one element, put that in list
		tmp.push_back(optimizeValuePL->get(PT));
	}
	for (auto s : tmp) {
		optimizeValueMTs.push_back(stringToMTree(s));
	}
	if (optimizeValueMTs.size() != 1 && optimizeValueMTs.size() != numberParents) {
		cout << "OPTIMIZER_SIMPLE-optimizeValue provided a list :\"" << optimizeValuePL->get(PT) << "\" with a bad size (must be either 1, or number of parents).\nexiting." << endl;
		exit(1);
	}

	surviveRateMT = stringToMTree(surviveRatePL->get(PT));
	selfRateMT = stringToMTree(selfRatePL->get(PT));
	elitismCountMT = stringToMTree(elitismCountPL->get(PT));
	elitismRangeMT = stringToMTree(elitismRangePL->get(PT));
	nextPopSizeMT = stringToMTree(nextPopSizePL->get(PT));

	for (auto selectionMethod : selectionMethods) {
		vector<string> selectorArgs;
		stringstream ss(selectionMethod); // Turn the string into a stream.
		string tok;

		while (getline(ss, tok, '(')) {
			selectorArgs.push_back(tok);
		}

		selectorArgs[1].pop_back();
		//cout << "SimpleOptimizer method: " << selectorArgs[0] << "  " << selectorArgs[1] << endl;
		//cout << selectorArgs.size() << endl;
		if (selectorArgs[0] == "Tournament") {
			selectors.push_back(make_shared<TournamentSelector>(selectorArgs[1], this));
		}
		else if (selectorArgs[0] == "Roulette") {
			selectors.push_back(make_shared<RouletteSelector>(selectorArgs[1], this));
		}
		else {
			cout << "  in SimpleOptimizer constructor, selector method \"" << selectionMethod << "\" is not known.\n  exiting..." << endl;
			exit(1);
		}
	}

	optimizeDMValue = "optimizeValue"; // max will be max this value from dataMap

	popFileColumns.clear();
	popFileColumns.push_back("optimizeValue");
	for (int j = 1; j < (int)optimizeValueMTs.size(); j++) {
		popFileColumns.push_back("optimizeValue_" + to_string(j));
	}

}

void SimpleOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
	oldPopulationSize = (int)population.size();
	/////// MUST update to MTREE
	nextPopulationTargetSize = (int)nextPopSizeMT->eval(PT)[0];

	if (nextPopulationTargetSize == -1) {
		nextPopulationTargetSize = population.size();
	}
	/////// MUST update to MTREE

	nextPopulationSize = 0;

	selfCount = 0;
	eliteCount = 0;
	surviveCount = 0;
	maxScore.clear();
	minScore.clear();
	aveScore.clear();

	// for max and min score we will use one optimizeValueMTs[0]
	// Note: this is only used to report population status
	for (auto mt : optimizeValueMTs) {
		maxScore.push_back(mt->eval(population[0]->dataMap, population[0]->PT)[0]);
		minScore.push_back(maxScore.back());
		aveScore.push_back(0);
	}
	
	elites.clear();
	scores.clear();
	killList.clear();

	for (int i = 0; i < (int)population.size(); i++) {
		scores.push_back({});
		if (Random::P(surviveRateMT->eval(population[i]->dataMap, population[i]->PT)[0])) {
			// does this organism survive?
			surviveCount++;
			nextPopulationSize++;
		}
		else {
			// they did not survive!
			killList.insert(population[i]);
		}
		// for each optimizeValueMT, calculate the score with that MT for this org
		for (int j = 0; j < (int)optimizeValueMTs.size(); j++) {
			scores[i].push_back(optimizeValueMTs[j]->eval(population[i]->dataMap, population[i]->PT)[0]);

			aveScore[j] += scores[i][j];
			if (scores[i][j] > maxScore[j]) {
				maxScore[j] = scores[i][j];
			}
			if (scores[i][j] < minScore[j]) {
				minScore[j] = scores[i][j];
			}
		}
		// lastly, save all the optimizeValues to dataMap
		population[i]->dataMap.set("optimizeValue", scores[i][0]); // we need to have this for the archivist to be able to find max
		for (int j = 1; j < (int)scores[i].size(); j++) {
			population[i]->dataMap.set("optimizeValue_"+to_string(j), scores[i][j]);
		}
	}

	for (int j = 0; j < (int)optimizeValueMTs.size(); j++) {
		aveScore[j] /= oldPopulationSize;
	}

	// in order to pick elites, we need a list of scores. tempScores will be the zeroth score from each org
	vector<double> tempScores;
	for (auto orgScores : scores) {
		tempScores.push_back(orgScores[0]);
	}

	int elitismRange = (int)elitismRangeMT->eval(PT)[0];
	int elitismCount = (int)elitismCountMT->eval(PT)[0];
	for (int i = 0; i < elitismRange; i++) { // get handles for elite orgs
		elites.push_back(findGreatestInVector(tempScores)); // add index of current best to elites
		tempScores[elites.back()] = minScore[0]; // set score for this pick to min (so it's not picked again)
	}
	
	// first add elitism offspring for each of the best elitismRange organisms (assuming there is room)
	int currentElite = 0;
	int currentCopy = 0;
	while ((nextPopulationSize < nextPopulationTargetSize) && (currentElite < min(elitismRange,oldPopulationSize))) {
		currentCopy = 0;
		while ((nextPopulationSize < nextPopulationTargetSize) && (currentCopy < elitismCount)) {
			//population.push_back(population[elites[currentElite]]->makeMutatedOffspringFrom(population[elites[currentElite]]));
			population.push_back(population[elites[currentElite]]->makeCopy());
			nextPopulationSize++;
			eliteCount++;
			currentCopy++;
		}
		currentElite++;
	}

	// now select parents for remainder of population
	vector<shared_ptr<Organism>> parents;
	while (nextPopulationSize < nextPopulationTargetSize) {  // while we have not filled up the next generation
		if (numberParents == 1) {
			auto parent = population[selectors[0]->select(0)];
			population.push_back(parent->makeMutatedOffspringFrom(parent));
		}
		else {
			parents.clear();
			parents.push_back(population[selectors[0]->select(0)]);
			if (Random::P(selfRateMT->eval(parents[0]->dataMap, PT)[0])) {
				population.push_back(parents[0]->makeMutatedOffspringFrom(parents[0]));
			}
			else {
				while ((int)parents.size() < numberParents) {
					if (selectors.size() > 1) { // if there is more then one selector, use the one indexed for this parent
						parents.push_back(population[selectors[parents.size()]->select(parents.size())]);
					}
					else { // use the common selector
						parents.push_back(population[selectors[0]->select(parents.size())]);
					}
				}
				population.push_back(parents[0]->makeMutatedOffspringFromMany(parents));
			}
		}
		nextPopulationSize++;
	}
	cout << "max = " << to_string(maxScore[0]) << "   ave = " << to_string(aveScore[0]);
	for (auto org : population) {
		if (org->timeOfBirth != Global::update) {
			org->dataMap.set("Simple_numOffspring", org->offspringCount);
		}
	}

}

