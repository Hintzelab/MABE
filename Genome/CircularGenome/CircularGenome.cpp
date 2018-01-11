//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include "../../Global.h"
#include "CircularGenome.h"

// Initialize Parameters
shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeInitialPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeInitial", 5000, "starting size for genome");
shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationPointRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationPointRate", 0.005, "per site point mutation rate");
shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationCopyRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyRate", 0.00002, "per site insertion rate");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCopyMinSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyMinSize", 128, "minimum size of insertion mutation");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCopyMaxSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCopyMaxSize", 512, "maximum size of insertion mutation");
shared_ptr<ParameterLink<double>> CircularGenomeParameters::mutationDeleteRatePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteRate", 0.00002, "per site deletion rate");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationDeleteMinSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteMinSize", 128, "minimum size of insertion mutation");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationDeleteMaxSizePL = Parameters::register_parameter("GENOME_CIRCULAR-mutationDeleteMaxSize", 512, "maximum size of insertion mutation");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeMinPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeMin", 2000, "if genome is smaller then this, mutations will only increase chromosome size");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::sizeMaxPL = Parameters::register_parameter("GENOME_CIRCULAR-sizeMax", 20000, "if genome is larger then this, mutations will only decrease chromosome size");
shared_ptr<ParameterLink<int>> CircularGenomeParameters::mutationCrossCountPL = Parameters::register_parameter("GENOME_CIRCULAR-mutationCrossCount", 3, "number of crosses when performing crossover (including during recombination)");

// constructor
template<class T>
CircularGenome<T>::Handler::Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	genome = dynamic_pointer_cast<CircularGenome>(_genome);
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
// it uses readDirection to resolve them.	virtual void copyFrom(shared_ptr<Genome> from) {

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
	string rd = (readDirection) ? "forward" : "backwards";

	cout << "  siteIndex: " << siteIndex << "  EOC: " << EOC << "  EOG: " << EOG << "  direction: " << rd << endl;
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
	return ((value / genome->alphabetSize) * (valueMax - valueMin)) + valueMin;
}

template<class T>
void CircularGenome<T>::Handler::writeInt(int value, int valueMin, int valueMax) {
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	vector<T> decomposedValue;
	int writeValueBase = valueMax - valueMin + 1;
	value = value - valueMin;
	if (writeValueBase < value) {
		cout << "ERROR : attempting to write value to Circular Genome. \n value is too large :: (valueMax - valueMin + 1) < value!" << endl;
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
		cout << "Error: attempting to write double. given range is too small, value: " << value << " is not < valueMax: " << valueMin << " - valueMin: " << valueMin << "\n";
		exit(1);
	}
	value = ((value - valueMin) / (valueMax - valueMin)) * genome->alphabetSize;
	genome->sites[siteIndex] = (T)value;
	advanceIndex();
}



template<class T>
shared_ptr<AbstractGenome::Handler> CircularGenome<T>::Handler::makeCopy() {
	auto newGenomeHandler = make_shared<CircularGenome<T>::Handler>(genome, readDirection);
	newGenomeHandler->EOG = EOG;
	newGenomeHandler->EOC = EOC;
	newGenomeHandler->siteIndex = siteIndex;
	return(newGenomeHandler);
}

