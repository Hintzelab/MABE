//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once // directive to insure that this .h file is only included one time

#include "../AbstractOptimizer.h"
#include "../../Utilities/MTree.h"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <vector>
#include <memory>

class NSGAOptimizer : public AbstractOptimizer {
public:
	static std::shared_ptr<ParameterLink<std::string>> optimizeFormulasPL;
	static std::shared_ptr<ParameterLink<std::string>> optimizeFormulaNamesPL;
	static std::shared_ptr<ParameterLink<bool>> recordOptimizeValuesPL;

	std::vector<std::vector<double>> scores;
	std::vector<std::string> scoreNames;
	bool scoresHaveDelta = false;

	std::shared_ptr<Abstract_MTree> nextPopSizeFormula;
	std::vector<std::shared_ptr<Organism>> newPopulation;
	std::vector<std::shared_ptr<Organism>> oldPopulation;

	int numberParents;

	bool recordOptimizeValues;

	std::vector<std::shared_ptr<Abstract_MTree>> optimizeFormulasMTs;

	NSGAOptimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual void optimize(std::vector<std::shared_ptr<Organism>> &population) override;

	virtual void cleanup(std::vector<std::shared_ptr<Organism>> &population) override;

	int NDSSelect(const std::vector<int> &tournamentIndexList);
};
