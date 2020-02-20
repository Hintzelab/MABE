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

#include "AbstractChromosome.h"

template <class T> class TemplatedChromosome : public AbstractChromosome {
  std::vector<T> sites;

public:
  class CodingRegions {
  private:
    std::map<int, std::vector<std::pair<int, int>>> regions;

  public:
    // set a coding region value if that value is > -1 (values < 0 denote that
    // the
    // action should not be recorded in codingRegions
    virtual void assignCode(int code, int siteIndex, int CodingRegionIndex);

    // converts codingRegions to a string. If argument is provided, then only
    // that region will
    // will be returned (or a blank string)
    virtual std::string codingRegionsToString(int CodingRegionIndex = -1);
  };

  CodingRegions codingRegions;

  // default fail constructor - if alphebetSize default has not been explicitly
  // defined, this is run.
  TemplatedChromosome();

  TemplatedChromosome(int chromosomeLength);

  TemplatedChromosome(int chromosomeLength, double _alphabetSize);

  virtual ~TemplatedChromosome() = default;

  // return a shared_ptr to a new chromosome like this one (same alphabetSize
  // and sites.size())
  virtual std::shared_ptr<AbstractChromosome> makeLike();
  virtual std::shared_ptr<AbstractChromosome> makeCopy();

  // insures that a site index is valid, if in index is > sites.size(), mod it.
  // return true if siteIndex went out of range
  virtual inline bool modulateIndex(int &siteIndex) override {
    // cout << " In modulateIndex " << "   " << siteIndex << endl;
    if (siteIndex >= (int)sites.size() || siteIndex < 0) {
      siteIndex = loopMod(siteIndex, (int)sites.size());
      // cout << "   in loop " << siteIndex << endl;
      return true;
    }
    // cout << "   after: " << siteIndex << endl;

    return false;
  }

  // advance or reverse (if readDirection = false) a site index 1 or distance
  // sites and check that new index is valid
  // return true if siteIndex went out of range
  virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1,
                                   int distance = 1) override {
    std::string rd = (readDirection) ? "forward " + std::to_string(distance)
                                : "backward " + std::to_string(distance);
    // cout << " In advanceIndex " << rd << endl;
    siteIndex += (readDirection) ? distance : (-1 * distance); // move index
    return modulateIndex(siteIndex); // confirm that new index is in range
  }

  // read an int value in range [valueMin,valueMax] from chromosome starting at
  // index.
  // will use as many sites as needed. works in base alphabetSize
  // return true if siteIndex went out of range
  virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax,
                       bool readDirection, int code = -1,
                       int CodingRegionIndex = 0) override;

  // writes a value into a chromosome, uses a number of sites baised on valueMin
  // and valueMax);
  // return true if siteIndex went out of range
  virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax,
                        bool readDirection) override;
  // read one site of type T
  // return true if siteIndex went out of range
  virtual bool readSite(int &siteIndex, T &value, bool readDirection,
                        int code = -1, int CodingRegionIndex = 0);

  // scale value using valueMin and valueMax to alphabetSize and write at
  // siteIndex
  // value - MIN(valueMin,valueMax) must be < ABS(valueMax - valueMin)
  virtual bool writeDouble(int &siteIndex, double value, double valueMin,
                           double valueMax, bool readDirection) override;

  // this is a scaling function - while it will work with other types it should
  // only be used with double and float
  virtual bool siteToDouble(int &siteIndex, double &value, double valueMin,
                            double valueMax, bool readDirection, int code = -1,
                            int CodingRegionIndex = 0) override;

  // resize a chromosome to length and fill will values from alphabet
  virtual void fillRandom(int length) override;
  virtual void fillRandom() override;

  // starting with value start, fill this chromosome with acending values.
  // This function is to make testing easy.
  virtual void fillAcending(int &start) override;

  // fill all the sites of this chromosome with value.
  // This function is to make testing easy.
  virtual void fillConstant(const int value) override;

  virtual void readChromosomeFromSS(std::stringstream &ss,
                                    int _chromosomeLength) override;
  // convert a chromosome to a string
  virtual std::string chromosomeToStr() override;
  virtual void resize(int size) override;
  virtual int size() override;
  virtual DataMap getFixedStats() override;
  virtual DataMap getStats() override;
  virtual std::string codingRegionsToString();

  //////////////////////////////
  // MUTATION RELATED METHODS //
  //////////////////////////////

  // create a new chromosome with a copied peice of this chromosome. The size of
  // the copy
  // will be random between minSize and maxSize. the copy location will be
  // determined randomly
  // but will at lease segmentSize before the end of the chromosome.
  virtual std::shared_ptr<AbstractChromosome> getSegment(int minSize,
                                                    int maxSize) override;
  // insert from segement into this chromosome. The insertion location is
  // random.
  virtual void insertSegment(std::shared_ptr<AbstractChromosome> segment) override;
  virtual void mutatePoint() override;
  // mutate chromosome by getting a copy of a segment of this chromosome and
  // inserting that segment randomly into this chromosome
  virtual void mutateCopy(int minSize, int maxSize,
                          int chromosomeSizeMax) override;
  // delete a random segement from the chromosome
  virtual void mutateDelete(int minSize, int maxSize,
                            int chromosomeSizeMin) override;
  // delete the sites of this chromosome. Then set sites to a crossed over
  // chromosome made up of parents
  virtual void crossover(std::vector<std::shared_ptr<AbstractChromosome>> parents,
                         int crossCount) override;
};

