//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "CircularGenome.h"
#include <Global.h>
#include <cmath> // std::nextbefore
#include <cfloat> // DBL_MAX

// Initialize Parameters
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeInitialPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeInitial", 5000, "starting size for genome");
std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationPointRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationPointRate", 0.005, "per site point mutation rate");
std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationCopyRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyRate", 0.00002, "per site insertion rate");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCopyMinSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyMinSize", 128, "minimum size of insertion mutation");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCopyMaxSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyMaxSize", 512, "maximum size of insertion mutation");
std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationDeleteRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteRate", 0.00002, "per site deletion rate");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationDeleteMinSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteMinSize", 128, "minimum size of insertion mutation");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationDeleteMaxSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteMaxSize", 512, "maximum size of insertion mutation");
std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationIndelRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationIndelRate", 0.0, "per site insertion+deletion (indel) rate. This mutation copies a segment of the genome and deletes a segment of the same size so genome size remains fixed).");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationIndelMinSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationIndelMinSize", 128, "minimum size of insertion-deletion mutation");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationIndelMaxSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationIndelMaxSize", 512, "maximum size of insertion-deletion mutation");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationIndelInsertMethodPL = Parameters::register_parameter("GENOME_CIRCULAR-mutationIndelInsertMethod", 0, "where is copied material inserted?\n0 = place random, 1 = replace deleted sites, 2 = insert just before copied material");
std::shared_ptr<ParameterLink<bool>> CircularGenomeParameters::mutationIndelCopyFirstPL = Parameters::register_parameter("GENOME_CIRCULAR-mutationIndelCopyFirst", true, "whether copy or deletion happens first (0 = delete first, 1 = copy first)");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeMinPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeMin", 2000, "if genome is smaller then this, mutations will only increase chromosome size");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeMaxPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeMax", 20000, "if genome is larger then this, mutations will only decrease chromosome size");
std::shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCrossCountPL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCrossCount", 3, "number of crosses when performing crossover (including during recombination)");


std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationPointOffsetRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationPointOffsetRate", 0.0, "per site point offset mutation rate (site changes in range (+/-)mutationPointOffsetRange)");
std::shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationPointOffsetRangePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationPointOffsetRange", 1.0, "range of PointOffset mutation");
std::shared_ptr<ParameterLink<bool>> CircularGenomeParameters::mutationPointOffsetUniformPL = Parameters::register_parameter("GENOME_CIRCULAR-mutationPointOffsetUniform", true, "if true, offset will be from a uniform distribution, if false, from a normal distribution (where mean is 0 and std_dev is mutationPointOffsetRange)");


// constructor
template<class T>
CircularGenome<T>::Handler::Handler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	genome = std::dynamic_pointer_cast<CircularGenome>(_genome);
	setReadDirection(_readDirection);
	resetHandler();
}

template<class T>
void CircularGenome<T>::Handler::resetHandler() {
	if (readDirection) {  // if reading forward
		siteIndex = 0;
	} else {  // if reading backwards
		siteIndex = (int)genome->size() - 1;  // set to last site in last chromosome
	}
	resetEOG();
	resetEOC();
}

