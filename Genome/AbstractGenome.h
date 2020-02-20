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

#include <cstdlib>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include <Utilities/Utilities.h>
#include <Utilities/Data.h>
#include <Utilities/Parameters.h>
#include <Utilities/Random.h>

class AbstractGenome {

public:
  static std::shared_ptr<ParameterLink<std::string>> genomeTypeStrPL;
  static std::shared_ptr<ParameterLink<double>> alphabetSizePL;
  static std::shared_ptr<ParameterLink<std::string>> genomeSitesTypePL;

  const std::shared_ptr<ParametersTable> PT;

  // Handlers are how you access Genomes for reading and writting.
  // to get a handle for a genome call that that genomes newHandler() method
  class Handler {
  public:
    // shared_ptr<AbstractGenome> genome;
    bool readDirection; // true = forward, false = backwards
    bool EOG;           // end of genome
    bool EOC; // end of chromosome - in this context chromosome is a subsection
              // of the genome
    // which after having passed we may want to perform some different behavior

    Handler() {
      readDirection = true;
      EOG = false;
      EOC = false;
    }

    Handler(std::shared_ptr<AbstractGenome> _genome,
            bool _readDirection = true) {
      readDirection = _readDirection;
      EOG = false;
      EOC = false;
    }

    virtual void resetEOG() {
      EOG = false;
      EOC = false;
    }

    virtual void resetEOC() { EOC = false; }

    virtual void setReadDirection(bool _readDirection) {
      readDirection = _readDirection;
    }

    virtual void toggleReadDirection() { readDirection = !readDirection; }

    virtual void resetHandler() = 0;
    virtual void resetHandlerOnChromosome() = 0;

    virtual ~Handler() {}

    //// convert genome sites at location index into a value in range [min,max]
    ///and advance index to the next unused site
    //// any sites used will be assigned code in codingRegions
    //// no undefined action, this function must be defined
    virtual int readInt(int valueMin, int valueMax, int code = -1,
                        int CodingRegionIndex = 0) = 0;

    virtual double readDouble(double valueMin, double valueMax, int code = -1,
                              int CodingRegionIndex = 0) {
      std::cout << "ERROR: readDouble(double valueMin, double valueMax, int code = "
              "-1, int CodingRegionIndex = 0) in AbstractGenome::Handler was "
              "called!\n This has not been implemented yet the chromosome "
              "class you are using!\n";
      exit(1);
    }

    virtual void writeInt(int value, int valueMin, int valueMax) = 0;
    virtual void writeDouble(double value, double valueMin, double valueMax) {
      std::cout << "ERROR: writeDouble(double value, double valueMin, double "
              "valueMax) in AbstractGenome::Handler was called!\n This has not "
              "been implemented yet the chromosome class you are using!\n";
      exit(1);
    }
    virtual std::vector<std::vector<int>> readTable(std::pair<int, int> tableSize,
                                          std::pair<int, int> tableMaxSize,
                                          std::pair<int, int> valueRange,
                                          int code = -1,
                                          int CodingRegionIndex = 0) = 0;
    virtual void advanceIndex(int distance = 1) = 0;

    virtual std::shared_ptr<AbstractGenome::Handler> makeCopy() {
      std::cout << "  ERROR :: in AbstractGenome::Handler::makeCopy() - You are "
              "using the abstract version of this function. This method has "
              "not been defined for the class you are using.\n  Exiting."
           << std::endl;
      exit(1);
    }

    virtual void copyTo(std::shared_ptr<Handler> to) = 0;

    virtual bool atEOG() { return false; }

    virtual bool atEOC() { return false; }

    virtual void printIndex() = 0;

    virtual bool inTelomere(int length) { return false; }

    virtual void randomize() = 0;
  };

  DataMap dataMap;
  std::vector<std::string>
      genomeFileColumns;         // =
                                 // {"ID","alphabetSize","chromosomeCount","chromosomeLength","sitesCount","genomeAncestors","sites"};
  std::vector<std::string> popFileColumns; // = {"genomeLength"};

