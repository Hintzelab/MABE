//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#include "AbstractBrain.h"

shared_ptr<ParameterLink<string>> AbstractBrain::brainTypeStrPL = Parameters::register_parameter("BRAIN-brainType", (string) "Markov", "brain to be used in evolution loop, [Markov, Wire, Human, ConstantValues]");  // string parameter for outputMethod;
shared_ptr<ParameterLink<int>> AbstractBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN-hiddenNodes", 8, "number of hidden nodes, if brain type supports hiden nodes");  // string parameter for outputMethod;
shared_ptr<ParameterLink<bool>> AbstractBrain::serialProcessingPL = Parameters::register_parameter("BRAIN-serialProcessing", false, "outputs from units will write to nodes, not nodesNext");

shared_ptr<AbstractBrain> AbstractBrain::makeCopy(shared_ptr<ParametersTable> _PT) {
	cout << "ERROR IN AbstractBrain::makeCopy() - You are using the abstract copy constructor for brains. You must define your own" << endl;
	exit(1);
}
