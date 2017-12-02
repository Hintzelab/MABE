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

#include <stdlib.h>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "../../Utilities/Utilities.h"
#include "../../Utilities/Data.h"
#include "../../Utilities/Parameters.h"
#include "../../Utilities/Random.h"
#include "Chromosome/AbstractChromosome.h"
#include "Chromosome/TemplatedChromosome.h"
#include "../AbstractGenome.h"

using namespace std;

class MultiGenome: public AbstractGenome {
public:

	//static shared_ptr<string> test;

	static shared_ptr<ParameterLink<int>> initialPloidyPL;
	static shared_ptr<ParameterLink<int>> initialChromosomesPL;
	static shared_ptr<ParameterLink<int>> initialChromosomeSizePL;
	static shared_ptr<ParameterLink<double>> pointMutationRatePL;
	static shared_ptr<ParameterLink<double>> insertionRatePL;
	static shared_ptr<ParameterLink<int>> insertionMinSizePL;
	static shared_ptr<ParameterLink<int>> insertionMaxSizePL;
	static shared_ptr<ParameterLink<double>> deletionRatePL;
	static shared_ptr<ParameterLink<int>> deletionMinSizePL;
	static shared_ptr<ParameterLink<int>> deletionMaxSizePL;
	static shared_ptr<ParameterLink<int>> maxChromosomeSizePL;
	static shared_ptr<ParameterLink<int>> minChromosomeSizePL;
	static shared_ptr<ParameterLink<int>> crossCountPL;  // number of crosses to make when performing crossover

	//shared_ptr<ParameterLink<int>> initialPloidyLPL;
	//shared_ptr<ParameterLink<int>> initialChromosomesLPL;
	//shared_ptr<ParameterLink<int>> initialChromosomeSizeLPL;
	//shared_ptr<ParameterLink<double>> pointMutationRateLPL;
	//shared_ptr<ParameterLink<double>> insertionRateLPL;
	//shared_ptr<ParameterLink<int>> insertionMinSizeLPL;
	//shared_ptr<ParameterLink<int>> insertionMaxSizeLPL;
	//shared_ptr<ParameterLink<double>> deletionRateLPL;
	//shared_ptr<ParameterLink<int>> deletionMinSizeLPL;
	//shared_ptr<ParameterLink<int>> deletionMaxSizeLPL;
	//shared_ptr<ParameterLink<int>> maxChromosomeSizeLPL;
	//shared_ptr<ParameterLink<int>> minChromosomeSizeLPL;
	//shared_ptr<ParameterLink<int>> crossCountLPL;  // number of crosses to make when performing crossover

	class Handler: public AbstractGenome::Handler {
	public:
		shared_ptr<MultiGenome> genome;
		int siteIndex;
		int chromosomeIndex;

		Handler() = delete;

		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = 1);
		virtual ~Handler() = default;

		virtual void resetHandler() override;
		virtual void resetHandlerOnChromosome() override;

		// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
		// it uses readDirection to resolve them.	virtual void copyFrom(shared_ptr<Genome> from) {

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

		virtual void advanceChromosome();
		virtual void printIndex() override;
		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) override;
		virtual double readDouble(double valueMin, double valueMax, int code = -1, int CodingRegionIndex = 0) override;

		virtual void writeInt(int value, int valueMin, int valueMax) override;
		virtual void writeDouble(double value, double valueMin, double valueMax) override;

		virtual shared_ptr<AbstractGenome::Handler> makeCopy() override;

		// copy contents of this handler to "to"
		virtual void copyTo(shared_ptr<AbstractGenome::Handler> to) override;
		// true if handler is within length sites from end of a chromosome
		virtual bool inTelomere(int length) override;
		// move this handler to a random location in genome
		virtual void randomize() override;
		virtual vector<vector<int>> readTable(pair<int, int> tableSize, pair<int, int> tableMaxSize, pair<int, int> valueRange, int code = -1, int CodingRegionIndex = 0) override;

	};
