//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "MultiGenome.h"
#include <Global.h>

// Initialize Parameters

std::shared_ptr<ParameterLink<int>> MultiGenome::initialPloidyPL = Parameters::register_parameter("GENOME_MULTI-chromosome_ploidy", 1, "number of chromosomes in each chromosome_set");
std::shared_ptr<ParameterLink<int>> MultiGenome::initialChromosomesPL = Parameters::register_parameter("GENOME_MULTI-chromosome_sets", 1, "number of chromosome sets (e.g. chromosome_set = 2 and chromosome_ploidy = 3 will result in a genome with 6 chromosomes)");

std::shared_ptr<ParameterLink<int>> MultiGenome::initialChromosomeSizePL = Parameters::register_parameter("GENOME_MULTI-chromosomeSizeInitial", 5000, "starting size for all chromosomes in genome (inital genome size will be chromosomeSizeInitial * chromosome_sets * chromosome_ploidy)");
std::shared_ptr<ParameterLink<double>> MultiGenome::pointMutationRatePL = Parameters::register_parameter("GENOME_MULTI-mutationPointRate", 0.005, "per site point mutation rate");
std::shared_ptr<ParameterLink<double>> MultiGenome::insertionRatePL = Parameters::register_parameter("GENOME_MULTI-mutationCopyRate", 0.00002, "per site insertion rate");
std::shared_ptr<ParameterLink<int>> MultiGenome::insertionMinSizePL = Parameters::register_parameter("GENOME_MULTI-mutationCopyMinSize", 128, "minimum size of insertion mutation");
std::shared_ptr<ParameterLink<int>> MultiGenome::insertionMaxSizePL = Parameters::register_parameter("GENOME_MULTI-mutationCopyMaxSize", 512, "maximum size of insertion mutation");
std::shared_ptr<ParameterLink<double>> MultiGenome::deletionRatePL = Parameters::register_parameter("GENOME_MULTI-mutationDeleteRate", 0.00002, "per site deletion rate");
std::shared_ptr<ParameterLink<int>> MultiGenome::deletionMinSizePL = Parameters::register_parameter("GENOME_MULTI-mutationDeleteMinSize", 128, "minimum size of insertion mutation");
std::shared_ptr<ParameterLink<int>> MultiGenome::deletionMaxSizePL = Parameters::register_parameter("GENOME_MULTI-mutationDeleteMaxSize", 512, "maximum size of insertion mutation");
std::shared_ptr<ParameterLink<int>> MultiGenome::minChromosomeSizePL = Parameters::register_parameter("GENOME_MULTI-chromosomeSizeMin", 2000, "if a chromosome is smaller then this, mutations will only increase chromosome size");
std::shared_ptr<ParameterLink<int>> MultiGenome::maxChromosomeSizePL = Parameters::register_parameter("GENOME_MULTI-chromosomeSizeMax", 20000, "if a chromosome is larger then this, mutations will only decrease chromosome size");

std::shared_ptr<ParameterLink<int>> MultiGenome::crossCountPL = Parameters::register_parameter("GENOME_MULTI-mutationCrossCount", 3, "number of crosses when performing crossover");

// constructor
MultiGenome::Handler::Handler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	genome = std::dynamic_pointer_cast<MultiGenome>(_genome);
	setReadDirection(_readDirection);
	resetHandler();
}

void MultiGenome::Handler::resetHandler() {
	if (readDirection) {  // if reading forward
		chromosomeIndex = 0;
		siteIndex = 0;
	} else {  // if reading backwards
		chromosomeIndex = ((int) genome->chromosomes.size()) - 1;  // set to last chromosome
		siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // set to last site in last chromosome
	}
	resetEOG();
	resetEOC();
}

void MultiGenome::Handler::resetHandlerOnChromosome() {
	if (readDirection) {  // if reading forward
		siteIndex = 0;
	} else {  // if reading backwards
		siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // set to last site in last chromosome
	}
	resetEOC();
}
// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
// it uses readDirection to resolve them.	virtual void copyFrom(std::shared_ptr<Genome> from) {