template<class T>
void CircularGenome<T>::Handler::resetHandlerOnChromosome() {
	if (readDirection) {  // if reading forward
		siteIndex = 0;
	} else {  // if reading backwards
		siteIndex = (int)genome->size() - 1;  // set to last site in last chromosome
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
template<class T>
void CircularGenome<T>::Handler::modulateIndex() {
	if (readDirection) {
		// first see if we are past last chromosome
		if (siteIndex >= (int) genome->size()) {
			siteIndex = 0;  // reset chromosomeIndex
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}
	} else {  //reading backwards!
		// first see if we are past last chromosome
		if (siteIndex < 0) {
			siteIndex = (int)genome->size() - 1;
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}
	}
}

// advanceIndex will move the index forward distance sites.
// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
// site 5 of the next chromosome. Should this be fixed?!??
template<class T>
void CircularGenome<T>::Handler::advanceIndex(int distance) {
	if (readDirection) {  // reading forward
		siteIndex += distance;  // if there are enough sites left in the current chromosome, just move siteIndex
	} else {  // reading backwards
		siteIndex -= distance;  // if there are enough sites in the current chromosome (between siteIndex and start of chromosome) just move siteIndex
	}
	modulateIndex();
}

// returns true if this Handler has reached the end of genome (or start if direction is backwards).
template<class T>
bool CircularGenome<T>::Handler::atEOG() {
	modulateIndex();
	return EOG;
}

template<class T>
bool CircularGenome<T>::Handler::atEOC() {
	modulateIndex();
	return EOC;
}

template<class T>
void CircularGenome<T>::Handler::printIndex() {
	std::string rd = (readDirection) ? "forward" : "backwards";

	std::cout << "  siteIndex: " << siteIndex << "  EOC: " << EOC << "  EOG: " << EOG << "  direction: " << rd << std::endl;
}

template<class T>
int CircularGenome<T>::Handler::readInt(int valueMin, int valueMax, int code, int CodingRegionIndex) {
	int value;
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	double currentMax = genome->alphabetSize;
	value = (int) genome->sites[siteIndex];
	//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	advanceIndex();  // EOC = end of chromosome
	while ((valueMax - valueMin + 1) > currentMax) {  // we don't have enough bits of information
		value = (value * (int)genome->alphabetSize) + (int) genome->sites[siteIndex];  // next site
		//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		advanceIndex();
		currentMax = currentMax * genome->alphabetSize;
	}
	return (value % (valueMax - valueMin + 1)) + valueMin;;
}

// when an int is read from a double chromosome, only one site is read (since it has sufficent accuracy) and then this value is
// scaled by alphabet size, then by valueMin, valueMax and then rounded.
template<>
int CircularGenome<double>::Handler::readInt(int valueMin, int valueMax, int code, int CodingRegionIndex) {
	double value;
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	valueMax += 1; // do this so that range is inclusive!
	value = genome->sites[siteIndex];
	advanceIndex();
	//cout << "  value: " << value << "  valueMin: " << valueMin << "  valueMax: " << valueMax << "  final: " << (value * ((valueMax - valueMin) / genome->alphabetSize)) + valueMin << endl;
	//cout << "  value: " << value << "  valueMin: " << valueMin << "  valueMax: " << valueMax << "  final: " << ((value / genome->alphabetSize) * (valueMax - valueMin)) + valueMin << endl;
	return (int)(((value / genome->alphabetSize) * (valueMax - valueMin)) + valueMin);
}


template<class T>
double CircularGenome<T>::Handler::readDouble(double valueMin, double valueMax, int code, int CodingRegionIndex) {
	double value;
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	value = (double) genome->sites[siteIndex];
	//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	advanceIndex();
	//scale the value
	//cout << "  value: " << value << "  valueMin: " << valueMin << "  valueMax: " << valueMax << "  final: " << (value * ((valueMax - valueMin) / genome->alphabetSize)) + valueMin << endl;
	// old version where return values did not include upper value... return ((value / genome->alphabetSize) * (valueMax - valueMin)) + valueMin;
	return (value / (genome->alphabetSize - 1.0)) * (valueMax - valueMin) + valueMin;
}

template<>
double CircularGenome<double>::Handler::readDouble(double valueMin, double valueMax, int code, int CodingRegionIndex) {
	double value;
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	value = (double)genome->sites[siteIndex];
	//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	advanceIndex();
	//scale the value
	//cout << "  value: " << value << "  valueMin: " << valueMin << "  valueMax: " << valueMax << "  final: " << (value * ((valueMax - valueMin) / genome->alphabetSize)) + valueMin << endl;
	return ((value / genome->alphabetSize) * (valueMax - valueMin)) + valueMin;
}

template<class T>
void CircularGenome<T>::Handler::writeInt(int value, int valueMin, int valueMax) {
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	std::vector<T> decomposedValue;
	int writeValueBase = valueMax - valueMin + 1;
	value = value - valueMin;
	if (writeValueBase < value) {
		std::cout << "ERROR : attempting to write value to Circular Genome. \n value is too large :: (valueMax - valueMin + 1) < value!" << std::endl;
		exit(1);
	}
	while (writeValueBase > genome->alphabetSize) {  // load value in alphabetSize chunks into decomposedValue
		decomposedValue.push_back(value % ((int) genome->alphabetSize));
		value = (int)((double)value / genome->alphabetSize);
		writeValueBase = (int)((double)writeValueBase / genome->alphabetSize);
	}
	decomposedValue.push_back(value);
	while ((int)decomposedValue.size() > 0) {  // starting with the last element in decomposedValue, copy into genome.
		genome->sites[siteIndex] = decomposedValue[(int)decomposedValue.size() - 1];
		advanceIndex();
		decomposedValue.pop_back();
	}
}

// when writing an int into a double genome, a number should be generated such that a read int will extract the same value.
// thus, value (an int) is scaled between valueMin and valueMax to a value between 0 and 1 and then scaled up by alphabet size.

template<>
void CircularGenome<double>::Handler::writeInt(int value, int valueMin, int valueMax) {
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	valueMax += 1; // do this so that range is inclusive!
	//if (value  > valueMax) {
	//	cout << "ERROR : attempting to write value to <double> Circular Genome. \n value is too large!" << endl;
	//	exit(1);
	//}
	genome->sites[siteIndex] = (((double)(value - valueMin) / (double)(valueMax - valueMin)) * genome->alphabetSize);
	advanceIndex();
}


// scale value using valueMin and valueMax to alphabetSize and write at siteIndex
// value - MIN(valueMin,valueMax) must be < ABS(valueMax - valueMin)
template<class T> 
void CircularGenome<T>::Handler::writeDouble(double value, double valueMin, double valueMax) {
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	if ((value - valueMin) > (valueMax - valueMin)) {
		std::cout << "Error: attempting to write double. given range is too small, value: " << value << " is not < valueMax: " << valueMin << " - valueMin: " << valueMin << "\n";
		exit(1);
	}
	// old version: value = ((value - valueMin) / (valueMax - valueMin)) * genome->alphabetSize;
	//std::cout << value << "   " << valueMax << "   " << valueMin << " = ";
	value = ((value - valueMin) / (valueMax - valueMin)) * (genome->alphabetSize - 1.0);
	//std::cout << value << std::endl;
	genome->sites[siteIndex] = (T)value;
	advanceIndex();
}

template<>
void CircularGenome<double>::Handler::writeDouble(double value, double valueMin, double valueMax) {
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	if ((value - valueMin) > (valueMax - valueMin)) {
		std::cout << "Error: attempting to write double. given range is too small, value: " << value << " is not < valueMax: " << valueMin << " - valueMin: " << valueMin << "\n";
		exit(1);
	}
	value = ((value - valueMin) / (valueMax - valueMin)) * genome->alphabetSize;
	genome->sites[siteIndex] = value;
	advanceIndex();
}


template<class T>
std::shared_ptr<AbstractGenome::Handler> CircularGenome<T>::Handler::makeCopy() {
	auto newGenomeHandler = std::make_shared<CircularGenome<T>::Handler>(genome, readDirection);
	newGenomeHandler->EOG = EOG;
	newGenomeHandler->EOC = EOC;
	newGenomeHandler->siteIndex = siteIndex;
	return(newGenomeHandler);
}

template<class T>
void CircularGenome<T>::Handler::copyTo(std::shared_ptr<AbstractGenome::Handler> to) {
	auto castTo = std::dynamic_pointer_cast<CircularGenome<T>::Handler>(to);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	castTo->readDirection = readDirection;
	castTo->genome = genome;
	castTo->siteIndex = siteIndex;
	castTo->EOG = EOG;
}

template<class T>
bool CircularGenome<T>::Handler::inTelomere(int length) {
	modulateIndex();
	if (atEOG()) {
		return true;
	}
	if (readDirection) {  // if reading forward
		return (siteIndex >= ((int)genome->size() - length));
	} else {
		return (siteIndex - 1 < length);
	}
}

template<class T>
void CircularGenome<T>::Handler::randomize() {
	siteIndex = Random::getIndex((int)genome->size());
}

template<class T>
std::vector<std::vector<int>> CircularGenome<T>::Handler::readTable(std::pair<int, int> tableSize, std::pair<int, int> tableMaxSize, std::pair<int, int> valueRange, int code, int CodingRegionIndex) {
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

template<class T>
void CircularGenome<T>::setupCircularGenome(int _size, double _alphabetSize) {
	sites.resize(_size);
	alphabetSize = _alphabetSize;
	// define columns to be written to genome files
	genomeFileColumns.clear();
	genomeFileColumns.push_back("update");
	genomeFileColumns.push_back("ID");
	genomeFileColumns.push_back("alphabetSize");
	genomeFileColumns.push_back("genomeLength");
	genomeFileColumns.push_back("sites");
    
	genomeFileColumns.push_back("countCopy");
    genomeFileColumns.push_back("countDelete");
	genomeFileColumns.push_back("countPoint");
	genomeFileColumns.push_back("countPointOffset");
	genomeFileColumns.push_back("countIndel");
	
	// define columns to added to ave files
	popFileColumns.clear();
	popFileColumns.push_back("genomeLength");
    
	popFileColumns.push_back("countCopy");
    popFileColumns.push_back("countDelete");
	popFileColumns.push_back("countPoint");
	popFileColumns.push_back("countPointOffset");
	popFileColumns.push_back("countIndel");

	recordDataMap();
}

//template<class T>
//CircularGenome<T>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_){
//	setupCircularGenome(_size, _alphabetSize);
//}

template<class T>
CircularGenome<T>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
	setupCircularGenome(_size, _alphabetSize);
	std::cout << "ERROR : TYPE specified for CircularGenome is not supported.\nTypes supported are: int, double, bool, unsigned char" << std::endl;
	exit(1);
}

template<>
CircularGenome<int>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<bool>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
	if (_alphabetSize != 2) {
		std::cout << "ERROR: in CircularGenome constructor, alphabetSize for bool must be 2!" << std::endl;
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<double>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<unsigned char>::CircularGenome(double _alphabetSize, int _size, std::shared_ptr<ParametersTable> PT_) : AbstractGenome(PT_) {
	if (_alphabetSize > 256 || _alphabetSize < 2) {
		std::cout << "ERROR: in CircularGenome constructor, alphabetSize for unsigned char must be 2 or greater and 256 or less!" << std::endl;
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}

template<class T>
std::shared_ptr<AbstractGenome> CircularGenome<T>::makeCopy(std::shared_ptr<ParametersTable> PT_) {
	if (PT_ == nullptr) {
		PT_ = PT;
	}

	auto newGenome = std::make_shared<CircularGenome>(alphabetSize, 1, PT_);

	newGenome->sites = sites; 
	newGenome->countPoint = countPoint;
	newGenome->countPointOffset = countPointOffset;
	newGenome->countDelete = countDelete;
	newGenome->countCopy = countCopy;
	newGenome->countIndel = countIndel;

	return newGenome;
}



template<class T>
std::shared_ptr<AbstractGenome::Handler> CircularGenome<T>::newHandler(std::shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	//cout << "In Genome::newHandler()" << endl;
	return std::make_shared<Handler>(_genome, _readDirection);
}

template<class T> int CircularGenome<T>::size() {
	return (int) sites.size();
}

template<class T>
double CircularGenome<T>::getAlphabetSize() {
	return alphabetSize;
}

// randomize this genomes contents
template<class T>
void CircularGenome<T>::fillRandom() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = (T) Random::getDouble(alphabetSize);
	}
}

template<> inline void CircularGenome<double>::fillRandom() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = Random::getDouble(0, alphabetSize);
	}
}

