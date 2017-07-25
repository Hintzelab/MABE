//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#pragma once

#include "../AbstractOptimizer.h"

using namespace std;

class TournamentOptimizer : public AbstractOptimizer {
 public:

	 static shared_ptr<ParameterLink<string>> optimizeFormulaPL;
	 static shared_ptr<ParameterLink<int>> elitismPL;
	static shared_ptr<ParameterLink<int>> tournamentSizePL;

	shared_ptr<ParameterLink<int>> elitismLPL;
	shared_ptr<ParameterLink<int>> tournamentSizeLPL;

	shared_ptr<ParameterLink<int>> popSizeLPL;



	TournamentOptimizer(shared_ptr<ParametersTable> _PT = nullptr) : AbstractOptimizer(_PT) {
		optimizeFormula = (PT == nullptr) ? stringToMTree(optimizeFormulaPL->lookup()) : stringToMTree(PT->lookupString("OPTIMIZER_TOURNAMENT-optimizeValue"));
		elitismLPL = (PT == nullptr) ? elitismPL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT-elitism", PT);
		tournamentSizeLPL = (PT == nullptr) ? tournamentSizePL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT-tournamentSize", PT);
		popSizeLPL = (PT == nullptr) ? Global::popSizePL : Parameters::getIntLink("GLOBAL-popSize", PT);
		// columns to be added to ave file
		popFileColumns.clear();
		popFileColumns.push_back("optimizeValue");
	}

	virtual void optimize(vector<shared_ptr<Organism>> &population) override;

	//virtual string maxValueName() override {
	//	return (PT == nullptr) ? optimizeFormulaPL->lookup() : PT->lookupString("OPTIMIZER_TOURNAMENT-optimizeValue");
	//}
};
