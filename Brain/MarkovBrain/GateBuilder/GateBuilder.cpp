//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "GateBuilder.h"
#include <cmath>

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingProbGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_PROBABILISTIC-allow",
                                   false,
                                   "set to true to enable probabilistic gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::probGateInitialCountPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_PROBABILISTIC-initialCount", 3,
        "seed genome with this many start codons");
std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingDetGatePL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_DETERMINISTIC-allow", true,
        "set to true to enable deterministic gates?");
std::shared_ptr<ParameterLink<int>> Gate_Builder::detGateInitialCountPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_DETERMINISTIC-initialCount", 6,
        "seed genome with this many start codons");
std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingEpsiGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-allow", false,
                                   "set to true to enable epsilon gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::epsiGateInitialCountPL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_EPSILON-initialCount", 3,
                                   "seed genome with this many start codons");
std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingVoidGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-allow", false,
                                   "set to true to enable void gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::voidGateInitialCountPL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-initialCount", 3,
                                   "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingGPGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_GENETICPROGRAMING-allow",
                                   false,
                                   "set to true to enable GP (what?) gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::gPGateInitialCountPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_GENETICPROGRAMING-initialCount", 3,
        "seed genome with this many start codons");
// std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingThGatePL =
// Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate", false,
// "set to true to enable threshold gates");
// std::shared_ptr<ParameterLink<int>> Gate_Builder::thGateInitialCountPL =
// Parameters::register_parameter("BRAIN_MARKOV_GATES-thresholdGate_InitialCount",
// 3, "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingTritDeterministicGatePL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_TRIT-allow", false,
        "set to true to enable tritDeterministic gates");
std::shared_ptr<ParameterLink<int>>
    Gate_Builder::tritDeterministicGateInitialCountPL =
        Parameters::register_parameter(
            "BRAIN_MARKOV_GATES_TRIT-initialCount", 3,
            "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingNeuronGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-allow", false,
                                   "set to true to enable Neuron gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::neuronGateInitialCountPL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_NEURON-initialCount", 3,
                                   "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingFeedbackGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_FEEDBACK-allow", false,
                                   "set to true to enable feedback gates");
std::shared_ptr<ParameterLink<int>> Gate_Builder::feedbackGateInitialCountPL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_FEEDBACK-initialCount",
                                   3,
                                   "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingDecoGatePL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_DECOMPOSABLE-allow",
                                   false,
                                   "set to true to enable decomposible gates");
std::shared_ptr<ParameterLink<bool>> Gate_Builder::decoUse2LevelPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_DECOMPOSABLE-use2Level", false,
        "set to true to allow \"super decomposable\" gates");
std::shared_ptr<ParameterLink<bool>> Gate_Builder::deco2LevelRowFirstPL =
    Parameters::register_parameter("BRAIN_MARKOV_GATES_DECOMPOSABLE-rowFirst",
                                   true, "set to true to make second-order "
                                         "decomposable gates operate in "
                                         "row-first expansion");
std::shared_ptr<ParameterLink<int>> Gate_Builder::decoGateInitialCountPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_DECOMPOSABLE-initialCount", 3,
        "seed genome with this many start codons");

std::shared_ptr<ParameterLink<bool>> Gate_Builder::usingDecomposableFeedbackGatePL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_GATES_DECOMPOSABLE_FEEDBACK-allow", false,
        "set to true to enable decomposable feedback gates");
std::shared_ptr<ParameterLink<int>>
    Gate_Builder::decomposableFeedbackGateInitialCountPL =
        Parameters::register_parameter(
            "BRAIN_MARKOV_GATES_DECOMPOSABLE_FEEDBACK-initialCount", 3,
            "seed genome with this many start codons");

std::shared_ptr<ParameterLink<int>> Gate_Builder::bitsPerBrainAddressPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_ADVANCED-bitsPerBrainAddress", 8,
        "how many bits are evaluated to determine the brain addresses");
