//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License


#include "../SimpleOptimizer/SimpleOptimizer.h"
#include "IslandsOptimizer.h"
#include "../../Utilities/CSV.h"

std::shared_ptr<ParameterLink<std::string>> IslandsOptimizer::IslandNameSpaceListPL =
Parameters::register_parameter(
	"OPTIMIZER_ISLANDS-IslandNameSpaceList", static_cast<std::string>("[op1::,op2::,op3::,op4::,op5::]"),
	"list of name spaces to use for island optimizers");
std::shared_ptr<ParameterLink<double>> IslandsOptimizer::migrationRatePL =
Parameters::register_parameter(
	"OPTIMIZER_ISLANDS-migrationRate", .02,
	"% of new organisms which migrate to a random island at birth");

IslandsOptimizer::IslandsOptimizer(std::shared_ptr<ParametersTable> PT_)
    : AbstractOptimizer(PT_) {

    CSVReader reader;
    auto island_ns_list = IslandNameSpaceListPL->get(PT);
    auto opNameSpaces =
        reader.parseLine(island_ns_list.substr(1, island_ns_list.size() - 2));

    for (auto &nameSpace : opNameSpaces) {
      islandOptimizers.push_back(std::make_shared<SimpleOptimizer>(
          nameSpace == "root::" ? Parameters::root
                                : Parameters::root->getTable(nameSpace)));
	}
	islands = islandOptimizers.size();
	migrationRate = migrationRatePL->get(PT);
	std::cout << "  IslandOptimizer has " << islands << " islands with names ";
	for (auto& nameSpace : opNameSpaces) {
		std::cout << nameSpace << " ";
	}
	std::cout << std::endl;

	popFileColumns.clear();
	popFileColumns.push_back("optimizeValue");
	//popFileColumns.push_back("IsOp_island");
}

void IslandsOptimizer::optimize(std::vector<std::shared_ptr<Organism>> &population) {
	std::vector<std::vector<std::shared_ptr<Organism>>> islandPopulations(islands);

	int popSize = static_cast<int>(population.size());
	if (Global::update == 0) {
		for (auto org : population) {
			org->dataMap.set("IsOp_island", Random::getIndex(islands));
		}
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
			if (org->timeOfBirth == Global::update) {
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



}

