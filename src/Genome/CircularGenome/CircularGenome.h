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
#include <limits>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include <Utilities/Utilities.h>
#include <Utilities/Data.h>
#include <Utilities/Parameters.h>
#include <Utilities/Random.h>
#include <Genome/AbstractGenome.h>

// needed to move static values to own class because of templating.
class CircularGenomeParameters {
public:
	static std::shared_ptr<ParameterLink<int>> sizeInitialPL;
	static std::shared_ptr<ParameterLink<double>> mutationPointRatePL;
	static std::shared_ptr<ParameterLink<double>> mutationPointOffsetRatePL;
	static std::shared_ptr<ParameterLink<double>> mutationPointOffsetRangePL;
    static std::shared_ptr<ParameterLink<bool>> mutationPointOffsetUniformPL;
	static std::shared_ptr<ParameterLink<double>> mutationCopyRatePL;
	static std::shared_ptr<ParameterLink<int>> mutationCopyMinSizePL;
	static std::shared_ptr<ParameterLink<int>> mutationCopyMaxSizePL;
	static std::shared_ptr<ParameterLink<double>> mutationDeleteRatePL;
	static std::shared_ptr<ParameterLink<int>> mutationDeleteMinSizePL;
	static std::shared_ptr<ParameterLink<int>> mutationDeleteMaxSizePL;
	static std::shared_ptr<ParameterLink<int>> sizeMaxPL;
	static std::shared_ptr<ParameterLink<int>> sizeMinPL;
	static std::shared_ptr<ParameterLink<int>> mutationCrossCountPL;  // number of crosses to make when performing crossover

	static std::shared_ptr<ParameterLink<double>> mutationIndelRatePL;
	static std::shared_ptr<ParameterLink<int>> mutationIndelMinSizePL;
	static std::shared_ptr<ParameterLink<int>> mutationIndelMaxSizePL;
	static std::shared_ptr<ParameterLink<int>> mutationIndelInsertMethodPL;
	static std::shared_ptr<ParameterLink<bool>> mutationIndelCopyFirstPL;

};

template<class T>
class CircularGenome : public AbstractGenome {

public:

	class Handler : public AbstractGenome::Handler {
	public:
		std::shared_ptr<CircularGenome> genome;
		int siteIndex;

		Handler() = delete;

		Handler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection = 1);
		virtual ~Handler() = default;

		virtual void resetHandler() override;
		virtual void resetHandlerOnChromosome() override;

		// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
		// it uses readDirection to resolve them.	virtual void copyFrom(std::shared_ptr<Genome> from) {

				//  modulate index truncates nonexistant sites. i.e. if the current addres is chromosome 1, site 10 and
		// chromosome 10 is 8 long, modulateIndex will set the index to chromosome 2, site 0 (not site 2).
		// If this behavior is required, use advance Index instead.
		// If the chromosomeIndex has past the last chromosome (or the first
		// if read direction = -1) then EOG (end of genome) is set true.
		virtual void modulateIndex();

		// advanceIndex will move the index forward distance sites.
		// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
		// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
		// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
		// site 5 of the next chromosome. Should this be fixed?!??
		virtual void advanceIndex(int distance = 1) override;

		// returns true if this Handler has reached the end of genome (or start if direction is backwards).
		virtual bool atEOG() override;
		virtual bool atEOC() override;

