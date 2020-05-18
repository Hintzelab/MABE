//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License


#include "IslandsOptimizer.h"

std::shared_ptr<ParameterLink<std::string>> IslandsOptimizer::IslandNameSpaceListPL =
Parameters::register_parameter(
	"OPTIMIZER_ISLANDS-IslandNameSpaceList", static_cast<std::string>("Tmin::,Roul::,Tmin::Ts50::,Tmax::"),
	"list of name spaces to use for island optimizers\n"
	"make sure to define the optimzers using namespace prefixes:\n"
	"example (add this to settings config:\n"
	"%                                            # close catagory (if one is open)\n"
	"Tmin::OPTIMIZER-optimizer = Tournament       # set Tmin:: to tournament\n"
	"Roul::OPTIMIZER-optimizer = Roulette         # set Roul:: to roulette\n"
	"Tmin::Ts50::OPTIMIZER-optimizer = Tournament # set Tmin::Ts50 to tournament\n"
	"              # this is redundent, because Tmin:: is already tournament\n"
	"Tmax::OPTIMIZER-optimizer = Tournament       # set Tmax::to tournament\n"
	"\n"
	"Tmin::OPTIMIZER_TOURNAMENT-minimizeError = 1 # set Tmin:: to minimize error\n"
	"              # by inheritance, this also sets op1::op3::\n"
	"Tmin::TS50::OPTIMIZER_TOURNAMENT-tournamentSize = 50 # set Tmin::Ts50:: tounamentSize"
);
std::shared_ptr<ParameterLink<double>> IslandsOptimizer::migrationRatePL =
Parameters::register_parameter(
	"OPTIMIZER_ISLANDS-migrationRate", .02,
	"% of new organisms which migrate to a random island at birth");

IslandsOptimizer::IslandsOptimizer(std::shared_ptr<ParametersTable> PT_)
    : AbstractOptimizer(PT_) {

	std::vector<std::string> opNameSpaces;
	convertCSVListToVector(IslandNameSpaceListPL->get(PT), opNameSpaces);

	islands = opNameSpaces.size();
	std::cout << "  setting up IslandOptimizer. Found " << islands << " islands:" << std::endl;
	for (auto& nameSpace : opNameSpaces) {
		auto thisPT = Parameters::root->getTable(nameSpace);
		islandOptimizers.push_back(makeOptimizer(thisPT));

		auto islandType = thisPT->lookupString("OPTIMIZER-optimizer");
		std::cout << "      namespace: " << nameSpace << " island type: ";
	}

	migrationRate = migrationRatePL->get(PT);

	// leave this undefined so that max.csv is not generated
	//optimizeFormula = optimizeValueMT;

	popFileColumns.clear();
	// since diffrent optimizers may generate diffrent values, we will leave this empty
}

void IslandsOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
	std::vector<std::vector<std::shared_ptr<Organism>>> islandPopulations(islands);

	int popSize = static_cast<int>(population.size());
	if (Global::update == 0) {
		for (auto org : population) {
			org->dataMap.set("IsOp_island", Random::getIndex(islands));
		}
		allKeys = population[0]->dataMap.getKeys(); // get all keys from a dataMap before optimizing
		sort(allKeys.begin(), allKeys.end());
	}

	for (auto org : population) {
		islandPopulations[org->dataMap.getIntVector("IsOp_island")[0]].push_back(org);
	}

	population.clear();
	killList.clear();
	std::cout << "\n  optimizing...";
	for (size_t island = 0; island < islands; island++){
		std::cout << "\n    island " << island << " : " << islandOptimizers[island]->PT->getTableNameSpace() << "   (" << islandPopulations[island].size() << ")  ";
		islandOptimizers[island]->optimize(islandPopulations[island]);
		for (auto org : islandPopulations[island]) {
			population.push_back(org);
			if (org->timeOfBirth == Global::update) { // if an org is brand new there is a chance is will migrate
				if (Random::P(migrationRate)) { // chance for migration
					org->dataMap.set("IsOp_island", Random::getIndex(islands));
				}
				else { // stay on your island
					org->dataMap.set("IsOp_island", static_cast<int>(island));
				}
			}
		}
		for (auto org : islandOptimizers[island]->killList) {
			killList.insert(org);
		}
	}

	// now, look at how dataMaps were changed by island optimizers, and figure out what will
	// need to be added so that all orgs have the same values in their data maps
	if (Global::update == 0) {
		for (auto ipop : islandPopulations) {
			// for each island, look at the 0th organisms datamap, and see if it adds any columns
			auto thisIslandsKeys = ipop[0]->dataMap.getKeys();
			sort(thisIslandsKeys.begin(), thisIslandsKeys.end());
			std::vector<std::string> diff;
			std::set_difference(thisIslandsKeys.begin(), thisIslandsKeys.end(),
				allKeys.begin(), allKeys.end(), // allKeys was generated above and already sorted.
				std::inserter(diff, diff.begin()));
			for (auto s : diff) { // add each new column name to fillerLookup with information about wether this column is a number of string
				fillerLookup[s] = ipop[0]->dataMap.lookupDataMapTypeName(ipop[0]->dataMap.findKeyInData(s)) == "string";
			}
		}
		// now we know all the new columns
		for (size_t i = 0; i < islandPopulations.size(); i++) {
			// for each island, again look at the 0th element, but this time, make a list for each island of the
			// columns we will need to add
			fillerKeys.push_back({}); // add an empty vector for this island
			for (auto fillerPair : fillerLookup) {
				auto thisIslandsKeys = islandPopulations[i][0]->dataMap.getKeys();
				if (find(thisIslandsKeys.begin(), thisIslandsKeys.end(), fillerPair.first) == thisIslandsKeys.end()){ // if not found
					fillerKeys[i].push_back(fillerPair.first); // this key is missing from this islands organisms, add it to the fillerKeys for that island
				}
			}
		}
	}

	// add missing columns to dataMaps
	// fillerKeys tells us for each island what we need to add
	// fillerLookup tells us the type of the data that we need to add (0 = number, 1 = string)
	for (auto org : population) {
		for (auto key : fillerKeys[org->dataMap.getIntVector("IsOp_island")[0]]) {
			if (fillerLookup[key] == 0) {
				org->dataMap.set(key, 0);
			}
			else {
				org->dataMap.set(key, (std::string)"---");
			}
		}
	}
}