//  modulate index truncates nonexistant sites. i.e. if the current addres is chromosome 1, site 10 and
// chromosome 10 is 8 long, modulateIndex will set the index to chromosome 2, site 0 (not site 2).
// If this behavior is required, use advance Index instead.
// If the chromosomeIndex has past the last chromosome (or the first
// if read direction = -1) then EOG (end of genome) is set true.
void MultiGenome::Handler::modulateIndex() {
	if (readDirection) {
		// first see if we are past last chromosome
		if (chromosomeIndex >= (int) genome->chromosomes.size()) {
			chromosomeIndex = 0;  // reset chromosomeIndex
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}

		// now that we know that the chromosome index is in range, check the site index
		if (genome->chromosomes[chromosomeIndex]->modulateIndex(siteIndex)) {
			chromosomeIndex++;  // if the site index is out of range, increment the chromosomeIndex
			// now that we know that the site index is also in range, we have to check the chromosome index again!
			if (chromosomeIndex >= (int) genome->chromosomes.size()) {
				chromosomeIndex = 0;  // if the site index is out of range, increment the chromosomeIndex
				siteIndex = 0;  // ... and reset the site index
				EOG = true;
			};
			siteIndex = 0;  // ... and reset the site index
			EOC = true;
		}
	} else {  //reading backwards!
		// first see if we are past last chromosome
		if (chromosomeIndex < 0) {
			chromosomeIndex = ((int) genome->chromosomes.size()) - 1;  // reset chromosomeIndex (to last chromosome)
			siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // reset siteIndex (to last site in this chromosome)
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}

		// now that we know that the chromosome index is in range, check the site index
		if (genome->chromosomes[chromosomeIndex]->modulateIndex(siteIndex)) {
			chromosomeIndex--;  // if the site index is out of range, increment the chromosomeIndex
			// now that we know that the site index is also in range, we have to check the chromosome index again!
			if (chromosomeIndex < 0) {
				chromosomeIndex = ((int) genome->chromosomes.size()) - 1;  // if the site index is out of range, decrement the chromosomeIndex
				EOG = true;
			}
			siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // reset siteIndex (to last site in this chromosome)
			EOC = true;
		}

	}
}

// advanceIndex will move the index forward distance sites.
// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
// site 5 of the next chromosome. Should this be fixed?!??
void MultiGenome::Handler::advanceIndex(int distance) {
	//modulateIndex();
	if (readDirection) {  // reading forward
		if ((genome->chromosomes[chromosomeIndex]->size() - siteIndex) > distance) {
			siteIndex += distance;  // if there are enough sites left in the current chromosome, just move siteIndex
		} else {  // there are not enough sites in the current chromosome, must move to next chromosome
			distance = distance - (genome->chromosomes[chromosomeIndex]->size() - siteIndex);
			// reduce distance by the number of sites between siteIndex and end of Chromosome
			advanceChromosome();  // advance to the next chromosome;
			advanceIndex(distance);  // advanceIndex by remaining distance
		}

	} else {  // reading backwards

		if (siteIndex > distance) {
			siteIndex -= distance;  // if there are enough sites in the current chromosome (between siteIndex and start of chromosome) just move siteIndex
		} else {  // there are not enough sites in the current chromosome, must move to next chromosome
			distance = distance - siteIndex;  // reduce distance by the number of sites between siteIndex and start of Chromosome
			advanceChromosome();  // advance to the next chromosome;
			advanceIndex(distance);  // advanceIndex by remaining distance
		}

	}

}

// returns true if this Handler has reached the end of genome (or start if direction is backwards).
bool MultiGenome::Handler::atEOG() {
	modulateIndex();
	return EOG;
}

bool MultiGenome::Handler::atEOC() {
	modulateIndex();
	return EOC;
}

void MultiGenome::Handler::advanceChromosome() {
	chromosomeIndex += (readDirection) ? 1 : (-1);  //move index
	siteIndex = 0;  // set siteIndex to 0 so modulateIndex can not advance again
					// if we are reading forward, siteIndex should = 0 at this time
	modulateIndex();
	if (!readDirection) {  // if we are reading backwards, set siteIndex to the last site
		siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;
	}
	EOC = true;
}

void MultiGenome::Handler::printIndex() {
	std::string rd = (readDirection) ? "forward" : "backwards";

	std::cout << "chromosomeIndex: " << chromosomeIndex << "  siteIndex: " << siteIndex << "  EOC: " << EOC << "  EOG: " << EOG << "  direction: " << rd << std::endl;
}

int MultiGenome::Handler::readInt(int valueMin, int valueMax, int code, int CodingRegionIndex) {
	int value;
	//modulateIndex();
	if (genome->chromosomes[chromosomeIndex]->readInt(siteIndex, value, valueMin, valueMax, readDirection, code, CodingRegionIndex)) {
		advanceChromosome();
	}
	return value;
}

