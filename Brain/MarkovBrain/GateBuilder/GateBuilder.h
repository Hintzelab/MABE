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

#include <bitset>
#include "../../../Utilities/Parameters.h"
#include "../Gate/DeterministicGate.h"
#include "../Gate/EpsilonGate.h"
#include "../Gate/FeedbackGate.h"
#include "../Gate/GPGate.h"
#include "../Gate/NeuronGate.h"
#include "../Gate/ProbabilisticGate.h"
//#include "../Gate/ThresholdGate.h"
#include "../Gate/TritDeterministicGate.h"
#include "../Gate/VoidGate.h"
#include "../Gate/DecomposableGate.h"
#include "../Gate/DecomposableFeedbackGate.h"

class Gate_Builder { // manages what kinds of gates can be built
public:
  static std::shared_ptr<ParameterLink<bool>> usingProbGatePL;
  static std::shared_ptr<ParameterLink<int>> probGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingDetGatePL;
  static std::shared_ptr<ParameterLink<int>> detGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingEpsiGatePL;
  static std::shared_ptr<ParameterLink<int>> epsiGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingVoidGatePL;
  static std::shared_ptr<ParameterLink<int>> voidGateInitialCountPL;

  static std::shared_ptr<ParameterLink<bool>> usingFeedbackGatePL;
  static std::shared_ptr<ParameterLink<int>> feedbackGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingDecomposableFeedbackGatePL;
  static std::shared_ptr<ParameterLink<int>> decomposableFeedbackGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingDecoGatePL;
  static std::shared_ptr<ParameterLink<bool>> decoUse2LevelPL;
  static std::shared_ptr<ParameterLink<bool>> deco2LevelRowFirstPL;
  static std::shared_ptr<ParameterLink<int>> decoGateInitialCountPL;
  static std::shared_ptr<ParameterLink<bool>> usingGPGatePL;
  static std::shared_ptr<ParameterLink<int>> gPGateInitialCountPL;
  static std::shared_ptr<ParameterLink<int>> thGateInitialCountPL;

  static std::shared_ptr<ParameterLink<bool>> usingTritDeterministicGatePL;
  static std::shared_ptr<ParameterLink<int>> tritDeterministicGateInitialCountPL;

  static std::shared_ptr<ParameterLink<bool>> usingNeuronGatePL;
  static std::shared_ptr<ParameterLink<int>> neuronGateInitialCountPL;

  static std::shared_ptr<ParameterLink<int>> bitsPerBrainAddressPL; // how many bits
                                                               // are evaluated
                                                               // to determine
                                                               // the brain
                                                               // addresses.
  static std::shared_ptr<ParameterLink<int>> bitsPerCodonPL;

  std::set<int> inUseGateTypes;
  std::set<std::string> inUseGateNames;
  std::vector<std::vector<int>> gateStartCodes;

  std::map<int, int> intialGateCounts;

  const std::shared_ptr<ParametersTable> PT;

  std::vector<std::function<std::shared_ptr<AbstractGate>(
      std::shared_ptr<AbstractGenome::Handler>, int gateID,
      std::shared_ptr<ParametersTable> gatePT)>> makeGate;

  // the following "Codes" are identifiers for different gate types. These
  // numbers are used to look up constructors, and also as the first half of the
  // start codon, if codons are being used to generate gates.   More may be
  // added, but the current numbers MUST not change (if they do,   then genomes
  // will generate different brains! and is not   backward-compatible)
  enum Codes {
    Probabilistic = 42,
    Deterministic = 43,
    Epsilon = 44,
    Void = 45,
    GP = 46,
    TritDeterministic = 47,
    Neuron = 48,
    Feedback = 49,
    Decomposable = 50,
    DecomposableFeedback = 51
  };

  // Gate_Builder() = default;
  Gate_Builder(std::shared_ptr<ParametersTable> PT_ = nullptr)
      : PT(PT_){
    setupGates();
  }

  ~Gate_Builder() = default;

  void AddGate(int gateType,
               std::function<std::shared_ptr<AbstractGate>(
                   std::shared_ptr<AbstractGenome::Handler>, int gateID,
                   std::shared_ptr<ParametersTable> gatePT)> theFunction);
  void setupGates();

  // int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler,
  // shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain
  // state value address from a genome
  static void getSomeBrainAddresses(
      const int &howMany, const int &howManyMax, std::vector<int> &addresses,
      std::shared_ptr<AbstractGenome::Handler> genomeHandler, int code,
      int gateID,
      std::shared_ptr<ParametersTable>); // extracts many brain state value
                                         // addresses from a genome

  static std::pair<std::vector<int>, std::vector<int>> getInputsAndOutputs(
      const std::pair<int, int> insRange, const std::pair<int, int>,
      std::shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID,
      std::shared_ptr<ParametersTable> ); // extracts the input and output brain
                                        // state value addresses for this gate
  static std::pair<std::vector<int>, std::vector<int>>
  getInputsAndOutputs(const std::string IO_ranges, int &inMax, int &outMax,
                      std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                      int gateID, std::shared_ptr<ParametersTable> ,
                      const std::string featureName = "undefined");

  void constructDecomposableFeedbackGates(int);
  void constructFeedbackGates(int);
  void constructNeuronGates(int);
  void constructTritDeterministicGates(int);
  void constructGPGates(int);
  void constructVoidGates(int);
  void constructEpsilonGates(int);
  void constructDeterministicGates(int);
  void constructDecomposableGates(int);
  void constructProbabilisticGates(int);
  /* *** some c++ 11 magic to speed up translation from genome to gates *** */
  // function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler>
  // genomeHandler, int gateID)> Gate_Builder::makeGate[256];
};

