//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "AbstractBrain.h"

shared_ptr<ParameterLink<string>> AbstractBrain::brainTypeStrPL = Parameters::register_parameter("BRAIN-brainType", (string) "Markov", "brain to be used in evolution loop, [Markov, Wire, Human, ConstantValues]");  // string parameter for outputMethod;
shared_ptr<ParameterLink<int>> AbstractBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN-hiddenNodes", 8, "number of hidden nodes, if brain type supports hiden nodes");  // string parameter for outputMethod;
shared_ptr<ParameterLink<bool>> AbstractBrain::serialProcessingPL = Parameters::register_parameter("BRAIN-serialProcessing", false, "outputs from units will write to nodes, not nodesNext");