double MultiGenome::Handler::readDouble(double valueMin, double valueMax, int code, int CodingRegionIndex) {
	double value;
	if (genome->chromosomes[chromosomeIndex]->siteToDouble(siteIndex, value, valueMin, valueMax, readDirection, code, CodingRegionIndex)) {
		advanceChromosome();
	}
	return value;

}

void MultiGenome::Handler::writeInt(int value, int valueMin, int valueMax) {
	modulateIndex();
	if (genome->chromosomes[chromosomeIndex]->writeInt(siteIndex, value, valueMin, valueMax, readDirection)) {
		advanceChromosome();
	}
}

void MultiGenome::Handler::writeDouble(double value, double valueMin, double valueMax) {
	modulateIndex();
	if (genome->chromosomes[chromosomeIndex]->writeDouble(siteIndex, value, valueMin, valueMax, readDirection)) {
		advanceChromosome();
	}
}

std::shared_ptr<AbstractGenome::Handler> MultiGenome::Handler::makeCopy() {
	auto newGenomeHandler = std::make_shared<MultiGenome::Handler>(genome, readDirection);
	newGenomeHandler->EOG = EOG;
	newGenomeHandler->EOC = EOC;
	newGenomeHandler->siteIndex = siteIndex;
	newGenomeHandler->chromosomeIndex = chromosomeIndex;
	return(newGenomeHandler);
}

void MultiGenome::Handler::copyTo(std::shared_ptr<AbstractGenome::Handler> to) {
	auto castTo = std::dynamic_pointer_cast<MultiGenome::Handler>(to);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	castTo->readDirection = readDirection;
	castTo->genome = genome;
	castTo->chromosomeIndex = chromosomeIndex;
	castTo->siteIndex = siteIndex;
	castTo->EOG = EOG;
	castTo->EOC = EOC;
}

bool MultiGenome::Handler::inTelomere(int length) {
	modulateIndex();
	if (atEOC() || atEOG()) {
		return true;
	}
	if (readDirection) {  // if reading forward
		return (siteIndex >= (genome->chromosomes[chromosomeIndex]->size() - length));
	} else {
		return (siteIndex - 1 < length);
	}
}

void MultiGenome::Handler::randomize() {
	chromosomeIndex = Random::getIndex(genome->chromosomes.size());
	siteIndex = Random::getIndex(genome->chromosomes[chromosomeIndex]->size());
}

std::vector<std::vector<int>> MultiGenome::Handler::readTable(
    std::pair<int, int> tableSize, std::pair<int, int> tableMaxSize,
    std::pair<int, int> valueRange, int code, int CodingRegionIndex) {
        std::vector<std::vector<int>> table;
	int x = 0;
	int y = 0;
	int Y = tableSize.first;
	int X = tableSize.second;
	int maxY = tableMaxSize.first;
	int maxX = tableMaxSize.second;

	table.resize(Y);  // set the number of rows in the table

	for (; y < (Y); y++) {
		table[y].resize(X);  // set the number of columns in this row
		for (x = 0; x < X; x++) {
			//table[y][x] = (Type) (sites[index]);
			table[y][x] = readInt(valueRange.first, valueRange.second, code, CodingRegionIndex);
		}
		for (; x < maxX; x++) {
			readInt(valueRange.first, valueRange.second);  // advance genomeIndex to account for unused entries in the max sized table for this row
		}
	}
	for (; y < (maxY); y++) {
		for (x = 0; x < maxX; x++) {
			readInt(valueRange.first, valueRange.second);  // advance to account for unused rows
		}
	}
	return table;
}

