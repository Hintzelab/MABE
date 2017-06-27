//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

//  This file was auto-generated with MBuilder.py

#ifndef __AutoBuild__Modules__
#define __AutoBuild__Modules__
#include "World/TestWorld/TestWorld.h"
#include "World/WeedWorld/WeedWorld.h"
#include "World/MorrisTestWorld/MorrisTestWorld.h"
#include "World/BerryWorld/BerryWorld.h"
#include "World/BerryPlusWorld/BerryPlusWorld.h"
#include "World/IPDWorld/IPDWorld.h"
#include "World/NumeralClassifierWorld/NumeralClassifierWorld.h"
#include "World/SOFWorld/SOFWorld.h"
#include "World/MemoryWorld/MemoryWorld.h"
#include "World/RPPWorld/RPPWorld.h"
#include "Genome/CircularGenome/CircularGenome.h"
#include "Genome/MultiGenome/MultiGenome.h"
#include "Brain/MarkovBrain/MarkovBrain.h"
#include "Brain/CGPBrain/CGPBrain.h"
#include "Brain/LSTMBrain/LSTMBrain.h"
#include "Brain/ConstantValuesBrain/ConstantValuesBrain.h"
#include "Brain/GeneticProgramingBrain/GeneticProgramingBrain.h"
#include "Brain/HumanBrain/HumanBrain.h"
#include "Brain/IPDBrain/IPDBrain.h"
#include "Brain/WireBrain/WireBrain.h"
#include "Optimizer/GAOptimizer/GAOptimizer.h"
#include "Optimizer/TournamentOptimizer/TournamentOptimizer.h"
#include "Optimizer/Tournament2Optimizer/Tournament2Optimizer.h"

#include "Archivist/DefaultArchivist.h"
#include "Archivist/LODwAPArchivist/LODwAPArchivist.h"
#include "Archivist/SSwDArchivist/SSwDArchivist.h"


//create a world
shared_ptr<AbstractWorld> makeWorld(shared_ptr<ParametersTable> PT = Parameters::root){
  shared_ptr<AbstractWorld> newWorld;
  bool found = false;
  string worldType = (PT == nullptr) ? AbstractWorld::worldTypePL->lookup() : PT->lookupString("WORLD-worldType");
  if (worldType == "Test") {
    newWorld = make_shared<TestWorld>(PT);
    found = true;
    }
  if (worldType == "Weed") {
    newWorld = make_shared<WeedWorld>(PT);
    found = true;
    }
  if (worldType == "MorrisTest") {
    newWorld = make_shared<MorrisTestWorld>(PT);
    found = true;
    }
  if (worldType == "Berry") {
    newWorld = make_shared<BerryWorld>(PT);
    found = true;
    }
  if (worldType == "BerryPlus") {
    newWorld = make_shared<BerryPlusWorld>(PT);
    found = true;
    }
  if (worldType == "IPD") {
    newWorld = make_shared<IPDWorld>(PT);
    found = true;
    }
  if (worldType == "NumeralClassifier") {
    newWorld = make_shared<NumeralClassifierWorld>(PT);
    found = true;
    }
  if (worldType == "SOF") {
    newWorld = make_shared<SOFWorld>(PT);
    found = true;
    }
  if (worldType == "Memory") {
    newWorld = make_shared<MemoryWorld>(PT);
    found = true;
    }
  if (worldType == "RPP") {
    newWorld = make_shared<RPPWorld>(PT);
    found = true;
    }
  if (!found){
    cout << "  ERROR! could not find WORLD-worldType \"" << worldType << "\".\n  Exiting." << endl;
    exit(1);
    }
  return newWorld;
}


//create an optimizer
shared_ptr<AbstractOptimizer> makeOptimizer(shared_ptr<ParametersTable> PT = Parameters::root){
  shared_ptr<AbstractOptimizer> newOptimizer;
  bool found = false;
  string optimizerType = (PT == nullptr) ? AbstractOptimizer::Optimizer_MethodStrPL->lookup() : PT->lookupString("OPTIMIZER-optimizer");
  if (optimizerType == "GA") {
    newOptimizer = make_shared<GAOptimizer>(PT);
    found = true;
    }
  if (optimizerType == "Tournament") {
    newOptimizer = make_shared<TournamentOptimizer>(PT);
    found = true;
    }
  if (optimizerType == "Tournament2") {
    newOptimizer = make_shared<Tournament2Optimizer>(PT);
    found = true;
    }
  if (!found){
    cout << "  ERROR! could not find OPTIMIZER-optimizer \"" << optimizerType << "\".\n  Exiting." << endl;
    exit(1);
    }
  return newOptimizer;
}