template<class T>
void CircularGenome<T>::Handler::copyTo(shared_ptr<AbstractGenome::Handler> to) {
	auto castTo = dynamic_pointer_cast<CircularGenome<T>::Handler>(to);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
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
vector<vector<int>> CircularGenome<T>::Handler::readTable(pair<int, int> tableSize, pair<int, int> tableMaxSize, pair<int, int> valueRange, int code, int CodingRegionIndex) {
	vector<vector<int>> table;
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
	//initialSizeLPL = (PT == nullptr) ? CircularGenomeParameters::sizeInitialPL : Parameters::getIntLink("GENOME_CIRCULAR-sizeInitial", PT);
	//mutationPointRateLPL = (PT == nullptr) ? CircularGenomeParameters::mutationPointRatePL : Parameters::getDoubleLink("GENOME_CIRCULAR-mutationPointRate", PT);
	//mutationCopyRateLPL = (PT == nullptr) ? CircularGenomeParameters::mutationCopyRatePL : Parameters::getDoubleLink("GENOME_CIRCULAR-mutationCopyRate", PT);
	//mutationCopyMinSizeLPL = (PT == nullptr) ? CircularGenomeParameters::mutationCopyMinSizePL : Parameters::getIntLink("GENOME_CIRCULAR-mutationCopyMinSize", PT);
	//mutationCopyMaxSizeLPL = (PT == nullptr) ? CircularGenomeParameters::mutationCopyMaxSizePL : Parameters::getIntLink("GENOME_CIRCULAR-mutationCopyMaxSize", PT);
	//mutationDeleteRateLPL = (PT == nullptr) ? CircularGenomeParameters::mutationDeleteRatePL : Parameters::getDoubleLink("GENOME_CIRCULAR-mutationDeleteRate", PT);
	//mutationDeleteMinSizeLPL = (PT == nullptr) ? CircularGenomeParameters::mutationDeleteMinSizePL : Parameters::getIntLink("GENOME_CIRCULAR-mutationDeleteMinSize", PT);
	//mutationDeleteMaxSizeLPL = (PT == nullptr) ? CircularGenomeParameters::mutationDeleteMaxSizePL : Parameters::getIntLink("GENOME_CIRCULAR-mutationDeleteMaxSize", PT);
	//sizeMinLPL = (PT == nullptr) ? CircularGenomeParameters::sizeMinPL : Parameters::getIntLink("GENOME_CIRCULAR-sizeMin", PT);
	//sizeMaxLPL = (PT == nullptr) ? CircularGenomeParameters::sizeMaxPL : Parameters::getIntLink("GENOME_CIRCULAR-sizeMax", PT);
	//mutationCrossCountLPL = (PT == nullptr) ? CircularGenomeParameters::mutationCrossCountPL : Parameters::getIntLink("GENOME_CIRCULAR-mutationCrossCount", PT);

	sites.resize(_size);
	alphabetSize = _alphabetSize;
	// define columns to be written to genome files
	genomeFileColumns.clear();
	genomeFileColumns.push_back("update");
	genomeFileColumns.push_back("ID");
	genomeFileColumns.push_back("alphabetSize");
	genomeFileColumns.push_back("genomeLength");
	genomeFileColumns.push_back("sites");
	// define columns to added to ave files
	popFileColumns.clear();
	popFileColumns.push_back("genomeLength");

	recordDataMap();
}

//template<class T>
//CircularGenome<T>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT){
//	setupCircularGenome(_size, _alphabetSize);
//}

template<class T>
CircularGenome<T>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT) {
	setupCircularGenome(_size, _alphabetSize);
	cout << "ERROR : TYPE specified for CircularGenome is not supported.\nTypes supported are: int, double, bool, unsigned char" << endl;
	exit(1);
}