// make an empty genome and ploidy = 1
MultiGenome::MultiGenome(std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_){

	//initialPloidyLPL = (PT == nullptr) ? initialGenomeSizePL : Parameters::getIntLink("GENOME_CIRCULAR-sizeInitial", PT);;
	//initialChromosomesLPL = ;
	//initialChromosomeSizeLPL = ;
	//pointMutationRateLPL = (PT == nullptr) ? pointMutationRatePL : Parameters::getDoubleLink("GENOME_MULTI-mutationPointRate", PT);
	//insertionRateLPL = (PT == nullptr) ? insertionRatePL : Parameters::getDoubleLink("GENOME_MULTI-mutationCopyRate", PT);
	//insertionMinSizeLPL = (PT == nullptr) ? insertionMinSizePL : Parameters::getIntLink("GENOME_MULTI-mutationCopyMinSize", PT);
	//insertionMaxSizeLPL = (PT == nullptr) ? insertionMaxSizePL : Parameters::getIntLink("GENOME_MULTI-mutationCopyMaxSize", PT);
	//deletionRateLPL = (PT == nullptr) ? deletionRatePL : Parameters::getDoubleLink("GENOME_MULTI-mutationDeleteRate", PT);
	//deletionMinSizeLPL = (PT == nullptr) ? deletionMinSizePL : Parameters::getIntLink("GENOME_MULTI-mutationDeleteMinSize", PT);
	//deletionMaxSizeLPL = (PT == nullptr) ? deletionMaxSizePL : Parameters::getIntLink("GENOME_MULTI-mutationDeleteMaxSize", PT);
	//minChromosomeSizeLPL = (PT == nullptr) ? minChromosomeSizePL : Parameters::getIntLink("GENOME_MULTI-chromosomeSizeMin", PT);
	//maxChromosomeSizeLPL = (PT == nullptr) ? maxChromosomeSizePL : Parameters::getIntLink("GENOME_MULTI-chromosomeSizeMax", PT);
	//crossCountLPL = (PT == nullptr) ? crossCountPL : Parameters::getIntLink("GENOME_MULTI-mutationCrossCount", PT);

	ploidy = 1;
	// define columns to be written to genome files
	genomeFileColumns.clear();
	genomeFileColumns.push_back("update");
	genomeFileColumns.push_back("ID");
	genomeFileColumns.push_back("sitesCount");
	genomeFileColumns.push_back("chromosomeCount");
	genomeFileColumns.push_back("alphabetSize");
	genomeFileColumns.push_back("ploidy");
	genomeFileColumns.push_back("chromosomeLengths");
	genomeFileColumns.push_back("sites");
	// define columns to added to ave files
	popFileColumns.clear();
	popFileColumns.push_back("genomeLength");
}

// make a genome with 1 chromosome
MultiGenome::MultiGenome(std::shared_ptr<AbstractChromosome> _chromosome, std::shared_ptr<ParametersTable> PT_) :
		MultiGenome(PT_) {
	//ploidy = 1;
	chromosomes.push_back(_chromosome->makeLike());
	/////////chromosomes[0]->fillRandom();  // resize and set with random values
	recordDataMap();
}

// make a genome with 1 or more chromosome and ploidy >= 1
MultiGenome::MultiGenome(std::shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount, int _ploidy, std::shared_ptr<ParametersTable> PT_) :
		MultiGenome(PT_) {
	ploidy = _ploidy;
	if (ploidy < 1) {
		std::cout << "Error: Genome must have plodiy >= 1";
		exit(1);
	}
	if (chromosomeCount < 1) {
		std::cout << "Error: Genome must have at least one chromosome";
		exit(1);
	}
	for (int i = 0; i < (chromosomeCount * ploidy); i++) {
		chromosomes.push_back(_chromosome->makeLike());
	}
	recordDataMap();
}

std::shared_ptr<AbstractGenome> MultiGenome::makeCopy(std::shared_ptr<ParametersTable> PT_) {
	auto newGenome = std::make_shared<MultiGenome>(PT_);
	newGenome->chromosomes.resize(0);
	for (auto chromosome : chromosomes) {
		newGenome->chromosomes.push_back(chromosome->makeCopy());
	}
	newGenome->ploidy = ploidy;
	newGenome->dataMap = dataMap; 
	return newGenome;
}

std::shared_ptr<AbstractGenome::Handler> MultiGenome::newHandler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	//cout << "In Genome::newHandler()" << endl;
	for (auto chromosome : chromosomes) {
		if (chromosome->size() == 0) {
			std::cout << "Warning! :: you are creating a grenome handler to a genome with and empty chromosome. This is not allowed!\nExiting!\n\n";
			exit(1);
		}
	}

	return std::make_shared<Handler>(_genome, _readDirection);
}

double MultiGenome::getAlphabetSize() {
	return chromosomes[0]->alphabetSize;
}

// randomize this genomes contents
void MultiGenome::fillRandom() {
	for (auto chromosome : chromosomes) {
		chromosome->fillRandom();
	}
}

