//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <vector>

#include "Utilities/Parameters.h"

using namespace std;

class Global {
public:
	// parameters
	static shared_ptr<ParameterLink<int>> randomSeedPL;  // seed for random number generator, if -1 random number generator will be seeded randomly
	static shared_ptr<ParameterLink<int>> updatesPL;  // run until there is a MCRA at this time
	//static shared_ptr<ParameterLink<int>> popSizePL;  // number of genomes in the population
	static shared_ptr<ParameterLink<string>> initPopPL;  // number of genomes in the population
	static shared_ptr<ParameterLink<string>> modePL;  // number of genomes in the population

	static shared_ptr<ParameterLink<int>> maxLineLengthPL;  // max length of lines in the parameters files 
	static shared_ptr<ParameterLink<int>> commentIndentPL;  // minimum number of characters before comments 

	static shared_ptr<ParameterLink<string>> visualizePopulationFilePL;
	static shared_ptr<ParameterLink<string>> visualizeOrgIDPL;

	static shared_ptr<ParameterLink<string>> outputDirectoryPL;  // where files will be written

	//static shared_ptr<ParameterLink<string>> groupNameSpacesPL;

//	static shared_ptr<ParameterLink<int>> bitsPerBrainAddressPL;  // how many bits are evaluated to determine the brain addresses.
//	// i.e. ceil(bitsPerBrainAddress/8) = number of genome sites used
//	static shared_ptr<ParameterLink<int>> bitsPerCodonPL;

	//////////
	// Below are non configurable values (not set directly by Parameters.h methods)
	//////////
	static int update;  // the current "time"

};