		virtual void printIndex() override;
		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) override;
		virtual double readDouble(double valueMin, double valueMax, int code = -1, int CodingRegionIndex = 0) override;

		virtual void writeInt(int value, int valueMin, int valueMax) override;
		virtual void writeDouble(double value, double valueMin, double valueMax) override;

		virtual std::shared_ptr<AbstractGenome::Handler> makeCopy() override;

		// copy contents of this handler to "to"
		virtual void copyTo(std::shared_ptr<AbstractGenome::Handler> to) override;
		// true if handler is within length sites from end of a chromosome
		virtual bool inTelomere(int length) override;
		// move this handler to a random location in genome
		virtual void randomize() override;
		virtual std::vector<std::vector<int>> readTable(std::pair<int, int> tableSize, std::pair<int, int> tableMaxSize, std::pair<int, int> valueRange, int code = -1, int CodingRegionIndex = 0) override;

	};

	std::vector<T> sites;
	double alphabetSize;

	CircularGenome() = delete;

	CircularGenome(std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
		setupCircularGenome(256, 100);
	}

	//	CircularGenome(double _alphabetSize, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_){
	//		alphabetSize = _alphabetSize;
	//	}

	CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_);

	virtual std::shared_ptr<AbstractGenome> makeCopy(std::shared_ptr<ParametersTable> PT_);

	virtual ~CircularGenome() = default;

	virtual void setupCircularGenome(int _size, double _alphabetSize);

	virtual std::shared_ptr<AbstractGenome> makeLike() override {
		return std::make_shared<CircularGenome<T>>(alphabetSize, (int)sites.size(), PT);
	}

	virtual int size();
	virtual std::shared_ptr<AbstractGenome::Handler> newHandler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override;

	virtual double getAlphabetSize() override;

	// randomize this genomes contents
	virtual void fillRandom() override;

	// fill all sites of this genome with ascending values
	// This function is to make testing easy.
	virtual void fillAcending();

	// fill all sites of this genome with value
	// if "acendingChromosomes" = true, then increment value after each chromosome
	// This function is to make testing easy.
	virtual void fillConstant(int value);

	// Copy functions

	// copy the contents of another genome to this genome
	// no undefined action, this function must be defined
	virtual void copyFrom(std::shared_ptr<AbstractGenome> from) override;

	// Mutation functions

	virtual int countSites();

	virtual bool isEmpty() override;

	virtual void pointMutate(double range = -1);

	int countPoint = 0;
	int countPointOffset = 0;
	int countDelete = 0;
	int countCopy = 0;
	int countIndel = 0;

	virtual int incrementCopy();
	virtual int incrementPoint();
	virtual int incrementPointOffset();
	virtual int incrementDelete();
	virtual int incrementIndel();

	// apply mutations to this genome
	virtual void mutate() override;

	// make a mutated genome. from this genome
	// the undefined action is to return a new genome
	virtual std::shared_ptr<AbstractGenome> makeMutatedGenomeFrom(std::shared_ptr<AbstractGenome> parent) override;

	// make a mutated genome from a vector or genomes
	// inherit the ParamatersTable from the 0th parent
	// assumes all genomes have the same number of chromosomes and same ploidy
	// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
	// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
	// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
	// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
	virtual std::shared_ptr<AbstractGenome> makeMutatedGenomeFromMany(std::vector<std::shared_ptr<AbstractGenome>> parents) override;

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
	virtual DataMap getStats(std::string& prefix) override;

	virtual std::string getType() override{
		return "Circular";
	}

	virtual DataMap serialize(std::string& name) override;
	virtual void deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string>& orgData, std::string& name) override;

	virtual void recordDataMap() override;

	// load all genomes from a file
	//virtual void loadGenomeFile(string fileName, vector<std::shared_ptr<AbstractGenome>> &genomes) override;
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//	virtual void loadGenome(string fileName, string key, string keyValue);

// Translation functions - convert genomes into usefull stuff

	// convert a genome to a string
	virtual std::string genomeToStr() override;

	virtual void printGenome() override;

};

inline std::shared_ptr<AbstractGenome> CircularGenome_genomeFactory(std::shared_ptr<ParametersTable> PT) {
	std::shared_ptr<AbstractGenome> newGenome;
	std::string sitesType = AbstractGenome::genomeSitesTypePL->get(PT);
	double alphabetSize = AbstractGenome::alphabetSizePL->get(PT);
	int sizeInitial = CircularGenomeParameters::sizeInitialPL->get(PT);

	if (sitesType == "char") {
		newGenome = std::make_shared<CircularGenome<unsigned char>>(alphabetSize, sizeInitial, PT);
	} else if (sitesType == "int") {
		newGenome = std::make_shared<CircularGenome<int>>(alphabetSize, sizeInitial, PT);
	} else if (sitesType == "double") {
		newGenome = std::make_shared<CircularGenome<double>>(alphabetSize, sizeInitial, PT);
	} else if (sitesType == "bool") {
		newGenome = std::make_shared<CircularGenome<bool>>(alphabetSize, sizeInitial, PT);
	} else {
		std::cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << sitesType << "\" is not defined.\n\nExiting.\n" << std::endl;
		exit(1);
	}
	return newGenome;
}