// fill all sites of this genome with ascending values
// This function is to make testing easy.
void MultiGenome::fillAcending() {
	int start = 0;
	for (int i = 0; i < (int) chromosomes.size(); i++) {
		chromosomes[i]->fillAcending(start);
	}
}

// fill all sites of this genome with value
// if "acendingChromosomes" = true, then increment value after each chromosome
// This function is to make testing easy.
void MultiGenome::fillConstant(int value, bool acendingChromosomes) {
	for (int i = 0; i < (int) chromosomes.size(); i++) {
		chromosomes[i]->fillConstant(value);
		if (acendingChromosomes) {
			value++;
		}
	}
}

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
void MultiGenome::copyFrom(std::shared_ptr<AbstractGenome> from) {
	auto castFrom = std::dynamic_pointer_cast<MultiGenome>(from);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	chromosomes.resize(0);
	for (auto chromosome : castFrom->chromosomes) {
		chromosomes.push_back(chromosome->makeCopy());
	}
	ploidy = castFrom->ploidy;
}

// Mutation functions

int MultiGenome::countSites() {
	int nucleotides = 0;
	for (auto chromosome : chromosomes) {
		nucleotides += chromosome->size();
	}
	return nucleotides;
}

bool MultiGenome::isEmpty() {
	//cout << "in Genome::isEmpty(): " << to_string(countSites() == 0) << " : " << to_string(countSites()) << endl;
	return (countSites() == 0);
}

// apply mutations to this genome
void MultiGenome::mutate() {
	for (auto chromosome : chromosomes) {
		int nucleotides = chromosome->size();

		int howManyPoint = Random::getBinomial(nucleotides, pointMutationRatePL->get(PT));
		int howManyCopy = Random::getBinomial(nucleotides, insertionRatePL->get(PT));
		int howManyDelete = Random::getBinomial(nucleotides, deletionRatePL->get(PT));


		// do some point mutations
		for (int i = 0; i < howManyPoint; i++) {
			chromosome->mutatePoint();
		}
		// do some copy mutations
		int MaxChromosomeSize = maxChromosomeSizePL->get(PT);
		int IMax = insertionMaxSizePL->get(PT);
		int IMin = insertionMinSizePL->get(PT);
		//if (nucleotides < (PT == nullptr) ? *deletionRate : PT->lookupInt("GENOME_MULTI_chromosomeSizeMax")) {
		for (int i = 0; i < howManyCopy && (nucleotides < MaxChromosomeSize); i++) {
			chromosome->mutateCopy(IMin, IMax, MaxChromosomeSize);
			nucleotides = chromosome->size();
		}
		//}
		// do some deletion mutations

		int MinChromosomeSize = minChromosomeSizePL->get(PT);
		int DMax = deletionMaxSizePL->get(PT);
		int DMin = deletionMinSizePL->get(PT);

		//if (nucleotides > PT.lookup("GENOME_MULTI_chromosomeSizeMin")) {
			for (int i = 0; i < howManyDelete && (nucleotides > MinChromosomeSize); i++) {
				chromosome->mutateDelete(DMin, DMax, MinChromosomeSize);
				nucleotides = chromosome->size();
			}
		//}
	}
}

// make a mutated genome. from this genome
// inherit the ParamatersTable from the calling instance
std::shared_ptr<AbstractGenome> MultiGenome::makeMutatedGenomeFrom(std::shared_ptr<AbstractGenome> parent) {
	if (ploidy == 1) { // if single parent and single ploidy, then just copy and mutate
		auto newGenome = std::make_shared<MultiGenome>(PT);
		newGenome->copyFrom(parent);
		newGenome->mutate();
		newGenome->recordDataMap();
		return newGenome;
	}
	// otherwise, make ploidy parents and process selfing
	std::vector<std::shared_ptr<AbstractGenome>> parents;
	while (parents.size() < ploidy) {
		parents.push_back(parent);
	}
	return makeMutatedGenomeFromMany(parents);
}