std::shared_ptr<ParameterLink<int>> Gate_Builder::bitsPerCodonPL =
    Parameters::register_parameter(
        "BRAIN_MARKOV_ADVANCED-bitsPerCodon", 8,
        "how many bits are evaluated to determine the codon addresses");

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax"
// addresses and updates "codingRegions" with the addresses being used.
void Gate_Builder::getSomeBrainAddresses(
    const int &howMany, const int &howManyMax, std::vector<int> &addresses,
    std::shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID,
    std::shared_ptr<ParametersTable> PT_) {
  auto i = 0;
  while (i < howMany) // for the number of addresses we need
    addresses[i++] =
        genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(PT_)) - 1,
                               code, gateID); // get an address

  while (i++ < howManyMax) // leave room in the genome in case this gate gets
                           // more IO later
    genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(PT_)) - 1);
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many
// inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or
// outputs skip the unused sites in the genome
std::pair<std::vector<int>, std::vector<int>> Gate_Builder::getInputsAndOutputs(
    const std::pair<int, int> insRange, const std::pair<int, int> outsRange,
    std::shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID,
    std::shared_ptr<ParametersTable>
        PT_) { // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

  auto numInputs = genomeHandler->readInt(insRange.first, insRange.second,
                                          AbstractGate::IN_COUNT_CODE, gateID);
  // cout << "num_Inputs: " << numInputs << "\n";
  auto numOutputs = genomeHandler->readInt(outsRange.first, outsRange.second,
                                          AbstractGate::OUT_COUNT_CODE, gateID);
  // cout << "num_Outputs: " << numOutputs << "\n";
  std::vector<int> inputs(numInputs);
  std::vector<int> outputs(numOutputs);

  if (insRange.second > 0) 
    getSomeBrainAddresses(numInputs, insRange.second, inputs, genomeHandler,
                          AbstractGate::IN_ADDRESS_CODE, gateID, PT_);
  
  if (outsRange.second > 0) 
    getSomeBrainAddresses(numOutputs, outsRange.second, outputs, genomeHandler,
                          AbstractGate::OUT_ADDRESS_CODE, gateID, PT_);
  
  return {inputs, outputs};
}

// wrapper for getInputsAndOutputs - converts string with format
// MinIn-MaxIn/MinOut-MaxOut to two pairs and calls getInputsAndOutputs() with
// pairs
std::pair<std::vector<int>, std::vector<int>> Gate_Builder::getInputsAndOutputs(
    const std::string IO_Ranges, int &inMax, int &outMax,
    std::shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID,
    std::shared_ptr<ParametersTable> PT_, const std::string featureName) {

  static const std::regex io_range(R"(^(\d+)-(\d+),(\d+)-(\d+)$)"); 
  std::smatch m;
  if (!std::regex_match(IO_Ranges, m, io_range)) {
    std::cout << "  SYNTAX ERROR in IO_range: \"" << IO_Ranges << "\" for \""
              << featureName << "\".\n  Exiting." << std::endl;
    exit(1);
  }
  return getInputsAndOutputs({std::stol(m[1].str()), std::stol(m[2].str())},
                             {std::stol(m[3].str()), std::stol(m[4].str())},
                             genomeHandler, gateID, PT_);
}