  AbstractGenome() = delete;
  AbstractGenome(std::shared_ptr<ParametersTable> PT_) : PT(PT_) {}

  virtual ~AbstractGenome() = default;
  // virtual shared_ptr<AbstractGenome::Handler>
  // newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true)
  // override {

  virtual std::shared_ptr<AbstractGenome> makeCopy(std::shared_ptr<ParametersTable> PT_) {
    std::cout << "ERROR IN AbstractGenome::makeCopy() - You are using the abstract "
            "copy constructor for genomes. You must define your own"
         << std::endl;
    exit(1);
  }

  virtual std::shared_ptr<AbstractGenome> makeLike() = 0;

  virtual std::shared_ptr<AbstractGenome::Handler>
  newHandler(std::shared_ptr<AbstractGenome> _genome,
             bool _readDirection = true) = 0;
  virtual double getAlphabetSize() = 0;

  virtual void copyFrom(std::shared_ptr<AbstractGenome> from) = 0;

  virtual void fillRandom() = 0;

  //// gets data about genome which can be added to a data map
  //// data is in pairs of strings (key, value)
  //// the undefined action is to return an empty vector
  virtual DataMap getStats(std::string &name) {
    DataMap data;
    std::cout << "Warning! In AbstractGenome::getStats()...\n";
    return data;
  }

  // convert a genome into data map with data that can be saved to file
  virtual DataMap serialize(std::string &name) {
    std::cout << "ERROR! In AbstractGenome::serialize(). This method has not been "
            "written for the type of genome use are using.\n  Exiting.";
    exit(1);
  }

  // given a an unordered_map<string, string> and PT, load data into this genome
  virtual void deserialize(std::shared_ptr<ParametersTable> PT,
                           std::unordered_map<std::string, std::string> &orgData,
                           std::string &name) {
    std::cout << "ERROR! In AbstractGenome::deserialize(). This method has not been "
            "written for the type of genome use are using.\n  Exiting.";
    exit(1);
  }

  virtual std::string genomeToStr() {
    std::cout << "Warning! In AbstractGenome::genomeToStr()...\n";
    return "";
  }

  virtual void printGenome() {
    std::cout << "Warning! In AbstractGenome::printGenome()...\n";
  }

  virtual void loadGenomeFile(std::string fileName,
                              std::vector<std::shared_ptr<AbstractGenome>> &genomes) {
    std::cout << "loadGenomeFile() has not been defined for this type of genome!"
         << std::endl;
    exit(1);
  }

  // loadGenome will load file "fileName" and return the first genome whos
  // dataMap matches key,value
  // if no genome matches, function returns nullptr
  // if more then one genome matches, function return first match
  virtual std::shared_ptr<AbstractGenome> loadGenome(std::string fileName, std::string key,
                                                std::string value) {
    std::vector<std::shared_ptr<AbstractGenome>> genomes;
    loadGenomeFile(fileName, genomes);
    for (auto g : genomes) {
      if (std::to_string(g->dataMap.getIntVector(key)[0]) == value) {
        return g;
      }
    }
    return nullptr;
  }

  virtual bool isEmpty() = 0;

  virtual void mutate() = 0;
  virtual std::shared_ptr<AbstractGenome>
  makeMutatedGenomeFrom(std::shared_ptr<AbstractGenome> parent) = 0;
  virtual std::shared_ptr<AbstractGenome>
  makeMutatedGenomeFromMany(std::vector<std::shared_ptr<AbstractGenome>> parents) = 0;

  virtual int countSites() {
    std::cout << "Warning! In AbstractGenome::countSites()...\n";
    return 0;
  }

  virtual std::string getType() {
    std::cout << "ERROR! In AbstractGenome::getType()...\n This genome needs a "
            "getType function...\n  exiting.";
    exit(1);
    return "Undefined";
  }

  virtual void recordDataMap() = 0;
};

