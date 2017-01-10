//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__Tournament2_Optimizer__
#define __BasicMarkovBrainTemplate__Tournament2_Optimizer__

#include "../AbstractOptimizer.h"

class Tournament2Optimizer : public AbstractOptimizer {
 public:

	static shared_ptr<ParameterLink<string>> optimizeValuePL;
	static shared_ptr<ParameterLink<int>> elitismPL;
	static shared_ptr<ParameterLink<int>> tournamentSizePL;

	shared_ptr<ParameterLink<string>> optimizeValueLPL;
	shared_ptr<ParameterLink<int>> elitismLPL;
	shared_ptr<ParameterLink<int>> tournamentSizeLPL;

	Tournament2Optimizer(shared_ptr<ParametersTable> _PT = nullptr) : AbstractOptimizer(_PT) {
		optimizeValueLPL = (PT == nullptr) ? optimizeValuePL : Parameters::getStringLink("OPTIMIZER_TOURNAMENT2-optimizeValue", PT);
		elitismLPL = (PT == nullptr) ? elitismPL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT2-elitism", PT);
		tournamentSizeLPL = (PT == nullptr) ? tournamentSizePL : Parameters::getIntLink("OPTIMIZER_TOURNAMENT2-tournamentSize", PT);
		//cout << "Tournament2 Optimizer value is \"" << optimizeValueLPL->lookup() << "\"." << endl;
	}

	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) override;

	virtual string maxValueName() override {
		return(optimizeValueLPL->lookup());
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Tournament2_Optimizer__) */
