//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#define CGPBRAIN_DEBUG 0

#include "CGPBrain.h"

std::shared_ptr<ParameterLink<int>> CGPBrain::hiddenNodesPL =
    Parameters::register_parameter("BRAIN_CGP-hiddenNodes", 3,
                                   "number of hidden nodes");

std::shared_ptr<ParameterLink<std::string>> CGPBrain::availableOperatorsPL =
    Parameters::register_parameter(
        "BRAIN_CGP-availableOperators", (std::string) "all",
        "which opperators are allowed? all indicates, allow all opperators or, "
        "choose from: SUM,MULT,SUBTRACT,DIVIDE,SIN,COS,THRESH,RAND,IF,INV");
std::shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMaxPL =
    Parameters::register_parameter(
        "BRAIN_CGP-magnitudeMax", 1000000000.0,
        "values generated which are larger then this will by clipped");
std::shared_ptr<ParameterLink<double>> CGPBrain::magnitudeMinPL =
    Parameters::register_parameter(
        "BRAIN_CGP-magnitudeMin", -1000000000.0,
        "values generated which are smaller then this will by clipped");
std::shared_ptr<ParameterLink<int>> CGPBrain::numOpsPreVectorPL =
    Parameters::register_parameter("BRAIN_CGP-operatorsPreFormula", 8,
                                   "number of instructions per formula. "
                                   "Ignored if buildMode is \"genes\"");

std::shared_ptr<ParameterLink<std::string>> CGPBrain::genomeNamePL =
    Parameters::register_parameter("BRAIN_CGP_NAMES-genomeNameSpace",
                                   (std::string) "root::",
                                   "namespace used to set parameters for "
                                   "genome used to encode this brain");

std::shared_ptr<ParameterLink<std::string>> CGPBrain::buildModePL =
    Parameters::register_parameter(
        "BRAIN_CGP-buildMode", (std::string) "linear",
        "How is the genome converted, \"linear\" : linear conversion starting "
        "at begining of genome, \"codon\" : start codons locate "
        "operator+in1+in2 along with the formula/output index and a location "
        "in formula");
std::shared_ptr<ParameterLink<int>> CGPBrain::codonMaxPL =
    Parameters::register_parameter("BRAIN_CGP-codonMax", 100,
                                   "if using \"genes\" buildMode, values will "
                                   "be extracted from genome as integers "
                                   "[0..codonMax] and two sites that add to "
                                   "codonMax defines a start codon");

std::shared_ptr<ParameterLink<bool>> CGPBrain::readFromOutputsPL =
    Parameters::register_parameter(
        "BRAIN_CGP-readFromOutputs", true,
        "if true, previous updates outputs will be available as inputs.");

