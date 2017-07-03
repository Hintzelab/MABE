//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License


#include "../AbstractOptimizer.h"
#include "../../Utilities/VectorNd.h"


class Tournament2Optimizer : public AbstractOptimizer {
 public:

	static shared_ptr<ParameterLink<string>> optimizeFormulaPL;
	static shared_ptr<ParameterLink<string>> surviveFormulaPL;
	static shared_ptr<ParameterLink<int>> elitismPL;
	static shared_ptr<ParameterLink<int>> tournamentSizePL;
	static shared_ptr<ParameterLink<double>> selfRatePL;

	shared_ptr<ParameterLink<int>> elitismLPL;
	shared_ptr<ParameterLink<int>> tournamentSizeLPL;
	shared_ptr<ParameterLink<double>> selfRateLPL;

	shared_ptr<ParameterLink<int>> popSizeLPL;

	shared_ptr<Abstract_MTree> surviveFormula;

	Tournament2Optimizer(shared_ptr<ParametersTable> _PT = nullptr) : AbstractOptimizer(_PT) {
		optimizeFormula = (PT == nullptr) ? stringToMTree(optimizeFormulaPL->lookup()) : stringToMTree(PT->lookupString("OPTIMIZER_TOURNAMENT2-optimizeValue"));
		surviveFormula = (PT == nullptr) ? stringToMTree(surviveFormulaPL->lookup()) : stringToMTree(PT->lookupString("OPTIMIZER_TOURNAMENT2-surviveFormula"));
		elitismLPL = (PT == nullptr) ? elitismPL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT2-elitism", PT);
		tournamentSizeLPL = (PT == nullptr) ? tournamentSizePL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT2-tournamentSize", PT);
		selfRateLPL = (PT == nullptr) ? selfRatePL : Parameters::getDoubleLink("OPTIMIZER_TOURNAMENT2-selfRate", PT);

		popSizeLPL = (PT == nullptr) ? Global::popSizePL : Parameters::getIntLink("GLOBAL-popSize", PT);

		popFileColumns.clear();
		popFileColumns.push_back("optimizeValue");
	}

	virtual void optimize(vector<shared_ptr<Organism>> &population) override;

	virtual string maxValueName() override {
		return (PT == nullptr) ? optimizeFormulaPL->lookup() : PT->lookupString("OPTIMIZER_TOURNAMENT2-optimizeValue");
	}
};