template<> inline void CircularGenome<bool>::fillRandom() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = (bool)((int)Random::getDouble(alphabetSize));
	}
}

// fill all sites of this genome with ascending values
// This function is to make testing easy.
template<class T>
void CircularGenome<T>::fillAcending() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = ((int)i) % (int) alphabetSize;
	}
}

// fill all sites of this genome with value
// if "acendingChromosomes" = true, then increment value after each chromosome
// This function is to make testing easy.
template<class T>
void CircularGenome<T>::fillConstant(int value) {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = value;
	}
}

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
template<class T>
void CircularGenome<T>::copyFrom(std::shared_ptr<AbstractGenome> from) {
	auto castFrom = std::dynamic_pointer_cast<CircularGenome<T>>(from);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	alphabetSize = castFrom->alphabetSize;
	sites.clear();
	for (auto site : castFrom->sites) {
		sites.push_back(site);
	}
	countPoint = castFrom->countPoint;
	countPointOffset = castFrom->countPointOffset;
	countDelete = castFrom->countDelete;
	countCopy = castFrom->countCopy;
	countIndel = castFrom->countIndel;
}

// Mutation functions

template<class T>
int CircularGenome<T>::countSites() {
	return (int)sites.size();
}

template<class T>
bool CircularGenome<T>::isEmpty() {
	//cout << "in Genome::isEmpty(): " << to_string(countSites() == 0) << " : " << to_string(countSites()) << endl;
	return (countSites() == 0);
}

