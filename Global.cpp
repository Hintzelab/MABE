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

//global variables that should be accessible to all
shared_ptr<ParameterLink<int>> Global::randomSeedPL = Parameters::register_parameter("GLOBAL-randomSeed", 101, "seed for random number generator, if -1 random number generator will be seeded randomly");
shared_ptr<ParameterLink<int>> Global::updatesPL = Parameters::register_parameter("GLOBAL-updates", 50, "how long the program will run");
//shared_ptr<ParameterLink<int>> Global::popSizePL = Parameters::register_parameter("GLOBAL-popSize", 100, "number of genomes in the population");
shared_ptr<ParameterLink<string>> Global::initPopPL = Parameters::register_parameter("GLOBAL-initPop",(string) "100"  , "initial population to start MABE (if it's a number then those many default organisms will be loaded. If it's a file name with .plf that population loader file is parsed");
shared_ptr<ParameterLink<string>> Global::modePL = Parameters::register_parameter("GLOBAL-mode", (string) "run", "mode to run MABE in [run,visualize]");


shared_ptr<ParameterLink<int>> Global::maxLineLengthPL = Parameters::register_parameter("PARAMETER_FILES-maxLineLength", 180, "maximum line length");
shared_ptr<ParameterLink<int>> Global::commentIndentPL = Parameters::register_parameter("PARAMETER_FILES-commentIndent", 45, "minimum space before comments");

shared_ptr<ParameterLink<string>> Global::visualizePopulationFilePL = Parameters::register_parameter("VISUALIZATION_MODE-visualizePopulationFile", (string) "genome_1000.csv", "which genome file do you wish to load");
shared_ptr<ParameterLink<string>> Global::visualizeOrgIDPL = Parameters::register_parameter("VISUALIZATION_MODE-visualizeOrgIDs", (string)"[-1]", "ID of Genome you would like to visualize. -1 last genome file, -2 all genomes in file (world must support group evaluate)");

shared_ptr<ParameterLink<string>> Global::outputDirectoryPL = Parameters::register_parameter("GLOBAL-outputDirectory", (string) "./", "where files will be written");

//shared_ptr<ParameterLink<string>> Global::groupNameSpacesPL = Parameters::register_parameter("GLOBAL-groups", (string) "[]", "name spaces (also names) of groups to be created (in addition to the default 'no name' space group.)");

//shared_ptr<ParameterLink<int>> Global::bitsPerBrainAddressPL = Parameters::register_parameter("BRAIN-bitsPerBrainAddress", 8, "how many bits are evaluated to determine the brain addresses");
//shared_ptr<ParameterLink<int>> Global::bitsPerCodonPL = Parameters::register_parameter("BRAIN-bitsPerCodon", 8, "how many bits are evaluated to determine the codon addresses");

int Global::update = 0;