public:
	int ploidy;
	vector<shared_ptr<AbstractChromosome>> chromosomes;

	MultiGenome() = delete;
	MultiGenome(shared_ptr<ParametersTable> _PT);
	MultiGenome(shared_ptr<AbstractChromosome> _chromosome, shared_ptr<ParametersTable> _PT);
	MultiGenome(shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount, int _plodiy, shared_ptr<ParametersTable> _PT);
	virtual shared_ptr<AbstractGenome> makeCopy(shared_ptr<ParametersTable> _PT) override;
	virtual ~MultiGenome() = default;

	virtual shared_ptr<AbstractGenome> makeLike() override {
		return make_shared<MultiGenome>(chromosomes[0], (int) chromosomes.size() / ploidy, ploidy, PT);
	}

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override;

	virtual double getAlphabetSize() override;

	// randomize this genomes contents
	virtual void fillRandom() override;

	// fill all sites of this genome with ascending values
	// This function is to make testing easy.
	virtual void fillAcending();

	// fill all sites of this genome with value
	// if "acendingChromosomes" = true, then increment value after each chromosome
	// This function is to make testing easy.
	virtual void fillConstant(int value, bool acendingChromosomes = false);

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
	virtual void copyFrom(shared_ptr<AbstractGenome> from) override;

// Mutation functions

	virtual int countSites() override;

	virtual bool isEmpty() override;

	// apply mutations to this genome
	virtual void mutate() override;

	// make a mutated genome. from this genome
	// the undefined action is to return a new genome
	virtual shared_ptr<AbstractGenome> makeMutatedGenomeFrom(shared_ptr<AbstractGenome> parent) override;

	// make a mutated genome from a vector or genomes
	// inherit the ParamatersTable from the 0th parent
	// assumes all genomes have the same number of chromosomes and same ploidy
	// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
	// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
	// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
	// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
	virtual shared_ptr<AbstractGenome> makeMutatedGenomeFromMany(vector<shared_ptr<AbstractGenome>> parents) override;

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
	virtual DataMap getStats(string& prefix) override;

	virtual string getType() override {
		return "Multi";
	}
	virtual DataMap serialize(string& name) override;
	virtual void deserialize(shared_ptr<ParametersTable> PT, unordered_map<string, string>& orgData, string& name) override;

	virtual void recordDataMap() override;

	// load all genomes from a file
	virtual void loadGenomeFile(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) override;
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//	virtual void loadGenome(string fileName, string key, string keyValue);

// Translation functions - convert genomes into usefull stuff

	// convert a genome to a string
	virtual string genomeToStr() override;

	virtual void printGenome() override;

};

inline shared_ptr<AbstractGenome> MultiGenome_genomeFactory(shared_ptr<ParametersTable> PT) {
	shared_ptr<AbstractChromosome> templateChromosome;
	string sitesType = AbstractGenome::genomeSitesTypePL->get(PT);
	double alphabetSize = AbstractGenome::alphabetSizePL->get(PT);
	int chromosomeSizeInitial = MultiGenome::initialChromosomeSizePL->get(PT);
	int chromosome_sets = MultiGenome::initialChromosomesPL->get(PT);
	int chromosome_ploidy = MultiGenome::initialPloidyPL->get(PT);
	if (sitesType == "char") {
		templateChromosome = make_shared<TemplatedChromosome<unsigned char>>(chromosomeSizeInitial, alphabetSize);
	} else if (sitesType == "int") {
		templateChromosome = make_shared<TemplatedChromosome<int>>(chromosomeSizeInitial, alphabetSize);
	} else if (sitesType == "double") {
		templateChromosome = make_shared<TemplatedChromosome<double>>(chromosomeSizeInitial, alphabetSize);
	} else if (sitesType == "bool") {
		templateChromosome = make_shared<TemplatedChromosome<bool>>(chromosomeSizeInitial, alphabetSize);
	} else {
		cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << sitesType << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}
	return make_shared<MultiGenome>(templateChromosome, chromosome_sets, chromosome_ploidy, PT);
}



