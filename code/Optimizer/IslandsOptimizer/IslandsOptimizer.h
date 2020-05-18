//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <module_factories.h>

#include <Optimizer/AbstractOptimizer.h>
#include <Utilities/MTree.h>

#include <iostream>
#include <sstream>


class IslandsOptimizer : public AbstractOptimizer {
public:

	static std::shared_ptr<ParameterLink<std::string>> IslandNameSpaceListPL;
	static std::shared_ptr<ParameterLink<double>> migrationRatePL;

	std::vector <std::shared_ptr<AbstractOptimizer>> islandOptimizers;
	size_t islands;
	double migrationRate;
	std::vector<std::string> allKeys;
	std::vector<std::vector<std::string>> fillerKeys;
	std::map<std::string,int> fillerLookup; // if int = 0 (int,double,bool), write 0, else  (string) write "---"

	std::shared_ptr<Abstract_MTree> nextPopSizeMT;

	IslandsOptimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual void optimize(std::vector<std::shared_ptr<Organism>> &population) override;
};

