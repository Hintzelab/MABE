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
#include <stdlib.h>
#include <vector>

#include "../../../Utilities/Utilities.h"
#include "../../../Utilities/Data.h"
#include "../../../Utilities/Parameters.h"
#include "../../../Utilities/Random.h"

using namespace std;

class AbstractChromosome {
 public:
	double alphabetSize;  // values in this chromosome will be in the range of 0 to < alphabetSize
	                      // i.e. the number of diffrent values that can be at a site
	                      // for double this is the range of the value

	virtual ~AbstractChromosome() {
	}
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0) = 0;
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax, bool readDirection) = 0;
	virtual bool writeDouble(int &siteIndex, double value, double valueMin, double valueMax, bool readDirection) {
		cout << "ERROR: writeDouble() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, bool readDirection, int code, int CodingRegionIndex) {
		cout << "ERROR: siteToDouble() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;
	}

	virtual shared_ptr<AbstractChromosome> makeLike() = 0;

	virtual shared_ptr<AbstractChromosome> makeCopy() = 0;

	virtual void fillRandom() {
		cout << "ERROR: fillRandom() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}
	virtual void fillRandom(int length) {
		cout << "ERROR: fillRandom(int length) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void fillAcending(int &start) {
		cout << "ERROR: fillAcending(int &start) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void fillConstant(const int value) {
		cout << "ERROR: fillConstant(const int value) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void readChromosomeFromSS(stringstream &ss, int _chromosomeLength) {
		cout << "ERROR: readChromosomeFromSS(stringstream &ss, int _chromosomeLength) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual string chromosomeToStr() = 0;

	virtual void resize(int size) {
		cout << "ERROR: resize(int size) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual int size() {
		cout << "ERROR: size() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return -1;
	}

	virtual inline bool modulateIndex(int &siteIndex) {
		cout << "ERROR: modulateIndex(int &siteIndex) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;  // not at end of chromosome
	}
	virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) {
		cout << "ERROR: advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual DataMap getFixedStats() {
		cout << "ERROR: getFixedStats() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		DataMap data;
		return data;
	}

	virtual DataMap getStats() {
		cout << "ERROR: getStats() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		DataMap data;
		return data;
	}

	virtual shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize) {
		cout << "ERROR: getSegment(int minSize, int maxSize) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return nullptr;
	}

	virtual void insertSegment(shared_ptr<AbstractChromosome> segment) {
		cout << "ERROR: insertSegment(shared_ptr<AbstractChromosome> segment) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutatePoint() {
		cout << "ERROR: mutatePoint() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) {
		cout << "ERROR: mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) {
		cout << "ERROR: mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) {
		cout << "ERROR: crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}
};

