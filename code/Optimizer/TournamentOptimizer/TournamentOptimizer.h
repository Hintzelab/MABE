//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../AbstractOptimizer.h"
#include "../../Utilities/MTree.h"

#include <iostream>
#include <sstream>

class TournamentOptimizer : public AbstractOptimizer {
public:
	static std::shared_ptr<ParameterLink<int>> tournamentSizePL; // TournamentSize
	static std::shared_ptr<ParameterLink<int>> numberParentsPL; // number of parents (default 1, asexual)
	static std::shared_ptr<ParameterLink<std::string>> optimizeValuePL; // what value is used to generate
	static std::shared_ptr<ParameterLink<bool>> minimizeErrorPL; // If true, lower optimizeValue will be prefered

	int tournamentSize;
	int numberParents;
	bool minimizeError;
	std::shared_ptr<Abstract_MTree> optimizeValueMT;

	int selectParent(int tournamentSize, bool minimizeError, std::vector<double> scores, int popSize);

	TournamentOptimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual void optimize(std::vector<std::shared_ptr<Organism>> &population) override;
};