CGPBrain::CGPBrain(int _nrInNodes, int _nrOutNodes,
                   std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {

  convertCSVListToVector(availableOperatorsPL->get(PT), availableOperators);
  // nrHiddenValues = (PT == nullptr) ? hiddenNodesPL->lookup() :
  // PT->lookupInt("BRAIN_CGP-hiddenNodes");
  // magnitudeMax = (PT == nullptr) ? magnitudeMaxPL->lookup() :
  // PT->lookupDouble("BRAIN_CGP-magnitudeMax");
  // magnitudeMin = (PT == nullptr) ? magnitudeMinPL->lookup() :
  // PT->lookupDouble("BRAIN_CGP-magnitudeMin");
  // numOpsPreVector = (PT == nullptr) ? numOpsPreVectorPL->lookup() :
  // PT->lookupInt("BRAIN_CGP-operatorsPreFormula");

  // buildMode = (PT == nullptr) ? buildModePL->lookup() :
  // PT->lookupString("BRAIN_CGP-buildMode");
  // codonMax = (PT == nullptr) ? codonMaxPL->lookup() :
  // PT->lookupInt("BRAIN_CGP-codonMax");

  // readFromOutputs = (PT == nullptr) ? readFromOutputsPL->lookup() :
  // PT->lookupBool("BRAIN_CGP-readFromOutputs");

  allOps = {{"SUM", 0}, {"MULT", 1}, {"SUBTRACT", 2}, {"DIVIDE", 3},
            {"SIN", 4}, {"COS", 5},  {"THRESH", 6},   {"RAND", 7},
            {"IF", 8},  {"INV", 9}};

  if (availableOperators.size() == 0 || availableOperators[0] == "all") {
    availableOps = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  } else {
    for (auto o : availableOperators) {
      if (allOps.count(o)) {
        availableOps.push_back(allOps[o]);
      } else {
        std::cout << "In CGP brain inititialization, found unknown operator \""
                  << o << "\" in list of allowed operators.\n  exiting."
                  << std::endl;
        exit(1);
      }
    }
  }

  availableOpsCount = availableOps.size();

  nrInputTotal = nrInputValues +
                 ((readFromOutputsPL->get(PT)) ? nrOutputValues : 0) +
                 hiddenNodesPL->get(PT);
  nrOutputTotal = nrOutputValues + hiddenNodesPL->get(PT);

  readFromValues.resize(nrInputTotal, 0);
  writeToValues.resize(nrOutputTotal, 0);

  brainVectors.clear();

  // columns to be added to ave file
  popFileColumns.clear();
  if (buildModePL->get(PT) == "codon") {
    popFileColumns.push_back("cgpBrainAveFormulaLength");
  }
}

CGPBrain::CGPBrain(
    int _nrInNodes, int _nrOutNodes,
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes,
    std::shared_ptr<ParametersTable> PT_)
    : CGPBrain(_nrInNodes, _nrOutNodes, PT_) {

  brainVectors.resize(nrOutputTotal);
  auto handler = _genomes[genomeNamePL->get(PT)]->newHandler(
      _genomes[genomeNamePL->get(PT)]);

  if (buildModePL->get(PT) == "linear") {
    for (int f = 0; f < (nrOutputTotal); f++) {
      for (int i = 0; i < numOpsPreVectorPL->get(PT); i++) {
        brainVectors[f].push_back(
            availableOps[handler->readInt(0, availableOpsCount - 1)]);
        brainVectors[f].push_back(handler->readInt(
            0, nrInputTotal + (i)-1)); // num inputs + num new values - 1 since
                                       // first is 0 -1 more to correct for 0
                                       // indexing
        brainVectors[f].push_back(handler->readInt(0, nrInputTotal + (i)-1));
      }
    }
  } else if (buildModePL->get(PT) == "codon") {
#if CGPBRAIN_DEBUG == 1
    std::cout << "building with genes\n";
#endif
    // maps locations of ops in that formulas (index1 is formula, index2 is op,
    // double is location
    std::vector<std::vector<double>> locations;
    // size locations to the number of formulas needed
    locations.resize(nrOutputTotal);

    bool translation_Complete = false;

    if (_genomes[genomeNamePL->get(PT)]->isEmpty()) {
      translation_Complete = true;
    } else {

      bool readForward = true;
      auto genomeHandler = _genomes[genomeNamePL->get(PT)]->newHandler(
          _genomes[genomeNamePL->get(PT)], readForward);
      auto placeHolderGenomeHandler =
          _genomes[genomeNamePL->get(PT)]->newHandler(
              _genomes[genomeNamePL->get(PT)], readForward);
      auto gateGenomeHandler = _genomes[genomeNamePL->get(PT)]->newHandler(
          _genomes[genomeNamePL->get(PT)], readForward);

      // save start of genome info
      int i = 0;

      int testSite1Value, testSite2Value;
      testSite1Value = genomeHandler->readInt(0, codonMaxPL->get(PT));
      testSite2Value = genomeHandler->readInt(0, codonMaxPL->get(PT));
      while (!translation_Complete) {
        if (genomeHandler->atEOC()) { // if genomeIndex > testIndex, testIndex
                                      // has wrapped and we are done translating
          if (genomeHandler->atEOG()) {
            translation_Complete = true;
          }
          genomeHandler
              ->resetHandlerOnChromosome(); // reset to start of this chromosome
          genomeHandler->copyTo(placeHolderGenomeHandler); // move placeholder
                                                           // to the next
                                                           // chromosome aswell
                                                           // so mustReadAll
                                                           // method works
          testSite2Value = genomeHandler->readInt(
              0, codonMaxPL->get(PT)); // place first value in new chromosome in
                                       // testSite2 so !mustReadAll method works
        } else if (testSite1Value + testSite2Value ==
                   codonMaxPL->get(PT)) { // if we found a start codon
#if CGPBRAIN_DEBUG == 1
          std::cout << "\nstart codon found: " << testSite1Value << "   "
                    << testSite2Value << "\n";
#endif
          genomeHandler->copyTo(gateGenomeHandler);
          gateGenomeHandler->toggleReadDirection();
          gateGenomeHandler->readInt(
              0, codonMaxPL->get(PT)); // move back 2 start codon values
          gateGenomeHandler->readInt(0, codonMaxPL->get(PT));
          gateGenomeHandler
              ->toggleReadDirection(); // reverse the read direction again
          gateGenomeHandler->readInt(
              0, codonMaxPL->get(PT), 1,
              0); // mark start codon in genomes coding region
          gateGenomeHandler->readInt(0, codonMaxPL->get(PT), 1, 0);

          // Which formula is this op in?

          int f = gateGenomeHandler->readInt(0, nrOutputTotal - 1);
          // Now read the opperator and inputs
          int op = availableOps[gateGenomeHandler->readInt(
              0, availableOpsCount - 1, 2)];
          int in1 = gateGenomeHandler->readInt(
              0, (int)(_genomes[genomeNamePL->get(PT)]->getAlphabetSize() - 1),
              2);
          int in2 = gateGenomeHandler->readInt(
              0, (int)(_genomes[genomeNamePL->get(PT)]->getAlphabetSize() - 1),
              2);
#if CGPBRAIN_DEBUG == 1
          std::cout << "got values:   f: " << f << "  op : " << op
                    << "  in1: " << in1 << "  in2: " << in2 << "\n";
#endif
          // Now find the location in the formula
          double location = gateGenomeHandler->readDouble(0.0, 1.0);
#if CGPBRAIN_DEBUG == 1
          std::cout << "   @ location: " << location << "\n";
          std::cout << "   formula " << f << "  size = " << locations[f].size()
                    << "\n";
#endif
          if (locations[f].size() == 0 ||
              locations[f][locations[f].size() - 1] <= location) {
#if CGPBRAIN_DEBUG == 1
            if (locations[f].size() == 0) {
              std::cout << "      first"
                        << "\n";
            } else {
              std::cout << "      Last in formula... last location: "
                        << locations[f][locations[f].size() - 1] << "\n";
            }
#endif
            // if this formula is empty, or the new op is last push back new op
            brainVectors[f].push_back(op);
            brainVectors[f].push_back(in1);
            brainVectors[f].push_back(in2);
            locations[f].push_back(location);
          } else {
#if CGPBRAIN_DEBUG == 1
            std::cout << "      Not Last in formula... last location: "
                      << locations[f][locations[f].size() - 1] << "\n";
#endif
            // new op is not last
            int index = locations[f].size() - 1;
#if CGPBRAIN_DEBUG == 1
            std::cout << "index: " << index << "\n";
#endif
            // copy current last op to back of formula
            brainVectors[f].push_back(
                brainVectors[f][brainVectors[f].size() - 3]);
            brainVectors[f].push_back(
                brainVectors[f][brainVectors[f].size() - 2]);
            brainVectors[f].push_back(
                brainVectors[f][brainVectors[f].size() - 1]);
            locations[f].push_back(locations[f][locations[f].size() - 1]);

            // starting at the back, find the location, push each op forward
            // until we find it.
            while (index > 0 && locations[f][index] > location) {
              brainVectors[f][((index + 1) * 3) + 2] =
                  brainVectors[f][(index * 3) + 2];
              brainVectors[f][((index + 1) * 3) + 1] =
                  brainVectors[f][(index * 3) + 1];
              brainVectors[f][((index + 1) * 3) + 0] =
                  brainVectors[f][(index * 3) + 0];
              locations[f][index + 1] = locations[f][index];
#if CGPBRAIN_DEBUG == 1
              std::cout << index << " <<>> ";
#endif
              index--;
#if CGPBRAIN_DEBUG == 1
              std::cout << index << "\n";
#endif
            }

            // now index is set to where we need to insert the new instruction
            brainVectors[f][(index * 3)] = op;
            brainVectors[f][(index * 3) + 1] = in1;
            brainVectors[f][(index * 3) + 2] = in2;
            locations[f][index] = location;
          }
        }
        if (0) { // mustReadAll) {  // if start codon values are bigger then the
                 // alphabetSize of the genome, we must step forward one genome
                 // site at a time (slow)
          placeHolderGenomeHandler->advanceIndex();
          placeHolderGenomeHandler->copyTo(genomeHandler);
          testSite1Value = genomeHandler->readInt(0, codonMaxPL->get(PT));
          testSite2Value = genomeHandler->readInt(0, codonMaxPL->get(PT));
        } else { // we know that start codon values fit in a single site, so we
                 // can be clever
          testSite1Value = testSite2Value;
          testSite2Value = genomeHandler->readInt(0, codonMaxPL->get(PT));
        }
        // cout << testSite1Value << " + " << testSite2Value << " = " <<
        // testSite1Value + testSite2Value << endl;
      }
      // translation compete - now fix the input values
      for (int f = 0; f < (nrOutputTotal); f++) {
#if CGPBRAIN_DEBUG == 1
        std::cout << "f: " << f << "  :: ";
#endif
        for (int i = 0; i < (int)brainVectors[f].size(); i += 3) {
#if CGPBRAIN_DEBUG == 1
          std::cout << brainVectors[f][i] << "  ";
          std::cout << " (" << brainVectors[f][i + 1] << "%"
                    << nrInputTotal + (i / 3) << ") ";
#endif
          brainVectors[f][i + 1] =
              brainVectors[f][i + 1] %
              (nrInputTotal + (i / 3)); // num inputs + num new values - 1 since
                                        // first is 0 -1 more to correct for 0
                                        // indexing
#if CGPBRAIN_DEBUG == 1
          std::cout << brainVectors[f][i + 1] << "  ";
          std::cout << " (" << brainVectors[f][i + 2] << "%"
                    << nrInputTotal + (i / 3) << ") ";
#endif
          brainVectors[f][i + 2] =
              brainVectors[f][i + 2] %
              (nrInputTotal + (i / 3)); // num inputs + num new values - 1 since
                                        // first is 0 -1 more to correct for 0
                                        // indexing
#if CGPBRAIN_DEBUG == 1
          std::cout << brainVectors[f][i + 2] << " , ";
#endif
        }
#if CGPBRAIN_DEBUG == 1
        std::cout << "\n";
#endif
      }
      // input values are now correct
    }
  } else {
    std::cout << "\n\nIn CGP constructor, found unknown buildMode \""
              << buildModePL->get(PT) << "\".\n exiting." << std::endl;
    exit(1);
  }
}

void CGPBrain::resetBrain() {
  // std::cout << "in reset Brain" << endl;
  fill(inputValues.begin(), inputValues.end(), 0);
  fill(outputValues.begin(), outputValues.end(), 0);
  fill(readFromValues.begin(), readFromValues.end(), 0);
  fill(writeToValues.begin(), writeToValues.end(), 0);
}

void CGPBrain::update() {
  for (int index = 0; index < nrInputValues;
       index++) { // copy input values into readFromValues
    readFromValues[index] = inputValues[index];
  }
  if (readFromOutputsPL->get(PT)) { // if readFromOutputs, then add last outputs
    for (int index = 0; index < nrOutputValues; index++) {
      readFromValues[index + nrInputValues] = writeToValues[index];
    }
  }
  for (int index = nrOutputValues;
       index < nrOutputValues + hiddenNodesPL->get(PT);
       index++) { // add hidden values from writeToValues
    readFromValues[index + nrInputValues -
                   ((!readFromOutputsPL->get(PT)) ? nrOutputValues : 0)] =
        writeToValues[(index)];
  }

  DataMap dataMap;
  std::vector<double> values;

#if CGPBRAIN_DEBUG == 1
  std::cout << "***********************************\nSTART"
            << "\n";
#endif
  auto magnitudeMax = magnitudeMaxPL->get(PT);
  auto magnitudeMin = magnitudeMinPL->get(PT);
  for (int vec = 0; vec < (int)brainVectors.size(); vec++) {
#if CGPBRAIN_DEBUG == 1
    std::cout << "vec: " << vec << "\n";
#endif
    values.clear();
    values = readFromValues;
    for (int site = 0; site < (int)brainVectors[vec].size(); site += 3) {
      double op1 = values[brainVectors[vec][site + 1]];
      double op2 = values[brainVectors[vec][site + 2]];
      switch (brainVectors[vec][site]) {

      case 0: // SUM
#if CGPBRAIN_DEBUG == 1
        std::cout << "SUM(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, op1 + op2)));
        break;
      case 1: // MULT
#if CGPBRAIN_DEBUG == 1
        std::cout << "MULT(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, op1 * op2)));
        break;
      case 2: // SUBTRACT
#if CGPBRAIN_DEBUG == 1
        std::cout << "SUBTRACT(" << brainVectors[vec][site + 1] << "=" << op1
                  << "," << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, op1 - op2)));
        break;
      case 3: // DIVIDE
        if (op2 == 0) {
#if CGPBRAIN_DEBUG == 1
          std::cout << "DIVIDE**0**(" << brainVectors[vec][site + 1] << "="
                    << op1 << "," << brainVectors[vec][site + 2] << "=" << op2
                    << ")"
                    << "\n";
#endif
          values.push_back(0);
        } else {
#if CGPBRAIN_DEBUG == 1
          std::cout << "DIVIDE(" << brainVectors[vec][site + 1] << "=" << op1
                    << "," << brainVectors[vec][site + 2] << "=" << op2 << ")"
                    << "\n";
#endif
          values.push_back(
              std::min(magnitudeMax, std::max(magnitudeMin, op1 / op2)));
        }
        break;
      case 4: // SIN
#if CGPBRAIN_DEBUG == 1
        std::cout << "SIN(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, sin(op1))));
        break;
      case 5: // COS
#if CGPBRAIN_DEBUG == 1
        std::cout << "COS(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, cos(op1))));
        break;

      case 6: // THRESH
#if CGPBRAIN_DEBUG == 1
        std::cout << "THRESH(" << brainVectors[vec][site + 1] << "=" << op1
                  << "," << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(std::min(
            magnitudeMax, std::max(magnitudeMin, (op1 > op2) ? op2 : op1)));
        break;
      case 7: // RAND
#if CGPBRAIN_DEBUG == 1
        std::cout << "RAND(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(std::min(
            magnitudeMax, std::max(magnitudeMin, Random::getDouble(op1, op2))));
        break;
      case 8: // IF
