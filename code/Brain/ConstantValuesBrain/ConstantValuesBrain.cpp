//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "ConstantValuesBrain.h"

std::shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMinPL =
    Parameters::register_parameter("BRAIN_CONSTANT-valueMin", 0.0,
                                   "Minmum value that brain will deliver");
std::shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMaxPL =
    Parameters::register_parameter("BRAIN_CONSTANT-valueMax", 100.0,
                                   "Maximum value that brain will deliver");
std::shared_ptr<ParameterLink<int>> ConstantValuesBrain::valueTypePL =
    Parameters::register_parameter("BRAIN_CONSTANT-valueType", 0,
                                   "0 = int, 1 = double");
std::shared_ptr<ParameterLink<int>> ConstantValuesBrain::samplesPerValuePL =
    Parameters::register_parameter("BRAIN_CONSTANT-samplesPerValue", 1,
                                   "for each brain value, this many samples "
                                   "will be taken from genome and averaged");

std::shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeUniformPL =
    Parameters::register_parameter(
        "BRAIN_CONSTANT-initializeUniform", false,
        "Initialize genome randomly, with all samples having same value");
std::shared_ptr<ParameterLink<bool>> ConstantValuesBrain::initializeConstantPL =
    Parameters::register_parameter("BRAIN_CONSTANT-initializeConstant", false,
                                   "If true, all values in genome will be "
                                   "initialized to initial constant value.");
std::shared_ptr<ParameterLink<double>>
    ConstantValuesBrain::initializeConstantValuePL =
        Parameters::register_parameter("BRAIN_CONSTANT-initializeConstantValue",
                                       0.0, "If initialized constant, this "
                                            "value is used to initialize "
                                            "entire genome.");

std::shared_ptr<ParameterLink<std::string>> ConstantValuesBrain::genomeNamePL =
    Parameters::register_parameter("BRAIN_CONSTANT_NAMES-genomeNameSpace",
                                   (std::string) "root::",
                                   "namespace used to set parameters for "
                                   "genome used to encode this brain");

ConstantValuesBrain::ConstantValuesBrain(int _nrInNodes, int _nrOutNodes,
                                         std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {

  // columns to be added to ave file
  popFileColumns.clear();
  for (int i = 0; i < nrOutputValues; i++) {
    popFileColumns.push_back("brainValue" + std::to_string(i));
  }
}

std::shared_ptr<AbstractBrain> ConstantValuesBrain::makeBrain(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  std::shared_ptr<ConstantValuesBrain> newBrain =
      std::make_shared<ConstantValuesBrain>(nrInputValues, nrOutputValues, PT);
  auto genomeHandler = _genomes[genomeNamePL->get(PT)]->newHandler(
      _genomes[genomeNamePL->get(PT)], true);
  auto samplesPerValue = samplesPerValuePL->get(PT);
  auto valueType = valueTypePL->get(PT);
  auto valueMin = valueMinPL->get(PT);
  auto valueMax = valueMaxPL->get(PT);

  if (valueType != 0 && valueType!= 1) {
        std::cout
            << "  ERROR! BRAIN_CONSTANT-valueType is invalid. current value: "
            << valueType << std::endl;
        exit(1);
  }

  for (int i = 0; i < nrOutputValues; i++) {
    auto tempValue = 0.;
    for (int j = 0; j < samplesPerValue; j++) 
      tempValue += !valueType ? genomeHandler->readInt(valueMin, valueMax)
                              : genomeHandler->readDouble(valueMin, valueMax);

    newBrain->outputValues[i] = !valueType ? int(tempValue / samplesPerValue)
                                           : tempValue / samplesPerValue;
  }

  return newBrain;
}

void ConstantValuesBrain::resetBrain() {
  // do nothing! values never change!
}

void ConstantValuesBrain::update() {
  // do nothing! output is already set!
}

void inline ConstantValuesBrain::resetOutputs() {
  // do nothing! output is already set!
}

std::string ConstantValuesBrain::description() {
  return  "Constant Values Brain\n";
}

DataMap ConstantValuesBrain::getStats(std::string &prefix) {
  DataMap dataMap;
  for (int i = 0; i < nrOutputValues; i++) {
    dataMap.set(prefix + "brainValue" + std::to_string(i), outputValues[i]);
  }
  return dataMap;
}

void ConstantValuesBrain::initializeGenomes(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {

  auto samplesPerValue = samplesPerValuePL->get(PT);
  auto valueType = valueTypePL->get(PT);
  auto valueMin = valueMinPL->get(PT);
  auto valueMax = valueMaxPL->get(PT);
  auto initializeConstantValue = initializeConstantValuePL->get(PT);

  auto genomeName = genomeNamePL->get(PT);

  if (initializeConstantPL->get(PT)) {
    if (initializeConstantValue < valueMin) {
      std::cout << "ERROR: initializeConstantValue must be greater then or "
                   "equal to valueMin"
                << std::endl;
      exit(1);
    }
    if (initializeConstantValue > valueMax) {
      std::cout << "ERROR: initializeConstantValue must be less then or equal "
                   "to valueMax"
                << std::endl;
      exit(1);
    }
    auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
    while (!handler->atEOG()) {
      if (valueType == 1) {
        handler->writeDouble(initializeConstantValue, valueMin, valueMax);
      } else if (valueType == 0) {
        handler->writeInt((int)initializeConstantValue, (int)valueMin,
                          (int)valueMax);
      }
    }
    // handler->resetHandler();
    // handler->writeInt(initializeConstantValue, (int)valueMin, (int)valueMax);
  } else if (initializeUniformPL->get(PT)) {
    auto handler = _genomes[genomeName]->newHandler(_genomes[genomeName]);
    int count = 0;
    double randomValue = 0;
    if (valueType == 1) {
      randomValue = Random::getDouble(valueMin, valueMax);
    } else if (valueType == 0) {
      randomValue = Random::getInt((int)valueMin, (int)valueMax);
    }
    while (!handler->atEOG()) {
      if (count == samplesPerValue) {
        count = 0;
        if (valueType == 1) {
          randomValue = Random::getDouble(valueMin, valueMax);
        } else if (valueType == 0) {
          randomValue = Random::getInt((int)valueMin, (int)valueMax);
        }
      }
      if (valueType == 1) {
        handler->writeDouble(randomValue, valueMin, valueMax);
      } else if (valueType == 0) {
        handler->writeInt((int)randomValue, (int)valueMin, (int)valueMax);
      }
      count++;
    }
  } else {
    _genomes[genomeName]->fillRandom();
  }
}

std::shared_ptr<AbstractBrain>
ConstantValuesBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  auto newBrain =
      std::make_shared<ConstantValuesBrain>(nrInputValues, nrOutputValues, PT_);

  for (int i = 0; i < nrOutputValues; i++) {
    newBrain->outputValues[i] = outputValues[i];
  }

  return newBrain;
}