//template<class T>
//void CircularGenome<T>::pointMutate() {
//        
//        auto newVal = Random::getIndex((int)std::ceil((mutationPointWindowPL->get(PT))*alphabetSize));
//	sites[Random::getIndex((int)sites.size())] = newVal;
//}

// mutate a random site to a new value.
// if range is provied (not -1) then the new value will be within +/- range of current value
template<class T>
void CircularGenome<T>::pointMutate(double range) {
	if (range == -1) {
		sites[Random::getIndex((int)sites.size())] = Random::getIndex((int)alphabetSize);
	}
	else {
		int siteIndex = Random::getIndex((int)sites.size());
		int offsetValue;
		if (CircularGenomeParameters::mutationPointOffsetUniformPL->get(PT) == true) {
			offsetValue = Random::getInt(1, range) * ((Random::getIndex(2) * 2.0) - 1.0);
			// note! if range < 1 then all offsetValues will be 0, if range >= 1 offsetValues
			// will always be either >= 1 or <= -1
		}
		else { //normal/gaussian
			offsetValue = (int)Random::getNormal(0, range);
		}
		sites[siteIndex] = std::max(0, std::min((int)alphabetSize - 1, sites[siteIndex] + offsetValue));
	}
}

template<>
void CircularGenome<double>::pointMutate(double range) {
	if (range == -1) {
		sites[Random::getIndex((int)sites.size())] = Random::getDouble(alphabetSize);
	}
	else {
		int siteIndex = Random::getIndex((int)sites.size());
		bool pointOffsetUniform = false;
		double offsetValue;
		if (CircularGenomeParameters::mutationPointOffsetUniformPL->get(PT) == true) {
			offsetValue = Random::getDouble(-range, range);
		}
		else { //normal/gaussian
			offsetValue = Random::getNormal(0, range);
		}
		double maxValue = alphabetSize - (std::nextafter(alphabetSize, DBL_MAX) - alphabetSize); // next smallest double value for alphabetSize
		sites[siteIndex] = std::max(0.0, std::min(maxValue, sites[siteIndex] + (offsetValue)));
	}
}

template<class T>
int CircularGenome<T>::incrementCopy() {
    return countCopy++;
}

template<class T>
int CircularGenome<T>::incrementPoint() {
	return countPoint++;
}

template<class T>
int CircularGenome<T>::incrementPointOffset() {
	return countPointOffset++;
}

template<class T>
int CircularGenome<T>::incrementDelete() {
    return countDelete++;
}