#if CGPBRAIN_DEBUG == 1
        std::cout << "IF(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(std::min(magnitudeMax,
                                  std::max(magnitudeMin, (op1 > 0) ? op2 : 0)));
        break;
      case 9: // INV
#if CGPBRAIN_DEBUG == 1
        std::cout << "INV(" << brainVectors[vec][site + 1] << "=" << op1 << ","
                  << brainVectors[vec][site + 2] << "=" << op2 << ")"
                  << "\n";
#endif
        values.push_back(
            std::min(magnitudeMax, std::max(magnitudeMin, -1.0 * op1)));
        break;
      }
#if CGPBRAIN_DEBUG == 1
      std::cout << "values: ";
      for (auto v : values) {
        std::cout << v << " ";
      }
      std::cout << "\n";
      std::cout << "outputValues: ";
      for (auto v : outputValues) {
        std::cout << v << " ";
      }
      std::cout << "\n";
#endif
    }
    writeToValues[vec] = values.back();
    if (vec < nrOutputValues) {
      outputValues[vec] = values.back();
    }
  }
}

std::string CGPBrain::description() {
  std::string S = "CGPBrain\n";
  return S;
}

DataMap CGPBrain::getStats(std::string &prefix) {
  DataMap dataMap;

  double aveFormulaLength = 0.0;
  if (buildModePL->get(PT) == "codon") {
    for (auto vec : brainVectors) {
      aveFormulaLength += (double)(vec.size());
    }
    aveFormulaLength /= double(brainVectors.size());
    dataMap.set(prefix + "cgpBrainAveFormulaLength", aveFormulaLength);
  }
  // get stats
  // cout << "warning:: getStats for CGPBrain needs to be written." << endl;
  return (dataMap);
}

void CGPBrain::initializeGenomes(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  _genomes[genomeNamePL->get(PT)]->fillRandom();
}

std::shared_ptr<AbstractBrain>
CGPBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  auto newBrain =
      std::make_shared<CGPBrain>(nrInputValues, nrOutputValues, PT_);
  newBrain->brainVectors = brainVectors;
  return newBrain;
}
