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

#include <cmath>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

//#include "Gate/GateBuilder.h"
//#include "GateListBuilder/GateListBuilder.h"
#include "../Genome/AbstractGenome.h"
#include "../Utilities/Parameters.h"
#include "../Global.h"

class AbstractBrain {
public:
  static std::shared_ptr<ParameterLink<std::string>> brainTypeStrPL;

  const std::shared_ptr<ParametersTable> PT;

  std::vector<std::string> popFileColumns;

  bool recordActivity;
  std::string recordActivityFileName;

  int nrInputValues;
  int nrOutputValues;
  std::vector<double> inputValues;
  std::vector<double> outputValues;

  AbstractBrain() = delete;

  AbstractBrain(int ins, int outs, std::shared_ptr<ParametersTable> PT_)
      : PT(PT_) {
    nrInputValues = ins;
    nrOutputValues = outs;
    recordActivity = false;

    inputValues.resize(nrInputValues);
    outputValues.resize(nrOutputValues);
  }

  virtual ~AbstractBrain() = default;

  virtual void update() = 0;

  virtual std::string
  description() = 0; // returns a desription of this brain in it's current state
  virtual DataMap getStats(std::string &prefix) = 0; // returns a vector of string
                                                // pairs of any stats that can
                                                // then be used for data
                                                // tracking (etc.)
  virtual std::string getType() {
    std::cout << "ERROR! In AbstractBrain::getType()...\n This genome needs a "
            "getType function...\n  exiting.";
    exit(1);
    return "Undefined";
  }

  // convert a brain into data map with data that can be saved to file
  virtual DataMap serialize(std::string &name) {
    // cout << "ERROR! In AbstractBrain::serialize(). This method has not been
    // written for the type of brain use are using.\n  Exiting.";
    // exit(1);
    // return an empty data map - this is the case for a brain built from genome
    // where no data needs to be saved
    DataMap tempDataMap;
    return tempDataMap;
  }

  // given an unordered_map<string, string> and PT, load data into this brain
  virtual void deserialize(std::shared_ptr<ParametersTable> PT,
                           std::unordered_map<std::string, std::string> &orgData,
                           std::string &name) {
    // cout << "ERROR! In AbstractBrain::deserialize(). This method has not been
    // written for the type of brain use are using.\n  Exiting.";
    // exit(1);
    // do nothing... if this brain is built from genome, then nothing needs to
    // be done.
  }

  virtual void initializeGenomes(
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes){
      // do nothing by default... if this is a direct encoded brain, then no
      // action is needed.
      // should this be a madiatory function?
  };

  // Make a brain like the brain that called this function, using genomes and
  // initalizing other elements.
  virtual std::shared_ptr<AbstractBrain>
  makeBrain(std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
    std::cout << "WARRNING! you have called AbstractBrain::encodeBarin - This "
            "function must be defined from each brain type."
         << std::endl
         << "Exiting." << std::endl;
    exit(1);
    return makeCopy();
  }

  // Make a brain like the brain that called this function, using genomes and
  // inheriting other elements from parent.
  // in the default case, we assume geneticly encoded brains, so this just calls
  // the no parent version (i.e. build from genomes)
  virtual std::shared_ptr<AbstractBrain>
  makeBrainFrom(std::shared_ptr<AbstractBrain> parent,
                std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
    return makeBrain(_genomes);
  }

  // Make a brain like the brain that called this function, using genomes and
  // inheriting other elements from parents.
  // in the default case, we assume geneticly encoded brains, so this just calls
  // the no parent version (i.e. build from genomes)
  virtual std::shared_ptr<AbstractBrain> makeBrainFromMany(
      std::vector<std::shared_ptr<AbstractBrain>> parents,
      std::unordered_map<std::string, std::shared_ptr<AbstractGenome>> &_genomes) {
    return makeBrain(_genomes);
  }

  // apply direct mutations to this brain
  virtual void mutate() {
    // do nothing by default... if this is not a direct encoded brain, then no
    // action is needed.
    // should this be a madiatory function?
  }

  virtual void inline resetBrain() {
    resetInputs();
    resetOutputs();
  }

  virtual void inline setRecordActivity(bool _recordActivity) {
    recordActivity = _recordActivity;
  }

  virtual void inline setRecordFileName(std::string _recordActivityFileName) {
    recordActivityFileName = _recordActivityFileName;
  }

  virtual void inline resetOutputs() {
    for (int i = 0; i < nrOutputValues; i++) {
      outputValues[i] = 0.0;
    }
  }

  virtual void inline resetInputs() {
    for (int i = 0; i < nrInputValues; i++) {
      inputValues[i] = 0.0;
    }
  }

  inline virtual void setInput(const int &inputAddress, const double &value) {
    if (inputAddress < nrInputValues) {
      inputValues[inputAddress] = value;
    } else {
      std::cout << "in Brain::setInput() : Writing to invalid input ("
           << inputAddress << ") - this brain needs more inputs!\nExiting"
           << std::endl;
      exit(1);
    }
  }

  inline virtual double readInput(const int &inputAddress) {
    if (inputAddress < nrInputValues) {
      return inputValues[inputAddress];
    } else {
      std::cout << "in Brain::readInput() : Reading from invalid input ("
           << inputAddress << ") - this brain needs more inputs!\nExiting"
           << std::endl;
      exit(1);
    }
  }

  inline virtual void setOutput(const int &outputAddress, const double &value) {
    if (outputAddress < nrOutputValues) {
      outputValues[outputAddress] = value;
    } else {
      std::cout << "in Brain::setOutput() : Writing to invalid output ("
           << outputAddress << ") - this brain needs more outputs!\nExiting"
           << std::endl;
      exit(1);
    }
  }

  inline virtual double readOutput(const int &outputAddress) {
    if (outputAddress < nrOutputValues) {
      return outputValues[outputAddress];
    } else {
      std::cout << "in Brain::readOutput() : Reading from invalid output ("
           << outputAddress << ") - this brain needs more outputs!\nExiting"
           << std::endl;
      exit(1);
    }
  }

  //	// converts the value of each value in nodes[] to bit and converts the
  //bits to an int
  //	// useful to generate values for lookups, useful for caching
  //	int allNodesToBitToInt() {
  //		int result = 0;
  //		for (int i = 0; i < nrOfBrainNodes; i++)
  //			result = (result) + Bit(nodes[i]);
  //		return result;
  //
  //	}

  virtual std::shared_ptr<AbstractBrain>
  makeCopy(std::shared_ptr<ParametersTable> PT_ = nullptr) {
    std::cout << "ERROR IN AbstractBrain::makeCopy() - You are using the abstract "
            "copy constructor for brains. You must define your own"
         << std::endl;
    exit(1);
  }

  virtual std::unordered_set<std::string> requiredGenomes() {
    return {"root::"};
    // "root" = use empty name space
    // "GROUP::" = use group name space
    // "blah" = use "blah namespace at root level
    // "Group::blah" = use "blah" name space inside of group name space
  }
};
