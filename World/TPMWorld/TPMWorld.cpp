//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "TPMWorld.h"
#include "../../Brain/MarkovBrain/MarkovBrain.h"

std::shared_ptr<ParameterLink<int>> TPMWorld::numberOfInputsPL = Parameters::register_parameter("WORLD_TPM-numberOfInputs", 1, "number of inputs in this world");
std::shared_ptr<ParameterLink<int>> TPMWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_TPM-numberOfOutputs", 10, "number of outputs in this world");
std::shared_ptr<ParameterLink<std::string>> TPMWorld::groupNamePL = Parameters::register_parameter("WORLD_TPM_NAMES-groupNameSpace", (std::string) "root::", "namespace of group to be evaluated");
std::shared_ptr<ParameterLink<std::string>> TPMWorld::brainNamePL = Parameters::register_parameter( "WORLD_TPM_NAMES-brainNameSpace", (std::string) "root::", "namespace for parameters used to define brain");

TPMWorld::TPMWorld(std::shared_ptr<ParametersTable> PT_)
    : AbstractWorld(PT_) {

  // columns to be added to ave file
  popFileColumns.clear();
  popFileColumns.push_back("score");
}

void TPMWorld::evaluateSolo(std::shared_ptr<Organism> org, int analyze, int visualize, int debug) {
  auto brain = dynamic_pointer_cast<MarkovBrain>(org->brains[brainNamePL->get(PT)]);
  if (brain == nullptr) {
    std::cout << "Error: The brain type for agents in TPMWorld is not MarkovBrain, but TPMWorld assumes it to be so. Something is misconfigured." << std::endl;
    exit(1);
  }
  std::string tpmString = brain->getTPMdescription();
  org->dataMap.set("TPM", tpmString);
  std::string tpmSampled = brain->getsampledTPM();
  cout << "sample" << endl;
  org->dataMap.set("SampleTPM", tpmSampled);
  org->dataMap.set("score", 0.0);
}

