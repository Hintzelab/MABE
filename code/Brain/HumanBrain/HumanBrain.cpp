//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "HumanBrain.h"

std::shared_ptr<ParameterLink<bool>> HumanBrain::useActionMapPL =
    Parameters::register_parameter(
        "BRAIN_HUMAN-useActionMap", false,
        "if true, an action map will be used to translate user input");
std::shared_ptr<ParameterLink<std::string>> HumanBrain::actionMapFileNamePL =
    Parameters::register_parameter("BRAIN_HUMAN-actionMapFileName",
                                   (std::string) "actionMap.txt",
                                   "if useActionMap = true, use this file");

HumanBrain::HumanBrain(int _nrInNodes, int _nrOutNodes,
                       std::shared_ptr<ParametersTable> PT_)
    : AbstractBrain(_nrInNodes, _nrOutNodes, PT_) {
  useActionMap = useActionMapPL->get(PT);
  actionMapFileName = actionMapFileNamePL->get(PT);

  if (useActionMap) { // if using an action map, load map with lines of format
                      // char output1 output2 output3... file must match brain #
                      // of outputs
    std::string fileName = actionMapFileName;
    std::ifstream FILE(fileName);
    std::string rawLine;
    double readDouble;
    char readChar;
    std::vector<double> action;
    if (FILE.is_open()) { // if the file named by actionMapFileName can be
                          // opened
      while (getline(FILE, rawLine)) { // keep loading one line from the file at
                                       // a time into "line" until we get to the
                                       // end of the file
        std::stringstream ss(rawLine);
        ss >> readChar; // pull one char, this will be the key to the map
        action.clear();
        for (int i = 0; i < nrOutputValues;
             i++) {         // pull one double for each output
          ss >> readDouble; // read one double
          action.push_back(readDouble);
        }
        ss >> actionNames[readChar];
        actionMap[readChar] = action;
      }
    } else {
      std::cout << "\n\nERROR:HumanBrain constructor, unable to open file \""
                << fileName << "\"\n\nExiting\n" << std::endl;
      exit(1);
    }
  }

  // columns to be added to ave file
  popFileColumns.clear();
}

std::shared_ptr<AbstractBrain> HumanBrain::makeBrain(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  std::shared_ptr<HumanBrain> newBrain =
      std::make_shared<HumanBrain>(nrInputValues, nrOutputValues);
  return newBrain;
}

void HumanBrain::resetBrain() { AbstractBrain::resetBrain(); }

void HumanBrain::update() {
  std::cout << "Inputs: ";
  for (int i = 0; i < nrInputValues; i++) {
    std::cout << inputValues[i] << " ";
  }
  std::cout << "\nLast Outputs: ";
  for (int i = 0; i < nrOutputValues; i++) {
    std::cout << outputValues[i] << " ";
  }
  std::cout << "\n";

  outputValues.assign(nrOutputValues, 0.0);
  char key;
  if (useActionMap) {
    std::cout << "please enter action (* for options): ";
    std::cin >> key;
    while (actionMap.find(key) == actionMap.end()) {
      if (key == '*') {
        std::cout << "actions:"
                  << "\n";
        for (auto iter = actionMap.begin(); iter != actionMap.end(); iter++) {
          std::cout << iter->first << " " << actionNames[iter->first] << " (";
          for (size_t index = 0; index < iter->second.size(); index++) {
            std::cout << iter->second[index] << " ";
          }
          std::cout << ")"
                    << "\n";
        }
      } else {
        std::cout << "action not found."
                  << "\n";
      }
      std::cout << "please enter action: ";
      std::cin >> key;
    }
    // if we get here, we have a good key, move it into outputs
    for (int i = 0; i < nrOutputValues;
         i++) { // pull one double for each output
      outputValues[i] = actionMap[key][i];
    }
  } else { // not using action map
    std::cout << "please enter outputs (separated by space): ";
    std::string inputString;
    getline(std::cin, inputString);
    std::stringstream ss(inputString);
    for (int i = 0; i < nrOutputValues;
         i++) { // pull one double for each output
      ss >> outputValues[i];
    }
  }
}

std::string HumanBrain::description() { return "Human Brain\n"; }

DataMap HumanBrain::getStats(std::string &prefix) { return DataMap(); }

void HumanBrain::initializeGenomes(
    std::unordered_map<std::string, std::shared_ptr<AbstractGenome>>
        &_genomes) {
  // do nothing;
}

std::shared_ptr<AbstractBrain>
HumanBrain::makeCopy(std::shared_ptr<ParametersTable> PT_) {
  if (PT_ == nullptr) {
    PT_ = PT;
  }
  auto newBrain =
      std::make_shared<HumanBrain>(nrInputValues, nrOutputValues, PT_);
  return newBrain;
}

