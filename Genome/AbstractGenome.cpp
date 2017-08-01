//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "AbstractGenome.h"


////// GENOME-genomeType is actually set by Modules.h //////
shared_ptr<ParameterLink<string>> AbstractGenome::genomeTypeStrPL = Parameters::register_parameter("GENOME-genomeType", (string) "This_string_is_set_by_modules.h", "This_string_is_set_by_modules.h");  // string parameter for outputMethod;
////// GENOME-genomeType is actually set by Modules.h //////
shared_ptr<ParameterLink<double>> AbstractGenome::alphabetSizePL = Parameters::register_parameter("GENOME-alphabetSize", 256.0, "alphabet size for genome");  // string parameter for outputMethod;
shared_ptr<ParameterLink<string>> AbstractGenome::genomeSitesTypePL = Parameters::register_parameter("GENOME-sitesType", (string) "char", "type for sites in genome [char, int, double, bool]");  // string parameter for outputMethod;



