//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "Global.h"

// global variables that should be accessible to all
std::shared_ptr<ParameterLink<int>> Global::randomSeedPL =
    Parameters::register_parameter("GLOBAL-randomSeed", 101,
                                   "seed for random number generator, if -1 "
                                   "random number generator will be seeded "
                                   "randomly");
std::shared_ptr<ParameterLink<int>> Global::updatesPL =
    Parameters::register_parameter("GLOBAL-updates", 100,
                                   "how long the program will run");
std::shared_ptr<ParameterLink<std::string>> Global::initPopPL =
    Parameters::register_parameter(
        "GLOBAL-initPop", std::string("100"),
        "initial population to start MABE - this parameter is parsed with .plf syntax as if preceded by \"MASTER = \".\n"
        "a number can be used to set the population size to use random default orgs (i.e. 100 is read as \"default 100\")"
        "If a file name with .plf is provided, that population loader file is parsed");
std::shared_ptr<ParameterLink<std::string>> Global::modePL =
    Parameters::register_parameter(
        "GLOBAL-mode", std::string("run"),
        "mode to run MABE in [run,visualize,analyze]");

std::shared_ptr<ParameterLink<int>> Global::maxLineLengthPL =
    Parameters::register_parameter("PARAMETER_FILES-maxLineLength", 160,
                                   "maximum line length");
std::shared_ptr<ParameterLink<int>> Global::commentIndentPL =
    Parameters::register_parameter("PARAMETER_FILES-commentIndent", 45,
                                   "minimum space before comments");

std::shared_ptr<ParameterLink<std::string>> Global::outputPrefixPL =
    Parameters::register_parameter(
        "GLOBAL-outputPrefix", std::string("./"),
        "Directory and prefix specifying where data files will be written");

// shared_ptr<ParameterLink<string>> Global::groupNameSpacesPL =
// Parameters::register_parameter("GLOBAL-groups", (string) "[]", "name spaces
// (also names) of groups to be created (in addition to the default 'no name'
// space group.)");

// shared_ptr<ParameterLink<int>> Global::bitsPerBrainAddressPL =
// Parameters::register_parameter("BRAIN-bitsPerBrainAddress", 8, "how many bits
// are evaluated to determine the brain addresses");
// shared_ptr<ParameterLink<int>> Global::bitsPerCodonPL =
// Parameters::register_parameter("BRAIN-bitsPerCodon", 8, "how many bits are
// evaluated to determine the codon addresses");

int Global::update = 0;

