//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include <Optimizer/AbstractOptimizer.h>
#include <Utilities/MTree.h>

#include <iostream>
#include <sstream>

class {{MODULE_NAME}}Optimizer : public AbstractOptimizer {
public:
	static std::shared_ptr<ParameterLink<std::string>> optimizeValuePL; // what value is used to generate

	std::shared_ptr<Abstract_MTree> optimizeValueMT;

	{{MODULE_NAME}}Optimizer(std::shared_ptr<ParametersTable> PT_ = nullptr);

	virtual void optimize(std::vector<std::shared_ptr<Organism>> &population) override;
};

