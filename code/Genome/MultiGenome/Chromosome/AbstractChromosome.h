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

#include <algorithm>
#include <set>
#include <cstdlib>
#include <vector>

#include "../../../Utilities/Utilities.h"
#include "../../../Utilities/Data.h"
#include "../../../Utilities/Parameters.h"
#include "../../../Utilities/Random.h"

class AbstractChromosome {
public:
  double alphabetSize; // values in this chromosome will be in the range of 0 to
                       // < alphabetSize
  // i.e. the number of diffrent values that can be at a site
  // for double this is the range of the value

  virtual ~AbstractChromosome() {}
  virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax,
                       bool readDirection, int code = -1,
                       int CodingRegionIndex = 0) = 0;
  virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax,
                        bool readDirection) = 0;
  virtual bool writeDouble(int &siteIndex, double value, double valueMin,
                           double valueMax, bool readDirection) {
    std::cout << "ERROR: writeDouble() in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual bool siteToDouble(int &siteIndex, double &value, double valueMin,
                            double valueMax, bool readDirection, int code,
                            int CodingRegionIndex) {
    std::cout << "ERROR: siteToDouble() in AbstractChromosome was called!\n This "
            "has not been implemented yet the chromosome class you are "
            "using!\n";
    exit(1);
  }

  virtual std::shared_ptr<AbstractChromosome> makeLike() = 0;

  virtual std::shared_ptr<AbstractChromosome> makeCopy() = 0;

  virtual void fillRandom() {
    std::cout << "ERROR: fillRandom() in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }
  virtual void fillRandom(int length) {
    std::cout << "ERROR: fillRandom(int length) in AbstractChromosome was called!\n "
            "This has not been implemented yet the chromosome class you are "
            "using!\n";
    exit(1);
  }

  virtual void fillAcending(int &start) {
    std::cout << "ERROR: fillAcending(int &start) in AbstractChromosome was "
            "called!\n This has not been implemented yet the chromosome class "
            "you are using!\n";
    exit(1);
  }

  virtual void fillConstant(const int value) {
    std::cout << "ERROR: fillConstant(const int value) in AbstractChromosome was "
            "called!\n This has not been implemented yet the chromosome class "
            "you are using!\n";
    exit(1);
  }

  virtual void readChromosomeFromSS(std::stringstream &ss, int _chromosomeLength) {
    std::cout << "ERROR: readChromosomeFromSS(stringstream &ss, int "
            "_chromosomeLength) in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual std::string chromosomeToStr() = 0;

  virtual void resize(int size) {
    std::cout << "ERROR: resize(int size) in AbstractChromosome was called!\n This "
            "has not been implemented yet the chromosome class you are "
            "using!\n";
    exit(1);
  }

  virtual int size() {
    std::cout << "ERROR: size() in AbstractChromosome was called!\n This has not "
            "been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual inline bool modulateIndex(int &siteIndex) {
    std::cout << "ERROR: modulateIndex(int &siteIndex) in AbstractChromosome was "
            "called!\n This has not been implemented yet the chromosome class "
            "you are using!\n";
    exit(1);
  }
  virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1,
                                   int distance = 1) {
    std::cout << "ERROR: advanceIndex(int &siteIndex, bool readDirection = 1, int "
            "distance = 1) in AbstractChromosome was called!\n This has not "
            "been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual DataMap getFixedStats() {
    std::cout << "ERROR: getFixedStats() in AbstractChromosome was called!\n This "
            "has not been implemented yet the chromosome class you are "
            "using!\n";
    exit(1);
  }

  virtual DataMap getStats() {
    std::cout << "ERROR: getStats() in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual std::shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize) {
    std::cout << "ERROR: getSegment(int minSize, int maxSize) in AbstractChromosome "
            "was called!\n This has not been implemented yet the chromosome "
            "class you are using!\n";
    exit(1);
  }

  virtual void insertSegment(std::shared_ptr<AbstractChromosome> segment) {
    std::cout << "ERROR: insertSegment(shared_ptr<AbstractChromosome> segment) in "
            "AbstractChromosome was called!\n This has not been implemented "
            "yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual void mutatePoint() {
    std::cout << "ERROR: mutatePoint() in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) {
    std::cout << "ERROR: mutateCopy(int minSize, int maxSize, int "
            "chromosomeSizeMax) in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) {
    std::cout << "ERROR: mutateDelete(int minSize, int maxSize, int "
            "chromosomeSizeMin) in AbstractChromosome was called!\n This has "
            "not been implemented yet the chromosome class you are using!\n";
    exit(1);
  }

  virtual void crossover(std::vector<std::shared_ptr<AbstractChromosome>> parents,
                         int crossCount) {
    std::cout << "ERROR: crossover(vector<shared_ptr<AbstractChromosome>> parents, "
            "int crossCount) in AbstractChromosome was called!\n This has not "
            "been implemented yet the chromosome class you are using!\n";
    exit(1);
  }
};