// make a mutated genome from a vector or genomes
// inherit the ParamatersTable from the calling instance
// assumes all genomes have the same number of chromosomes and same ploidy
// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
std::shared_ptr<AbstractGenome> MultiGenome::makeMutatedGenomeFromMany(std::vector<std::shared_ptr<AbstractGenome>> parents) {
//	cout << "In Genome::makeMutatedGenome(vector<std::shared_ptr<AbstractGenome>> parents)\n";
	// first, check to make sure that parent genomes are conpatable.
	auto castParent0 = std::dynamic_pointer_cast<MultiGenome>(parents[0]);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	int testPloidy = castParent0->ploidy;
	size_t testChromosomeCount = castParent0->chromosomes.size();
	for (auto rawParent : parents) {
		auto parent = std::dynamic_pointer_cast<MultiGenome>(rawParent);
		if (parent->ploidy != testPloidy) {
			std::cout << "ERROR! In Genome::makeMutatedGenome(vector<std::shared_ptr<Genome>> parents). Parents are incompatible due to mismatched ploidy!\nExiting!\n";
			exit(1);
		}
		if (parent->chromosomes.size() != testChromosomeCount) {
			std::cout << "ERROR! In Genome::makeMutatedGenome(vector<std::shared_ptr<Genome>> parents). Parents are incompatible due to mismatched number of Chromosomes!\nExiting!\n";
			exit(1);
		}

	}
	auto newGenome = std::make_shared<MultiGenome>(PT);
	newGenome->ploidy = castParent0->ploidy;  // copy ploidy from 0th parent
	int crossCount = crossCountPL->get(PT);
	if (ploidy == 1) {  // if haploid then cross chromosomes from all parents
		for (size_t i = 0; i < castParent0->chromosomes.size(); i++) {
			newGenome->chromosomes.push_back(castParent0->chromosomes[0]->makeLike());
			std::vector<std::shared_ptr<AbstractChromosome>> parentChromosomes;
			for (auto rawParent : parents) {
				auto parent = std::dynamic_pointer_cast<MultiGenome>(rawParent);
				parentChromosomes.push_back(parent->chromosomes[i]);  // make a vector that contains the nth chromosome from each parent
			}
			newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, crossCount);  // create a crossover chromosome
		}
	} else if (ploidy == (int) parents.size()) {  // if multi ploid than cross chromosomes from all parents
		int setCount = castParent0->chromosomes.size() / ploidy;  // number of sets of chromosomes
		for (int currSet = 0; currSet < setCount; currSet++) {
			int parentCount = 0;
			for (auto parent : parents) {

				newGenome->chromosomes.push_back(castParent0->chromosomes[0]->makeLike());  // add a chromosome to this genome
				std::vector<std::shared_ptr<AbstractChromosome>> parentChromosomes;
				for (int pCount = 0; pCount < ploidy; pCount++) {  // make a vector containing all the chromosomes in this chromosome set from this parent
					parentChromosomes.push_back(std::dynamic_pointer_cast<MultiGenome>(parent)->chromosomes[(currSet * ploidy) + pCount]);
				}
				newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, crossCount);
				parentCount++;
			}
		}
	} else {  // ploidy does not match number of parents...
		std::cout << "In Genome::makeMutatedGenome(). ploidy > 1 and ploidy != number of parents... this is not supported (yet)!\nWhat does it even mean?\nExiting!";
		exit(1);
	}

	newGenome->mutate();
	newGenome->recordDataMap();
//	cout << "  Leaving Genome::makeMutatedGenome(vector<std::shared_ptr<AbstractGenome>> parents)\n";
	return newGenome;
}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
DataMap MultiGenome::getStats(std::string& prefix) {
	DataMap dataMap;
	dataMap.set(prefix +"genomeLength",countSites());
	return (dataMap);
}

DataMap MultiGenome::serialize(std::string& name) {
	DataMap serialDataMap;

	std::string chromosomeLengths = "";
	for (size_t c = 0; c < chromosomes.size(); c++) {
		chromosomeLengths += std::to_string(chromosomes[c]->size()) + ",";
	}
	chromosomeLengths.pop_back();
	chromosomeLengths += "";
	serialDataMap.set(name + "_chromosomeLengths", chromosomeLengths);
	serialDataMap.set(name + "_sites", genomeToStr());
	return serialDataMap;
}