template<class T>
int CircularGenome<T>::incrementIndel() {
    return countIndel++;
}

// apply mutations to this genome
template<class T>
void CircularGenome<T>::mutate() {
	int howManyPoint = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationPointRatePL->get(PT));
	int howManyPointOffset = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationPointOffsetRatePL->get(PT));
	int howManyCopy = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationCopyRatePL->get(PT));
	int howManyDelete = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationDeleteRatePL->get(PT));
	int howManyIndel = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationIndelRatePL->get(PT));
	// do some point mutations
	for (int i = 0; i < howManyPoint; i++) {
		pointMutate();
		incrementPoint();
	}
	// do some pointOffset mutations
	double pointOffsetRange = CircularGenomeParameters::mutationPointOffsetRangePL->get(PT);
	for (int i = 0; i < howManyPointOffset; i++) {
		pointMutate(pointOffsetRange);
		incrementPointOffset();
	}
	// do some copy mutations
	int MaxGenomeSize = CircularGenomeParameters::sizeMaxPL->get(PT);
	int IMax = CircularGenomeParameters::mutationCopyMaxSizePL->get(PT);
	int IMin = CircularGenomeParameters::mutationCopyMinSizePL->get(PT);
	for (int i = 0; (i < howManyCopy) && (((int)sites.size()) < MaxGenomeSize); i++) {
		//chromosome->mutateCopy(PT.lookup("mutationCopyMinSize"), PT.lookup("mutationCopyMaxSize"), PT.lookup("chromosomeSizeMax"));

		//cout << "size: " << sites.size() << "->";
		////std::shared_ptr<Chromosome<T>> segment = dynamic_pointer_cast<Chromosome<T>>(getSegment(minSize, maxSize));

		int segmentSize = Random::getInt(IMax - IMin) + IMin;
		if (segmentSize > (int)sites.size()) {
			std::cout << "segmentSize = " << segmentSize << "  sites.size() = " << (int)sites.size() << std::endl;
			std::cout << "maxSize:minSize" << IMax << ":" << IMin << std::endl;
			std::cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for insert is > then sites.size()!\nExitting!" << std::endl;
			exit(1);
		}
		int segmentStart = Random::getInt((int)sites.size() - segmentSize);
		std::vector<T> segment;
		segment.clear();
		auto it = sites.begin();
		segment.insert(segment.begin(), it + segmentStart, it + segmentStart + segmentSize);

		////insertSegment(segment);
		it = sites.begin();
		sites.insert(it + Random::getInt((int)sites.size()), segment.begin(), segment.end());

		//cout << sites.size() << endl;

		incrementCopy();
	}
	// do some deletion mutations
	int MinGenomeSize = CircularGenomeParameters::sizeMinPL->get(PT);
	int DMax = CircularGenomeParameters::mutationDeleteMaxSizePL->get(PT);
	int DMin = CircularGenomeParameters::mutationDeleteMinSizePL->get(PT);
	for (int i = 0; (i < howManyDelete) && (((int)sites.size()) > MinGenomeSize); i++) {
		//chromosome->mutateDelete(PT.lookup("mutationDeletionMinSize"), PT.lookup("mutationDeletionMaxSize"), PT.lookup("chromosomeSizeMin"));

		int segmentSize = Random::getInt(DMax - DMin) + DMin;
		//cout << "segSize: " << segmentSize << "\tsize: " << sites.size() << "\t->\t";
		if (segmentSize > (int)sites.size()) {
			std::cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << std::endl;
			std::cout << "maxSize : minSize   " << DMax << " : " << DMin << std::endl;
			std::cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for delete is > then sites.size()!\nExitting!" << std::endl;
			exit(1);
		}
		int segmentStart = Random::getInt(((int)sites.size()) - segmentSize);
		sites.erase(sites.begin() + segmentStart, sites.begin() + segmentStart + segmentSize);

		incrementDelete();
	}
	// do some combination insertion-deletion (indel) mutations
	int IDMax = CircularGenomeParameters::mutationIndelMaxSizePL->get(PT);
	int IDMin = CircularGenomeParameters::mutationIndelMinSizePL->get(PT);
	bool copyFirst = CircularGenomeParameters::mutationIndelCopyFirstPL->get(PT);
	int insertMethod = CircularGenomeParameters::mutationIndelInsertMethodPL->get(PT);

	for (int i = 0; i < howManyIndel; i++) {

		int segmentSize = Random::getInt(IDMin, IDMax);
		if (segmentSize > (int)sites.size()) {
			std::cout << "segmentSize = " << segmentSize << "  sites.size() = " << (int)sites.size() << std::endl;
			std::cout << "maxSize:minSize" << IDMax << ":" << IDMin << std::endl;
			std::cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for indel is > then sites.size()!\nExiting!" << std::endl;
			exit(1);
		}

		// create a new genome segment
		std::vector<T> segment;
		auto it = sites.begin(); // i.e. iterator

		if (copyFirst) {
			// if copy before delete
			// copy a portion of the genome into segment
			int segmentStart = Random::getInt((int)sites.size() - segmentSize); // where to copy from
			int deleteStart = Random::getInt((int)sites.size() - segmentSize); // where to delete from
			segment.clear();
			segment.insert(segment.begin(), it + segmentStart, it + segmentStart + segmentSize);

/*
            std::cout << "\ncopyFirst\ngenome: ";
			for (auto s : sites) {
				std::cout << s << " ";
			}
			std::cout << "   size: " << segmentSize << "    segmentStart: " << segmentStart << "    deleteStart: " << deleteStart << std::endl;
			std::cout << std::endl << "segment: ";
			for (auto s : segment) {
				std::cout << s << " ";
			}
			std::cout << std::endl;
*/

			// delete a portion of the genome of the same size
			sites.erase(it + deleteStart, it + deleteStart + segmentSize);

/*
			std::cout << "\ngenome after delete: ";
			for (auto s : sites) {
				std::cout << s << " ";
			}
*/
			// insert the copied sites back into genome
			if (insertMethod == 0) {
				// copy to random location
				sites.insert(it + Random::getInt((int)sites.size()), segment.begin(), segment.end());
			}
			else if (insertMethod == 1) {
				// replace deleted segment
				sites.insert(it + deleteStart, segment.begin(), segment.end());
			}
			else if (insertMethod == 2) {
				// insert segment just in front of copied sites
				if (segmentStart > deleteStart) { // note if deleteStart is in copied segment things are weird.
					segmentStart -= deleteStart;  // but no matter what we do, it's going to be weird...
				}
				sites.insert(it + segmentStart, segment.begin(), segment.end());
			}
/*
			std::cout << "\ngenome after insert: ";
			for (auto s : sites) {
				std::cout << s << " ";
			}
*/
		}
		else {
			// delete before copy (deleted sites cannot be copied)
			// delete a portion of the genome
			int deleteStart = Random::getInt((int)sites.size() - segmentSize); // where to delete from
			sites.erase(it + deleteStart, it + deleteStart + segmentSize);

            if (segmentSize > sites.size()){
                std::cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for indel is > then sites.size() after deletion!\nUse a larger genome relitive to Indel min/max.\nExiting!" << std::endl;
                std::cout << "segmentSize = " << segmentSize << "  sites.size() after indel delete = " << (int)sites.size() << std::endl;
                exit(1);
            }
			// copy a portion of the genome into segment
			int segmentStart = Random::getInt((int)sites.size() - segmentSize);
			segment.clear();
			segment.insert(segment.begin(), it + segmentStart, it + segmentStart + segmentSize);

			// insert the copied sites back into genome
			if (insertMethod == 0) {
				// copy to random location
				sites.insert(it + Random::getInt((int)sites.size()), segment.begin(), segment.end());
			}
			else if (insertMethod == 1) {
				// replace deleted segment
				sites.insert(it + deleteStart, segment.begin(), segment.end());
			}
			else if (insertMethod == 2) {
				// insert segment just in front of copied sites
				sites.insert(it + segmentStart, segment.begin(), segment.end());
			}
		}
		incrementIndel();
	}
}