//create an archivist
shared_ptr<DefaultArchivist> makeArchivist(vector<string> popFileColumns, shared_ptr<Abstract_MTree> _maxFormula, shared_ptr<ParametersTable> PT = Parameters::root, string groupPrefix = ""){
  shared_ptr<DefaultArchivist> newArchivist;
  bool found = false;
  string archivistType = (PT == nullptr) ? DefaultArchivist::Arch_outputMethodStrPL->lookup() : PT->lookupString("ARCHIVIST-outputMethod");
  if (archivistType == "LODwAP") {
    newArchivist = make_shared<LODwAPArchivist>(popFileColumns, _maxFormula, PT, groupPrefix);
    found = true;
    }
  if (archivistType == "SSwD") {
    newArchivist = make_shared<SSwDArchivist>(popFileColumns, _maxFormula, PT, groupPrefix);
    found = true;
    }
  if (archivistType == "Default") {
    newArchivist = make_shared<DefaultArchivist>(popFileColumns, _maxFormula, PT, groupPrefix);
    found = true;
    }
  if (!found){
    cout << "  ERROR! could not find ARCHIVIST-outputMethod \"" << archivistType << "\".\n  Exiting." << endl;
    exit(1);
    }
  return newArchivist;
}


//create a template genome
shared_ptr<AbstractGenome> makeTemplateGenome(shared_ptr<ParametersTable> PT = nullptr){
  shared_ptr<AbstractGenome> newGenome;
  bool found = false;
  string genomeType = (PT == nullptr) ? AbstractGenome::genomeTypeStrPL->lookup() : PT->lookupString("GENOME-genomeType");
  if (genomeType == "Circular") {
    newGenome = CircularGenome_genomeFactory(PT);
    found = true;
    }
  if (genomeType == "Multi") {
    newGenome = MultiGenome_genomeFactory(PT);
    found = true;
    }
  if (found == false){
    cout << "  ERROR! could not find GENOME-genomeType \"" << genomeType << "\".\n  Exiting." << endl;
    exit(1);
    }
  return newGenome;
}


//create a template brain
shared_ptr<AbstractBrain> makeTemplateBrain(int inputs, int outputs, shared_ptr<ParametersTable> PT = nullptr){
  shared_ptr<AbstractBrain> newBrain;
  bool found = false;
  string brainType = (PT == nullptr) ? AbstractBrain::brainTypeStrPL->lookup() : PT->lookupString("BRAIN-brainType");
  if (brainType == "Markov") {
    newBrain = MarkovBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "CGP") {
    newBrain = CGPBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "LSTM") {
    newBrain = LSTMBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "ConstantValues") {
    newBrain = ConstantValuesBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "GeneticPrograming") {
    newBrain = GeneticProgramingBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "Human") {
    newBrain = HumanBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "IPD") {
    newBrain = IPDBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (brainType == "Wire") {
    newBrain = WireBrain_brainFactory(inputs, outputs, PT);
    found = true;
    }
  if (found == false){
    cout << "  ERROR! could not find BRAIN-brainType \"" << brainType << "\".\n  Exiting." << endl;
    exit(1);
    }
  return newBrain;
}


//configure Defaults and Documentation
void configureDefaultsAndDocumentation(){
  Parameters::root->setParameter("BRAIN-brainType", (string)"Markov");
  Parameters::root->setDocumentation("BRAIN-brainType", "brain to be used, [Markov, CGP, LSTM, ConstantValues, GeneticPrograming, Human, IPD, Wire]");

  Parameters::root->setParameter("GENOME-genomeType", (string)"Circular");
  Parameters::root->setDocumentation("GENOME-genomeType", "genome to be used, [Circular, Multi]");

  Parameters::root->setParameter("ARCHIVIST-outputMethod", (string)"LODwAP");
  Parameters::root->setDocumentation("ARCHIVIST-outputMethod", "output method, [LODwAP, SSwD, Default]");

  Parameters::root->setParameter("OPTIMIZER-optimizer", (string)"GA");
  Parameters::root->setDocumentation("OPTIMIZER-optimizer", "optimizer to be used, [GA, Tournament, Tournament2]");

  Parameters::root->setParameter("WORLD-worldType", (string)"Test");
  Parameters::root->setDocumentation("WORLD-worldType","world to be used, [Test, Weed, MorrisTest, Berry, BerryPlus, IPD, NumeralClassifier, SOF, Memory, RPP]");
}


#endif /* __AutoBuild__Modules__ */