template<>
CircularGenome<int>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<bool>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT) {
	if (_alphabetSize != 2) {
		cout << "ERROR: in CircularGenome constructor, alphabetSize for bool must be 2!" << endl;
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<double>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<unsigned char>::CircularGenome(double _alphabetSize, int _size, shared_ptr<ParametersTable> _PT) : AbstractGenome(_PT) {
	if (_alphabetSize > 256 || _alphabetSize < 2) {
		cout << "ERROR: in CircularGenome constructor, alphabetSize for unsigned char must be 2 or greater and 256 or less!" << endl;
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}

template<class T>
shared_ptr<AbstractGenome> CircularGenome<T>::makeCopy(shared_ptr<ParametersTable> _PT) {
	if (_PT == nullptr) {
		_PT = PT;
	}

	auto newGenome = make_shared<CircularGenome>(alphabetSize, 1, _PT);
	newGenome->sites = sites; 

	return newGenome;
}



template<class T>
shared_ptr<AbstractGenome::Handler> CircularGenome<T>::newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	//cout << "In Genome::newHandler()" << endl;
	return make_shared<Handler>(_genome, _readDirection);
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
void CircularGenome<T>::copyFrom(shared_ptr<AbstractGenome> from) {
	auto castFrom = dynamic_pointer_cast<CircularGenome<T>>(from);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	alphabetSize = castFrom->alphabetSize;
	sites.clear();
	for (auto site : castFrom->sites) {
		sites.push_back(site);
	}
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

template<class T>
void CircularGenome<T>::pointMutate() {
	sites[Random::getIndex((int)sites.size())] = Random::getIndex((int)alphabetSize);
}

template<>
void CircularGenome<double>::pointMutate() {
	sites[Random::getIndex((int)sites.size())] = Random::getDouble(alphabetSize);
}

// apply mutations to this genome
template<class T>
void CircularGenome<T>::mutate() {
	int howManyPoint = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationPointRatePL->get(PT));
	int howManyCopy = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationCopyRatePL->get(PT));
	int howManyDelete = Random::getBinomial((int)sites.size(), CircularGenomeParameters::mutationDeleteRatePL->get(PT));
	// do some point mutations
	for (int i = 0; i < howManyPoint; i++) {
		pointMutate();
	}
	// do some copy mutations
	int MaxGenomeSize = CircularGenomeParameters::sizeMaxPL->get(PT);
	int IMax = CircularGenomeParameters::mutationCopyMaxSizePL->get(PT);
	int IMin = CircularGenomeParameters::mutationCopyMinSizePL->get(PT);
	for (int i = 0; (i < howManyCopy) && (((int)sites.size()) < MaxGenomeSize); i++) {
		//chromosome->mutateCopy(PT.lookup("mutationCopyMinSize"), PT.lookup("mutationCopyMaxSize"), PT.lookup("chromosomeSizeMax"));

		//cout << "size: " << sites.size() << "->";
		////shared_ptr<Chromosome<T>> segment = dynamic_pointer_cast<Chromosome<T>>(getSegment(minSize, maxSize));

		int segmentSize = Random::getInt(IMax - IMin) + IMin;
		if (segmentSize > (int)sites.size()) {
			cout << "segmentSize = " << segmentSize << "  sites.size() = " << (int)sites.size() << endl;
			cout << "maxSize:minSize" << IMax << ":" << IMin << endl;
			cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for insert is > then sites.size()!\nExitting!" << endl;
			exit(1);
		}
		int segmentStart = Random::getInt((int)sites.size() - segmentSize);
		vector<T> segment;
		segment.clear();
		auto it = sites.begin();
		segment.insert(segment.begin(), it + segmentStart, it + segmentStart + segmentSize);

		////insertSegment(segment);
		it = sites.begin();
		sites.insert(it + Random::getInt((int)sites.size()), segment.begin(), segment.end());

		//cout << sites.size() << endl;
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
			cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << endl;
			cout << "maxSize : minSize   " << DMax << " : " << DMin << endl;
			cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for delete is > then sites.size()!\nExitting!" << endl;
			exit(1);
		}
		int segmentStart = Random::getInt(((int)sites.size()) - segmentSize);
		sites.erase(sites.begin() + segmentStart, sites.begin() + segmentStart + segmentSize);
	}
}

// make a mutated genome. from this genome
// inherit the ParamatersTable from the calling instance
template<class T>
shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFrom(shared_ptr<AbstractGenome> parent) {
	auto newGenome = make_shared<CircularGenome<T>>(PT);
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
shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFromMany(vector<shared_ptr<AbstractGenome>> parents) {
	//cout << "In Genome::makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents)" << endl;
	// first, check to make sure that parent genomes are conpatable.
	auto castParent0 = dynamic_pointer_cast<CircularGenome<T>>(parents[0]);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.

	auto newGenome = make_shared<CircularGenome<T>>(castParent0->alphabetSize,0,PT);
	//newGenome->alphabetSize = castParent0->alphabetSize;

//	vector<shared_ptr<AbstractChromosome>> parentChromosomes;
//	for (auto rawParent : parents) {
//		auto parent = dynamic_pointer_cast<CircularGenome<T>>(rawParent);
//		parentChromosomes.push_back(parent->chromosomes[i]);  // make a vector that contains the nth chromosome from each parent
//	}
//	newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, newGenome->PT.lookup("genomecrossCount"));  // create a crossover chromosome

	if (parents.size() == 0) {
		cout << "ERROR! in CircularGenome<T>::makeMutatedGenomeFromMany crossover... attempt to cross 0 parents\nExiting!" << endl;
		exit(1);
	}
	if (parents.size() == 1) {  // if there is only one parent...
		//cout << "one parent" << endl;
		return castParent0->makeMutatedGenomeFrom(castParent0);
	} else {
		//cout << "many parent" << endl;

		// extract the sites list from each parent
		vector<vector<T>> parentSites;
		for (auto parent : parents) {
			parentSites.push_back(dynamic_pointer_cast<CircularGenome<T>>(parent)->sites);
		}

		// randomly determine crossCount number crossLocations
		vector<double> crossLocations;
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
	//cout << "  Leaving Genome::makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents)\n" << flush;
	return newGenome;
}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector

template<class T>
DataMap CircularGenome<T>::getStats(string& prefix) {
	DataMap dataMap;
	dataMap.set(prefix + "genomeLength", countSites());
	return (dataMap);
}


template<class T>
DataMap CircularGenome<T>::serialize(string& name) {
	DataMap serialDataMap;
	serialDataMap.set(name + "_genomeLength", countSites());
	serialDataMap.set(name + "_sites", genomeToStr());
	return serialDataMap;
}

// given a DataMap and PT, return genome [name] from the DataMap
template<class T>
void CircularGenome<T>::deserialize(shared_ptr<ParametersTable> PT, unordered_map<string, string>& orgData, string& name) {
	char nextChar;
	string nextString;
	T value;
	// make sure that data has needed columns
	if (orgData.find("GENOME_" + name + "_sites") == orgData.end() || orgData.find("GENOME_" + name + "_genomeLength") == orgData.end()) {
		cout << "  In CircularGenome<T>::deserialize :: can not find either GENOME_" + name + "_sites or GENOME_" + name + "_genomeLength.\n  exiting" << endl;
		exit(1);
	}
	int genomeLength;
	load_value(orgData["GENOME_" + name + "_genomeLength"], genomeLength);

	string allSites = orgData["GENOME_" + name + "_sites"].substr(1, orgData["GENOME_" + name + "_sites"].size() - 1);
	std::stringstream ss(allSites);

	sites.clear();
	ss >> nextChar; // read past leading '['
	ss >> nextChar; // load first char of first site
	for (int i = 0; i < genomeLength; i++) {
		nextString = "";
		while (nextChar != ',' && nextChar != ']') {
			nextString += nextChar;
			ss >> nextChar;
		}
		load_value(nextString, value);
		//cout << nextString << " = " << value << ", ";
		sites.push_back(value);
		ss >> nextChar;
	}
	//cout << endl;
}

template<>
void CircularGenome<unsigned char>::deserialize(shared_ptr<ParametersTable> PT, unordered_map<string, string>& orgData, string& name) {
	char nextChar;
	string nextString;
	int value;
	// make sure that data has needed columns
	if (orgData.find("GENOME_" + name + "_sites") == orgData.end() || orgData.find("GENOME_" + name + "_genomeLength") == orgData.end()) {
		cout << "  In CircularGenome<T>::deserialize :: can not find either GENOME_" + name + "_sites or GENOME_" + name + "_genomeLength.\n  exiting" << endl;
		exit(1);
	}
	int genomeLength;
	load_value(orgData["GENOME_" + name + "_genomeLength"], genomeLength);

	string allSites = orgData["GENOME_" + name + "_sites"].substr(1, orgData["GENOME_" + name + "_sites"].size() - 1);
	std::stringstream ss(allSites);

	sites.clear();
	ss >> nextChar; // read past leading '['
	ss >> nextChar; // load first char of first site
	for (int i = 0; i < genomeLength; i++) {
		nextString = "";
		while (nextChar != ',' && nextChar != ']') {
			nextString += nextChar;
			ss >> nextChar;
		}
		load_value(nextString, value);
		//cout << nextString << " = " << value << ", ";
		sites.push_back((unsigned char)value);
		ss >> nextChar;
	}
	//cout << endl;
}



template<class T>
void CircularGenome<T>::recordDataMap() {
	dataMap.set("alphabetSize", alphabetSize);
	dataMap.set("genomeLength", countSites());

}
/*
// load all genomes from a file
template<class T>
void CircularGenome<T>::loadGenomeFile(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) {
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

			shared_ptr<CircularGenome<T>> newGenome = make_shared<CircularGenome<T>>(PT);
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
void CircularGenome<unsigned char>::loadGenomeFile(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) {
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

			shared_ptr<CircularGenome<unsigned char>> newGenome = make_shared<CircularGenome<unsigned char>>(PT);
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
string CircularGenome<T>::genomeToStr() {
	stringstream ss;
	ss << "\"[";

	for (size_t i = 0; i < sites.size()-1; i++) {
		ss << sites[i] << FileManager::separator;
	}
	ss << sites[sites.size() - 1] << "]\"";
	return ss.str();
}

template<>
string CircularGenome<unsigned char>::genomeToStr() {
	stringstream ss;
	ss << "\"[";

	for (size_t i = 0; i < sites.size() - 1; i++) {
		ss << (int)sites[i] << FileManager::separator;
	}
	ss << (int)sites[sites.size() - 1] << "]\"";
	return ss.str();
}




template<class T>
void CircularGenome<T>::printGenome() {
	cout << "alphabetSize: " << getAlphabetSize() << endl;
	cout << genomeToStr();
	cout << endl;
}