// make a mutated genome. from this genome
// inherit the ParamatersTable from the calling instance
template<class T>
std::shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFrom(std::shared_ptr<AbstractGenome> parent) {
	auto newGenome = std::make_shared<CircularGenome<T>>(PT);
	newGenome->copyFrom(parent);
    newGenome->mutate();
	newGenome->recordDataMap();
	return newGenome;
}

// make a mutated genome from a vector or genomes
// inherit the ParamatersTable from the calling instance
// assumes all genomes have the same numbe000000r of chromosomes and same ploidy
// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
template<class T>
std::shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFromMany(std::vector<std::shared_ptr<AbstractGenome>> parents) {
	//cout << "In Genome::makeMutatedGenome(std::vector<std::shared_ptr<AbstractGenome>> parents)" << endl;
	// first, check to make sure that parent genomes are conpatable.
	auto castParent0 = std::dynamic_pointer_cast<CircularGenome<T>>(parents[0]);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.

	auto newGenome = std::make_shared<CircularGenome<T>>(castParent0->alphabetSize,0,PT);
	//newGenome->alphabetSize = castParent0->alphabetSize;

//	vector<std::shared_ptr<AbstractChromosome>> parentChromosomes;
//	for (auto rawParent : parents) {
//		auto parent = dynamic_pointer_cast<CircularGenome<T>>(rawParent);
//		parentChromosomes.push_back(parent->chromosomes[i]);  // make a vector that contains the nth chromosome from each parent
//	}
//	newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, newGenome->PT.lookup("genomecrossCount"));  // create a crossover chromosome

	if (parents.size() == 0) {
		std::cout << "ERROR! in CircularGenome<T>::makeMutatedGenomeFromMany crossover... attempt to cross 0 parents\nExiting!" << std::endl;
		exit(1);
	}
	if (parents.size() == 1) {  // if there is only one parent...
		//cout << "one parent" << endl;
		return castParent0->makeMutatedGenomeFrom(castParent0);
	} else {
		//cout << "many parent" << endl;

		// extract the sites list from each parent
		std::vector<std::vector<T>> parentSites;
		for (auto parent : parents) {
			parentSites.push_back(std::dynamic_pointer_cast<CircularGenome<T>>(parent)->sites);
		}

		// randomly determine crossCount number crossLocations
		std::vector<double> crossLocations;
		int crossCount = CircularGenomeParameters::mutationCrossCountPL->get(PT);
		for (int i = 0; i < crossCount; i++) {  // get some cross locations (% of length of chromosome)
			crossLocations.push_back(Random::getDouble(1.0));
		}

		crossLocations.push_back(0.0);  // add start and end
		crossLocations.push_back(1.0);

		sort(crossLocations.begin(), crossLocations.end());  // sort crossLocations

		//cout << "crossing: " << flush;
		//for (auto location:crossLocations){
		//	cout << location << "  " << flush;
		//}
		//cout << "size init: " << size() << " -> " << flush;

		int pick;
		int lastPick = Random::getIndex((int)parents.size());
		newGenome->sites.clear();
		for (int c = 0; c < ((int)crossLocations.size()) - 1; c++) {
			// pick a chromosome to cross with. Make sure it's not the same chromosome!
			pick = Random::getIndex(((int)parents.size()) - 1);
			if (pick == lastPick) {
				pick++;
			}
			lastPick = pick;
			// add the segment to this chromosome
			//cout << "(" << parentSites[pick].size() << ") "<< c << ": " << (int)((double)parentSites[pick].size()*crossLocations[c]) << " " << (int)((double)parentSites[pick].size()*crossLocations[c+1]) << " " << flush;
			newGenome->sites.insert(newGenome->sites.end(), parentSites[pick].begin() + (int) ((double) parentSites[pick].size() * crossLocations[c]), parentSites[pick].begin() + (int) ((double) parentSites[pick].size() * crossLocations[c + 1]));
			//cout << " ++ " << flush;
		}
	}
	newGenome->mutate();
	newGenome->recordDataMap();
	//cout << "  Leaving Genome::makeMutatedGenome(vector<std::shared_ptr<AbstractGenome>> parents)\n" << flush;
	return newGenome;
}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector

template<class T>
DataMap CircularGenome<T>::getStats(std::string& prefix) {
	DataMap dataMap;
	dataMap.set(prefix + "genomeLength", countSites());
	dataMap.set(prefix + "countPoint", countPoint);
	dataMap.set(prefix + "countPointOffset", countPointOffset);
	dataMap.set(prefix + "countCopy", countCopy);
    dataMap.set(prefix + "countDelete", countDelete);
    dataMap.set(prefix + "countIndel", countIndel);
	return (dataMap);
}


template<class T>
DataMap CircularGenome<T>::serialize(std::string& name) {
	DataMap serialDataMap;
	serialDataMap.set(name + "_genomeLength", countSites());
	serialDataMap.set(name + "_sites", genomeToStr());
	return serialDataMap;
}

// given a DataMap and PT, return genome [name] from the DataMap
template<class T>
void CircularGenome<T>::deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string>& orgData, std::string& name) {
	char nextChar;
	std::string nextString;
	T value;
	// make sure that data has needed columns
	if (orgData.find(name + "_sites") == orgData.end() || orgData.find(name + "_genomeLength") == orgData.end()) {
		std::cout << "  In CircularGenome<T>::deserialize :: can not find either " + name + "_sites or " + name + "_genomeLength.\n  exiting" << std::endl;
		exit(1);
	}
	int genomeLength;
	convertString(orgData[name + "_genomeLength"], genomeLength);

	std::string allSites = orgData[name + "_sites"];
	std::stringstream ss(allSites);

  bool streamNotEmpty(true);
	sites.clear();
  streamNotEmpty = static_cast<bool>(ss >> nextChar);
	for (int i = 0; i < genomeLength; i++) {
		nextString = "";
		while (streamNotEmpty && (nextChar != ',')) {
			nextString += nextChar;
      streamNotEmpty = static_cast<bool>(ss >> nextChar);
		}
		convertString(nextString, value);
		//std::cout << nextString << " = " << value << ", ";
		sites.push_back(value);
    streamNotEmpty = static_cast<bool>(ss >> nextChar);
	}
	//std::cout << std::endl;
}

