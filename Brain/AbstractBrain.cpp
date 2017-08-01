//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AbstractBrain.h"

////// BRAIN-brainType is actually set by Modules.h //////
shared_ptr<ParameterLink<string>> AbstractBrain::brainTypeStrPL = Parameters::register_parameter("BRAIN-brainType", (string) "This_string_is_set_by_modules.h", "This_string_is_set_by_modules.h");  // string parameter for outputMethod;
////// BRAIN-brainType is actually set by Modules.h //////

																																																					  //shared_ptr<ParameterLink<int>> AbstractBrain::hiddenNodesPL = Parameters::register_parameter("BRAIN-hiddenNodes", 8, "number of hidden nodes, if brain type supports hiden nodes");  // string parameter for outputMethod;
//shared_ptr<ParameterLink<bool>> AbstractBrain::serialProcessingPL = Parameters::register_parameter("BRAIN-serialProcessing", false, "outputs from units will write to nodes, not nodesNext");