// given a DataMap and PT, return genome [name] from the DataMap
void MultiGenome::deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string>& orgData, std::string& name) {
	// make sure that data has needed columns
	if (orgData.find(name + "_sites") == orgData.end() || orgData.find(name + "_chromosomeLengths") == orgData.end()) {
		std::cout << "  In MultiGenome::deserialize :: can not find either " + name + "_sites or " + name + "_chromosomeLengths in orgData (passed to function).\n  exiting" << std::endl;
		exit(1);
	}

	std::vector<int> _chromosomeLengths;
	convertCSVListToVector(orgData[name + "_chromosomeLengths"], _chromosomeLengths);
	std::string sitesType = AbstractGenome::genomeSitesTypePL->get(PT);
	std::string allSites = orgData[name + "_sites"];
	std::stringstream ss(allSites);

	char nextChar;

	for (size_t i = 0; i < _chromosomeLengths.size(); i++) {
		//cout << i << "  :  " << ss.str() << endl;
		chromosomes[i]->readChromosomeFromSS(ss, _chromosomeLengths[i]);
	}
}

/////////////// FIX FIX FIX ////////////////////

void MultiGenome::recordDataMap() {
	dataMap.merge(chromosomes[0]->getFixedStats());
	dataMap.set("ploidy", ploidy);
	dataMap.setOutputBehavior("ploidy", DataMap::FIRST);
	dataMap.set("chromosomeCount", (int)chromosomes.size());
	dataMap.setOutputBehavior("chromosomeCount", DataMap::FIRST);
	dataMap.set("sitesCount", countSites());
	dataMap.setOutputBehavior("sitesCount", DataMap::FIRST);
	dataMap.clear("chromosomeLengths");
	for (size_t c = 0; c < chromosomes.size(); c++) {
		dataMap.append("chromosomeLengths", chromosomes[c]->size());
	}
	dataMap.setOutputBehavior("chromosomeLengths", DataMap::LIST);
}

// load all genomes from a file
void MultiGenome::loadGenomeFile(std::string fileName, std::vector<std::shared_ptr<AbstractGenome>> &genomes) {
	genomes.clear();
	std::ifstream FILE(fileName);
	std::string rawLine;
	int _update, _ID, _sitesCount, _chromosomeCount, _ploidy;
	double _alphabetSize;
	std::vector<int> _chromosomeLengths;
	char rubbish;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		getline(FILE, rawLine);  // bypass first line
		while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
			std::stringstream ss(rawLine);
//				ss >> target;
//				if (ss.fail()) {
//					return false;
//				} else {
//					string remaining;
//					ss >> remaining;
//					// stream failure means nothing left in stream, which is what we want
//					return ss.fail();
//				}
			ss >> _update >> rubbish >> _ID >> rubbish >> _sitesCount >> rubbish >> _chromosomeCount >> rubbish >> _alphabetSize >> rubbish >> _ploidy >> rubbish >> rubbish >> rubbish;
			_chromosomeLengths.resize(_chromosomeCount);
			for (int i = 0; i < _chromosomeCount; i++) {
				ss >> _chromosomeLengths[i] >> rubbish;
			}
			ss >> rubbish >> rubbish >> rubbish >> rubbish;

			std::shared_ptr<MultiGenome> newGenome = std::make_shared<MultiGenome>(chromosomes[0], _chromosomeCount / _ploidy, _ploidy,PT);
			for (int i = 0; i < _chromosomeCount; i++) {
				newGenome->chromosomes[i]->readChromosomeFromSS(ss, _chromosomeLengths[i]);
			}
			newGenome->dataMap.set("update", _update);
			newGenome->dataMap.set("ID", _ID);
			newGenome->ploidy = _ploidy;
			genomes.push_back(newGenome);
		}
	} else {
		std::cout << "\n\nERROR: In MultiGenome::loadGenomeFile, unable to open file \"" << fileName << "\"\n\nExiting\n" << std::endl;
		exit(1);
	}
}
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//virtual void loadGenome(string fileName, string key, string keyValue) {
//}

// Translation functions - convert genomes into usefull stuff

// convert a chromosome to a string
std::string MultiGenome::genomeToStr() {
	std::string S = "";

	for (size_t c = 0; c < chromosomes.size(); c++) {
		S.append(chromosomes[c]->chromosomeToStr() + FileManager::separator);
	}
	S.pop_back(); // clip off the trailing separator
	S.append("");
	return S;
}

void MultiGenome::printGenome() {
	std::cout << "alphabetSize: " << getAlphabetSize() << "  chromosomes: " << chromosomes.size() << "  ploidy: " << ploidy << std::endl;
	for (size_t c = 0; c < chromosomes.size(); c++) {
		std::cout << c << " : " << chromosomes[c]->size() << " : " << chromosomes[c]->chromosomeToStr() << std::endl;
	}
}