template<>
void CircularGenome<unsigned char>::deserialize(std::shared_ptr<ParametersTable> PT, std::unordered_map<std::string, std::string>& orgData, std::string& name) {
	char nextChar;
	std::string nextString;
	int value;
	// make sure that data has needed columns
	std::cout << "name: " << name << "  " << name + "_sites" << std::endl;
	if (orgData.find(name + "_sites") == orgData.end() || orgData.find(name + "_genomeLength") == orgData.end()) {
		std::cout << "  In CircularGenome<T>::deserialize :: can not find either " + name + "_sites or " + name + "_genomeLength.\n  exiting" << std::endl;
		exit(1);
	}
	int genomeLength;
	convertString(orgData[name + "_genomeLength"], genomeLength);

	std::string allSites = orgData[name + "_sites"];
	std::stringstream ss(allSites);

	sites.clear();
  bool streamNotEmpty(true);
  streamNotEmpty = static_cast<bool>(ss >> nextChar);
	for (int i = 0; i < genomeLength; i++) {
		nextString = "";
		while (streamNotEmpty && (nextChar != ',')) {
			nextString += nextChar;
            streamNotEmpty = static_cast<bool>(ss >> nextChar);
		}
		convertString(nextString, value);
        //std::cout << nextString << " = " << value << ", ";
		sites.push_back((unsigned char)value);
		ss >> nextChar;
	}
  //std::cout << std::endl;
}



template<class T>
void CircularGenome<T>::recordDataMap() {
	dataMap.set("alphabetSize", alphabetSize);
	dataMap.set("genomeLength", countSites());
	dataMap.set("countPoint", countPoint);
	dataMap.set("countPointOffset", countPointOffset);
	dataMap.set("countCopy", countCopy);
    dataMap.set("countDelete", countDelete);
    dataMap.set("countIndel", countIndel);
}
/*
// load all genomes from a file
template<class T>
void CircularGenome<T>::loadGenomeFile(string fileName, vector<std::shared_ptr<AbstractGenome>> &genomes) {
	genomes.clear();
	std::ifstream FILE(fileName);
	string rawLine;
	int _update, _ID, _genomeLength;
	double _alphabetSize;
	double value;
	char rubbish;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		getline(FILE, rawLine);  // bypass first line
		while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
			std::stringstream ss(rawLine);
			ss >> _update >> rubbish >> _ID >> rubbish >> _alphabetSize >> rubbish >> _genomeLength >> rubbish >> rubbish >> rubbish;

			std::shared_ptr<CircularGenome<T>> newGenome = make_shared<CircularGenome<T>>(PT);
			newGenome->alphabetSize = _alphabetSize;
			newGenome->sites.clear();
			for (int i = 0; i < _genomeLength; i++) {
				ss >> value >> rubbish;
				newGenome->sites.push_back((T)value);
			}
			newGenome->dataMap.Set("update", _update);
			newGenome->dataMap.Set("ID", _ID);
			genomes.push_back(newGenome);
		}
	} else {
		cout << "\n\nERROR: In CircularGenome::loadGenomeFile, unable to open file \"" << fileName << "\"\n\nExiting." << endl;
		exit(1);
	}
}

template<>
void CircularGenome<unsigned char>::loadGenomeFile(string fileName, vector<std::shared_ptr<AbstractGenome>> &genomes) {
	genomes.clear();
	std::ifstream FILE(fileName);
	string rawLine;
	int _update, _ID, _genomeLength;
	double _alphabetSize;
	double value;
	char rubbish;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		getline(FILE, rawLine);  // bypass first line
		while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
			std::stringstream ss(rawLine);
			ss >> _update >> rubbish >> _ID >> rubbish >> _alphabetSize >> rubbish >> _genomeLength >> rubbish >> rubbish >> rubbish;

			std::shared_ptr<CircularGenome<unsigned char>> newGenome = make_shared<CircularGenome<unsigned char>>(PT);
			newGenome->alphabetSize = _alphabetSize;
			newGenome->sites.clear();
			for (int i = 0; i < _genomeLength; i++) {
				ss >> value >> rubbish;
				newGenome->sites.push_back((unsigned char)value);
			}
			newGenome->dataMap.Set("update", _update);
			newGenome->dataMap.Set("ID", _ID);
			genomes.push_back(newGenome);
		}
	}
	else {
		cout << "\n\nERROR: In CircularGenome::loadGenomeFile<unsigned char>, unable to open file \"" << fileName << "\"\n\nExiting." << endl;
		exit(1);
	}
}
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//virtual void loadGenome(string fileName, string key, string keyValue) {
//}
*/
// Translation functions - convert genomes into usefull stuff

// convert a chromosome to a string
template<class T>
std::string CircularGenome<T>::genomeToStr() {
	std::stringstream ss;
	ss << "";

	for (size_t i = 0; i < sites.size()-1; i++) {
		ss << sites[i] << FileManager::separator;
	}
	ss << sites[sites.size() - 1];
	return ss.str();
}

template<>
std::string CircularGenome<unsigned char>::genomeToStr() {
	std::stringstream ss;
	ss << "";

	for (size_t i = 0; i < sites.size() - 1; i++) {
		ss << (int)sites[i] << FileManager::separator;
	}
	ss << (int)sites[sites.size() - 1] << "";
	return ss.str();
}




template<class T>
void CircularGenome<T>::printGenome() {
	std::cout << "alphabetSize: " << getAlphabetSize() << std::endl;
	std::cout << genomeToStr();
	std::cout << std::endl;
}