// setupGates() populates Gate::makeGate (a structure containing functions) with
// the constructors for various types of gates.
// there are 256 possible gates identified each by a pair of codons (n followed
// by 256-n)
// after initializing Gate::MakeGate, Gate::AddGate() adds values and the
// associated constructor function to Gate::MakeGate
void Gate_Builder::setupGates() {
  // the following "Codes" are identifiers for different gate types. These
  // number are used to look up constructors and also as the first 1/2 of the
  // start codeon
  // if codons are being used to generate gates.
  // more may be added, but the numbers should not change (if they do, then
  // genomes will generate different brains!)
  int ProbabilisticCode = 42; // TODO these should be enums
  int DeterministicCode = 43;
  int EpsilonCode = 44;
  int VoidCode = 45;
  int GPCode = 46;
  int TritDeterministicCode = 47;
  int NeuronCode = 48;
  int FeedbackCode = 49;
  int DecomposableCode = 50;
  int DecomposableFeedbackCode = 51;

  int bitsPerCodon = bitsPerCodonPL->get(PT);
  makeGate.resize(1 << bitsPerCodon);
  for (int i = 0; i < (1 << bitsPerCodon); i++) {
    AddGate(i, nullptr);
  }
  gateStartCodes.resize(1 << bitsPerCodon);

  if (usingProbGatePL->get(PT)) {
    inUseGateNames.insert("Probabilistic");
    int codonOne = ProbabilisticCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = probGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      std::string IO_Ranges = ProbabilisticGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_PROBABILISTIC");
      std::vector<std::vector<int>> rawTable = genomeHandler->readTable(
          {1 << addresses.first.size(), 1 << addresses.second.size()},
          {(int)pow(2, maxIn), (int)pow(2, maxOut)}, {0, 255},
          AbstractGate::DATA_CODE, gateID);
      if (genomeHandler->atEOC()) {
        std::shared_ptr<ProbabilisticGate> nullObj = nullptr;
        return nullObj;
      }
      return std::make_shared<ProbabilisticGate>(addresses, rawTable, gateID, PT_);
    });
  }
  if (usingDecoGatePL->get(PT)) {
    inUseGateNames.insert("Decomposable");
    int codonOne = DecomposableCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = decoGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      std::string IO_Ranges = DecomposableGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      bool decoUse2Level = decoUse2LevelPL->get(PT_);
      bool decoRowFirst = deco2LevelRowFirstPL->get(PT_);
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_DECOMPOSABLE");
      /// for debug
      // ostream_iterator<int> outInt(cout, ", ");
      // ostream_iterator<double> outDbl(cout, ", ");
      /// read in as many factors as there are outputs (one factor : one bit)
      /// these factors are used to expand by multiplication into the
      /// probabilities
      /// for each combination of output. 2-output gate will have 2 factors,
      /// which multiply together:
      /// (1-p)*(1-q), (1-p)*q, p*(1-q), p*q
      /// to form these 4 probabilities. By definition these 4 probabilities are
      /// now
      /// derived from independent probabilities and will not produce
      /// instantaneous causation.
      std::vector<std::vector<int>> rawTable = genomeHandler->readTable(
          {1 << addresses.first.size(), 1 << addresses.second.size()},
          {(int)pow(2, maxIn), (int)pow(2, maxOut)}, {0, 255},
          AbstractGate::DATA_CODE, gateID);
      if (decoUse2Level == false) { /// normal 1-level mode is to create
                                    /// decomposability on each row
        std::vector<std::vector<double>> factorsList(1 << addresses.first.size());
        for (std::vector<double> &factors : factorsList) {
          factors.resize(addresses.second.size());
          for (double &eachFactor : factors) {
            eachFactor = genomeHandler->readDouble(
                0, 1, AbstractGate::DATA_CODE, gateID);
          }
        }
        for (int rowi = 0; rowi < rawTable.size(); rowi++) {
          for (int outputi = 0; outputi < rawTable[rowi].size(); outputi++) {
            double p(1.0);
            /// loop through bits in each output and multiply
            /// the appropriate factors together in the right way (1-p) for
            /// bit=0 vs p for bit=1
            for (int biti = 0; biti < addresses.second.size(); biti++) {
              if ((outputi & (1 << biti)) ==
                  (1 << biti)) { /// if bit is set (there are same # bits as
                                 /// outputs)
                p *= factorsList[rowi][biti];
              } else {
                p *= (1.0 - factorsList[rowi][biti]);
              }
            }
            rawTable[rowi][outputi] = int(255 * p);
          }
        }
      } else { /// decoUse2Level true where both columns and rows are enforced
               /// decomposable
        std::vector<std::vector<double>> factorsList(addresses.first.size());
        for (std::vector<double> &factors : factorsList) {
          factors.resize(addresses.second.size());
          for (double &eachFactor : factors) {
            eachFactor = genomeHandler->readDouble(
                0, 1, AbstractGate::DATA_CODE, gateID);
          }
        }
        if (decoRowFirst) { /// expand row first
          std::vector<std::vector<double>> rowExpandedTable(
              addresses.first.size(),
              std::vector<double>(1 << addresses.second.size(), 0.0));
          /// begin expanding only the rows
          for (int rowi = 0; rowi < rowExpandedTable.size(); rowi++) {
            for (int outputi = 0; outputi < rowExpandedTable[rowi].size();
                 outputi++) {
              double p(1.0);
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for
              /// bit=0 vs p for bit=1
              for (int biti = 0; biti < addresses.second.size(); biti++) {
                if ((outputi & (1 << biti)) ==
                    (1 << biti)) { /// if bit is set (there are same # bits as
                                   /// outputs)
                  p *= factorsList[rowi][biti];
                } else {
                  p *= (1.0 - factorsList[rowi][biti]);
                }
              }
              rowExpandedTable[rowi][outputi] = p;
            }
          }
          /// now expand the columns
          for (int coli = 0; coli < rowExpandedTable[0].size(); coli++) {
            for (int outputi = 0; outputi < rawTable.size(); outputi++) {
              double p(1.0);
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for
              /// bit=0 vs p for bit=1
              for (int biti = 0; biti < addresses.first.size(); biti++) {
                if ((outputi & (1 << biti)) ==
                    (1 << biti)) { /// if bit is set (there are same # bits as
                                   /// outputs)
                  p *= rowExpandedTable[biti][coli];
                } else {
                  p *= (1.0 - rowExpandedTable[biti][coli]);
                }
              }
              rawTable[outputi][coli] = int(255 * p);
            }
          }
        } else { /// expand column first
          std::vector<std::vector<double>> colExpandedTable(
              1 << addresses.first.size(),
              std::vector<double>(addresses.first.size(), 0.0));
          // vector<vector<double>> rowExpandedTable = genomeHandler->readTable(
          // {addresses.first.size(), 1 << addresses.second.size()}, {maxIn,
          // (int)pow(2,maxOut)}, {0, 255}, AbstractGate::DATA_CODE, gateID);
          /// begin expanding only the rows
          for (int coli = 0; coli < colExpandedTable[0].size(); coli++) {
            for (int outputi = 0; outputi < colExpandedTable.size();
                 outputi++) {
              double p(1.0);
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for
              /// bit=0 vs p for bit=1
              for (int biti = 0; biti < addresses.first.size(); biti++) {
                if ((outputi & (1 << biti)) ==
                    (1 << biti)) { /// if bit is set (there are same # bits as
                                   /// outputs)
                  p *= factorsList[biti][coli];
                } else {
                  p *= (1.0 - factorsList[biti][coli]);
                }
              }
              colExpandedTable[outputi][coli] = p;
            }
          }
          /// now expand the rows
          for (int rowi = 0; rowi < colExpandedTable.size(); rowi++) {
            for (int outputi = 0; outputi < rawTable[0].size(); outputi++) {
              double p(1.0);
              /// loop through bits in each output and multiply
              /// the appropriate factors together in the right way (1-p) for
              /// bit=0 vs p for bit=1
              for (int biti = 0; biti < addresses.second.size(); biti++) {
                if ((outputi & (1 << biti)) ==
                    (1 << biti)) { /// if bit is set (there are same # bits as
                                   /// outputs)
                  p *= colExpandedTable[rowi][biti];
                } else {
                  p *= (1.0 - colExpandedTable[rowi][biti]);
                }
              }
              rawTable[rowi][outputi] = int(255 * p);
            }
          }
        }
      }
      if (genomeHandler->atEOC()) {
        std::shared_ptr<DecomposableGate> nullObj = nullptr;
        return nullObj;
      }
      return std::make_shared<DecomposableGate>(addresses, rawTable, gateID, PT_);
    });
  }
  if (usingDetGatePL->get(PT)) {
    inUseGateNames.insert("Deterministic");
    int codonOne = DeterministicCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = detGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      std::string IO_Ranges = DeterministicGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_DETERMINISTIC");
      std::vector<std::vector<int>> table = genomeHandler->readTable(
          {1 << (int)addresses.first.size(), (int)addresses.second.size()},
          {(int)pow(2, maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE,
          gateID);
      if (genomeHandler->atEOC()) {
        std::shared_ptr<DeterministicGate> nullObj = nullptr;
        return nullObj;
      }
      return std::make_shared<DeterministicGate>(addresses, table, gateID, PT_);
    });
  }
  if (usingEpsiGatePL->get(PT)) {
    inUseGateNames.insert("Epsilon");
    int codonOne = EpsilonCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = epsiGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {

      std::string IO_Ranges = EpsilonGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_EPSILON");
      std::vector<std::vector<int>> table = genomeHandler->readTable(
          {1 << (int)addresses.first.size(), (int)addresses.second.size()},
          {(int)pow(2, maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE,
          gateID);

      double epsilon = EpsilonGate::EpsilonSourcePL->get(PT_);

      if (epsilon > 1) {
        genomeHandler->advanceIndex((int)epsilon);
        epsilon =
            genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
      } else if (epsilon < 0) {
        // if we are reading from head of genome, the we are not worried about
        // EOC, so we get a new handler so as not to reset the EOC/EOG staus.
        auto cleanGenomeHandler = genomeHandler->makeCopy();
        cleanGenomeHandler->resetHandler();
        cleanGenomeHandler->advanceIndex((int)abs(epsilon));
        epsilon = cleanGenomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE,
                                                 gateID);
      }

      return std::make_shared<EpsilonGate>(addresses, table, gateID, epsilon, PT_);
    });
  }
  if (usingVoidGatePL->get(PT)) {
    inUseGateNames.insert("Void");
    int codonOne = VoidCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = voidGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {

      std::string IO_Ranges = VoidGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_VOID");
      std::vector<std::vector<int>> table = genomeHandler->readTable(
          {1 << (int)addresses.first.size(), (int)addresses.second.size()},
          {(int)pow(2, maxIn), maxOut}, {0, 1}, AbstractGate::DATA_CODE,
          gateID);

      double epsilon = VoidGate::voidGate_ProbabilityPL->get(PT_);

      if (epsilon > 1) {
        genomeHandler->advanceIndex((int)epsilon);
        epsilon =
            genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
      } else if (epsilon < 0) {
        // if we are reading from head of genome, the we are not worried about
        // EOC, so we get a new handler so as not to reset the EOC/EOG staus.
        auto cleanGenomeHandler = genomeHandler->makeCopy();
        cleanGenomeHandler->resetHandler();
        cleanGenomeHandler->advanceIndex((int)abs(epsilon));
        epsilon = cleanGenomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE,
                                                 gateID);
      }

      return std::make_shared<VoidGate>(addresses, table, gateID, epsilon, PT_);
    });
  }
  if (usingGPGatePL->get(PT)) {
    inUseGateNames.insert("GeneticPrograming");
    int codonOne = GPCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = gPGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      std::string IO_Ranges = GPGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_GENETICPROGRAMING");
      int operation =
          genomeHandler->readInt(0, 8, AbstractGate::DATA_CODE, gateID);
      std::vector<double> constValues;
      for (int i = 0; i < 4; i++) {
        double constValueMin = GPGate::constValueMinPL->get(PT_);
        double constValueMax = GPGate::constValueMaxPL->get(PT_);
        constValues.push_back(genomeHandler->readDouble(
            constValueMin, constValueMax, AbstractGate::DATA_CODE, gateID));
      }
      if (genomeHandler->atEOC()) {
        std::shared_ptr<GPGate> nullObj = nullptr;
        ;
        return nullObj;
      }
      return std::make_shared<GPGate>(addresses, operation, constValues, gateID,
                                 PT_);
    });
  }
  if (usingTritDeterministicGatePL->get(PT)) {
    inUseGateNames.insert("TritDeterministic");
    int codonOne = TritDeterministicCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = tritDeterministicGateInitialCountPL->get(PT);

    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      std::string IO_Ranges = TritDeterministicGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_TRIT");
      std::vector<std::vector<int>> table =
          genomeHandler->readTable({(int)pow(3, (int)addresses.first.size()),
                                    (int)addresses.second.size()},
                                   {(int)pow(3, maxIn), maxOut}, {-1, 1},
                                   AbstractGate::DATA_CODE, gateID);
      if (genomeHandler->atEOC()) {
        std::shared_ptr<TritDeterministicGate> nullObj = nullptr;
        ;
        return nullObj;
      }
      return std::make_shared<TritDeterministicGate>(addresses, table, gateID, PT_);
    });
  }
  if (usingNeuronGatePL->get(PT)) {
    inUseGateNames.insert("Neuron");
    int codonOne = NeuronCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = neuronGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      int defaultNumInputsMin = NeuronGate::defaultNumInputsMinPL->get(PT_);
      int defaultNumInputsMax = NeuronGate::defaultNumInputsMaxPL->get(PT_);
      int numInputs =
          genomeHandler->readInt(defaultNumInputsMin, defaultNumInputsMax,
                                 AbstractGate::IN_COUNT_CODE, gateID);
      std::vector<int> inputs;
      inputs.resize(numInputs);

      getSomeBrainAddresses(numInputs, defaultNumInputsMax, inputs,
                            genomeHandler, AbstractGate::IN_ADDRESS_CODE,
                            gateID, PT_);

      int output =
          genomeHandler->readInt(0, (1 << bitsPerBrainAddressPL->get(PT_)) - 1,
                                 AbstractGate::OUT_ADDRESS_CODE, gateID);

      int dischargeBehavior = NeuronGate::defaultDischargeBehaviorPL->get(PT_);
      if (dischargeBehavior == -1) {
        dischargeBehavior =
            genomeHandler->readInt(0, 2, AbstractGate::DATA_CODE, gateID);
      }

      double defaultThresholdMin = NeuronGate::defaultThresholdMinPL->get(PT_);
      double defaultThresholdMax = NeuronGate::defaultThresholdMaxPL->get(PT_);
      double thresholdValue =
          genomeHandler->readDouble(defaultThresholdMin, defaultThresholdMax,
                                    AbstractGate::DATA_CODE, gateID);

      bool thresholdActivates = 1;
      bool defaultAllowRepression =
          NeuronGate::defaultAllowRepressionPL->get(PT_);
      if (defaultAllowRepression == 1) {
        thresholdActivates =
            genomeHandler->readInt(0, 1, AbstractGate::DATA_CODE, gateID);
      }

      double decayRate =
          genomeHandler->readDouble(NeuronGate::defaultDecayRateMinPL->get(PT_),
                                    NeuronGate::defaultDecayRateMaxPL->get(PT_),
                                    AbstractGate::DATA_CODE, gateID);
      double deliveryCharge = genomeHandler->readDouble(
          NeuronGate::defaultDeliveryChargeMinPL->get(PT_),
          NeuronGate::defaultDeliveryChargeMaxPL->get(PT_),
          AbstractGate::DATA_CODE, gateID);
      double deliveryError = NeuronGate::defaultDeliveryErrorPL->get(PT_);

      int ThresholdFromNode = -1;
      int DeliveryChargeFromNode = -1;
      bool defaultThresholdFromNode =
          NeuronGate::defaultThresholdFromNodePL->get(PT_);
      if (defaultThresholdFromNode) {
        ThresholdFromNode = genomeHandler->readInt(
            0, (1 << bitsPerBrainAddressPL->get(PT_)) - 1,
            AbstractGate::IN_ADDRESS_CODE, gateID);
      }
      bool defaultDeliveryChargeFromNode =
          NeuronGate::defaultDeliveryChargeFromNodePL->get(PT_);
      if (defaultDeliveryChargeFromNode) {
        DeliveryChargeFromNode = genomeHandler->readInt(
            0, (1 << bitsPerBrainAddressPL->get(PT_)) - 1,
            AbstractGate::IN_ADDRESS_CODE, gateID);
      }
      if (genomeHandler->atEOC()) {
        std::shared_ptr<NeuronGate> nullObj = nullptr;
        ;
        return nullObj;
      }
      return std::make_shared<NeuronGate>(
          inputs, output, dischargeBehavior, thresholdValue, thresholdActivates,
          decayRate, deliveryCharge, deliveryError, ThresholdFromNode,
          DeliveryChargeFromNode, gateID, PT_);
    });
  }
  if (usingFeedbackGatePL->get(PT)) {
    inUseGateNames.insert("Feedback");
    int codonOne = FeedbackCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] = feedbackGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      unsigned int posFBNode, negFBNode;
      unsigned char nrPos, nrNeg;
      std::vector<double> posLevelOfFB, negLevelOfFB;
      std::string IO_Ranges = FeedbackGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_FEEDBACK");
      std::vector<std::vector<int>> rawTable = genomeHandler->readTable(
          {1 << addresses.first.size(), 1 << addresses.second.size()},
          {(int)pow(2, maxIn), (int)pow(2, maxOut)}, {0, 255},
          AbstractGate::DATA_CODE, gateID);
      posFBNode = genomeHandler->readInt(
          0, 255); // we will scale to, say, 256 and floor later.
      negFBNode = genomeHandler->readInt(0, 255);
      nrPos = genomeHandler->readInt(0, 3);
      nrNeg = genomeHandler->readInt(0, 3);
      posLevelOfFB.resize(nrPos);
      negLevelOfFB.resize(nrNeg);
      for (int i = 0; i < nrPos; i++)
        posLevelOfFB[i] = genomeHandler->readDouble(0, 256);
      for (int i = 0; i < nrNeg; i++)
        negLevelOfFB[i] = genomeHandler->readDouble(0, 256);
      if (genomeHandler->atEOC()) {
        std::shared_ptr<FeedbackGate> nullObj = nullptr;
        return nullObj;
      }

      return std::make_shared<FeedbackGate>(addresses, rawTable, posFBNode,
                                       negFBNode, nrPos, nrNeg, posLevelOfFB,
                                       negLevelOfFB, gateID, PT_);
      // std::pair<vector<int>,vector<int>> thepair =
      // std::make_pair(std::vector<int>(),std::vector<int>());
      // std::vector<std::vector<int>> dvec;
      // unsigned int uint = 0;
      // unsigned char uchar = '\0';
      // std::vector<double> vdouble;
      // return
      // make_shared<FeedbackGate>(thepair,dvec,uint,uint,uchar,uchar,vdouble,vdouble,gateID,
      // PT_);
    });
  }
  if (usingDecomposableFeedbackGatePL->get(PT)) {
    inUseGateNames.insert("DecomposableFeedback");
    int codonOne = DecomposableFeedbackCode;
    inUseGateTypes.insert(codonOne);
    {
      gateStartCodes[codonOne].push_back(codonOne);
      gateStartCodes[codonOne].push_back(((1 << bitsPerCodon) - 1) - codonOne);
    }
    intialGateCounts[codonOne] =
        decomposableFeedbackGateInitialCountPL->get(PT);
    AddGate(codonOne, [](std::shared_ptr<AbstractGenome::Handler> genomeHandler,
                         int gateID, std::shared_ptr<ParametersTable> PT_) {
      unsigned int posFBNode, negFBNode;
      unsigned char nrPos, nrNeg;
      std::vector<double> posLevelOfFB, negLevelOfFB;
      std::string IO_Ranges = DecomposableFeedbackGate::IO_RangesPL->get(PT_);
      int maxIn, maxOut;
      std::pair<std::vector<int>, std::vector<int>> addresses =
          getInputsAndOutputs(IO_Ranges, maxIn, maxOut, genomeHandler, gateID,
                              PT_, "BRAIN_MARKOV_GATES_FEEDBACK");
      // vector<vector<int>> rawTable = genomeHandler->readTable( {1 <<
      // addresses.first.size(), 1 << addresses.second.size()},
      // {(int)pow(2,maxIn), (int)pow(2,maxOut)}, {0, 255},
      // AbstractGate::DATA_CODE, gateID);
      std::vector<std::vector<int>> rawTable(
          1 << addresses.first.size(),
          std::vector<int>(1 << addresses.second.size(),
                      0)); /// correct size, and init to 0
      /// read factors and generate table data from factors
      std::vector<std::vector<double>> factorsList(1 << addresses.first.size());
      for (std::vector<double> &factors : factorsList) {
        factors.resize(addresses.second.size());
        for (double &eachFactor : factors) {
          eachFactor =
              genomeHandler->readDouble(0, 1, AbstractGate::DATA_CODE, gateID);
        }
      }
      std::bitset<32> bs(0); /// bitset
      for (int rowi = 0; rowi < rawTable.size(); rowi++) {
        for (int outputi = 0; outputi < rawTable[rowi].size(); outputi++) {
          double p(1.0);
          bs = outputi;
          /// loop through bits in each output and multiply
          /// the appropriate factors together in the right way (1-p) for bit=0
          /// vs p for bit=1
          for (int biti = 0; biti < addresses.second.size(); biti++) {
            p *= (bs[biti] ? factorsList[rowi][biti]
                           : 1 - factorsList[rowi][biti]);
          }
          rawTable[rowi][outputi] = int(255 * p);
        }
      }
      posFBNode = genomeHandler->readInt(
          0, 255); // we will scale to, say, 256 and floor later.
      negFBNode = genomeHandler->readInt(0, 255);
      nrPos = genomeHandler->readInt(0, 3);
      nrNeg = genomeHandler->readInt(0, 3);
      posLevelOfFB.resize(nrPos);
      negLevelOfFB.resize(nrNeg);
      for (int i = 0; i < nrPos; i++)
        posLevelOfFB[i] = genomeHandler->readDouble(0, 256);
      for (int i = 0; i < nrNeg; i++)
        negLevelOfFB[i] = genomeHandler->readDouble(0, 256);
      if (genomeHandler->atEOC()) {
        std::shared_ptr<DecomposableFeedbackGate> nullObj = nullptr;
        return nullObj;
      }
      return std::make_shared<DecomposableFeedbackGate>(
          addresses, rawTable, factorsList, posFBNode, negFBNode, nrPos, nrNeg,
          posLevelOfFB, negLevelOfFB, gateID, PT_);
    });
  }
}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
// function<std::shared_ptr<Gate>(std::shared_ptr<AbstractGenome::Handler> genomeHandler,
// int gateID)> Gate_Builder::makeGate[256];
// vector<function<std::shared_ptr<AbstractGate>(std::shared_ptr<AbstractGenome::Handler>,
// int gateID)>> Gate_Builder::makeGate;
void Gate_Builder::AddGate(
    int gateType,
    std::function<std::shared_ptr<AbstractGate>(
        std::shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID,
        std::shared_ptr<ParametersTable> gatePT)> theFunction) {
  makeGate[gateType] = theFunction;
}
/* *** end - some c++ 11 magic to speed up translation from genome to gates ***
 */
