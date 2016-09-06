//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "AbstractGenome.h"

shared_ptr<ParameterLink<string>> AbstractGenome::genomeTypeStrPL = Parameters::register_parameter("GENOME-genomeType", (string) "Circular", "genome to be used in evolution loop, [Multi, Circular]");  // string parameter for outputMethod;
shared_ptr<ParameterLink<double>> AbstractGenome::alphabetSizePL = Parameters::register_parameter("GENOME-alphabetSize", 256.0, "alphabet size for genome");  // string parameter for outputMethod;
shared_ptr<ParameterLink<string>> AbstractGenome::genomeSitesTypePL = Parameters::register_parameter("GENOME-sitesType", (string) "char", "type for sites in genome [char, int, double, bool]");  // string parameter for outputMethod;



