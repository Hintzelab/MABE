//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#include <limits>
#include "TemplatedChromosome.h"


// set a coding region value if that value is > -1 (values < 0 denote that the
// action should not be recorded in codingRegions
template <class T>
void TemplatedChromosome<T>::CodingRegions::assignCode(int code, int siteIndex,
                                                       int CodingRegionIndex) {
  if (code >= 0) {
    regions[CodingRegionIndex].push_back({siteIndex, code});
  }
}

// converts codingRegions to a string. If argument is provided, then only that
// region will
// will be returned (or a blank string)
template <class T>
std::string TemplatedChromosome<T>::CodingRegions::codingRegionsToString(
    int CodingRegionIndex) {
  std::string S = "";
  for (auto region : regions) {
    if (CodingRegionIndex < 0 || region.first == CodingRegionIndex) {
      S = S + std::to_string(region.first) + " -> ";
      for (auto site : region.second) {
        S = S + std::to_string(site.first) + ":" + std::to_string(site.second) + "  ";
      }
      S += "\n";
    }
  }
  return S;
}

// default fail constructor - if alphebetSize default has not been explicitly
// defined, this is run.
template <class T> TemplatedChromosome<T>::TemplatedChromosome() {
  alphabetSize = 0;
  std::cout << "ERROR : Chromosome constructor must specify chromosome length\n";
  exit(1);
}

template <class T>
TemplatedChromosome<T>::TemplatedChromosome(int chromosomeLength) {
  alphabetSize = 0;
  std::cout << "ERROR : TYPE specified for Chromosome is not supported.\nTypes "
          "supported are: int, double, bool, unsigned char\n";
  exit(1);
}

template <class T>
TemplatedChromosome<T>::TemplatedChromosome(int chromosomeLength,
                                            double _alphabetSize) {
  alphabetSize = 0;
  std::cout << "ERROR : TYPE specified for Chromosome is not supported.\nTypes "
          "supported are: int, double, bool, unsigned char\n";
  exit(1);
}

// return a shared_ptr to a new chromosome like this one (same alphabetSize and
// sites.size())
template <class T>
std::shared_ptr<AbstractChromosome> TemplatedChromosome<T>::makeLike() {
  return std::make_shared<TemplatedChromosome<T>>((int)sites.size(), alphabetSize);
}

template <class T>
std::shared_ptr<AbstractChromosome> TemplatedChromosome<T>::makeCopy() {
  std::shared_ptr<TemplatedChromosome> newChromosome =
      std::make_shared<TemplatedChromosome<T>>((int)sites.size(), alphabetSize);
  newChromosome->sites = sites;
  return newChromosome;
}

// read an int value in range [valueMin,valueMax] from chromosome starting at
// index.
// will use as many sites as needed. works in base alphabetSize
// return true if siteIndex went out of range
template <class T>
bool TemplatedChromosome<T>::readInt(int &siteIndex, int &value, int valueMin,
                                     int valueMax, bool readDirection, int code,
                                     int CodingRegionIndex) {
  if (valueMin > valueMax) {
    int temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  double currentMax = alphabetSize;
  modulateIndex(siteIndex);
  value = (int)sites[siteIndex];
  codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
  bool EOC = advanceIndex(siteIndex, readDirection); // EOC = end of chromosome
  while ((valueMax - valueMin + 1) > currentMax &&
         !EOC) { // we don't have enough bits of information NOTE: "!EOC" means
                 // that we stop at end of chromosome!
    value =
        ((int)value * (int)alphabetSize) + (int)sites[siteIndex]; // next site
    codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
    EOC = EOC | advanceIndex(siteIndex, readDirection);
    currentMax = currentMax * alphabetSize;
  }
  value = (value % (valueMax - valueMin + 1)) + valueMin;
  return EOC;
}

// when an int is read from a double chromosome, only one site is read (since it
// has sufficent accuracy) and then this value is
// scaled by alphabet size, then by valueMin, valueMax and then rounded.

template <>
bool TemplatedChromosome<double>::readInt(int &siteIndex, int &value,
                                          int valueMin, int valueMax,
                                          bool readDirection, int code,
                                          int CodingRegionIndex) {
  if (valueMin > valueMax) {
    int temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  valueMax += 1; // do this so that range is inclusive!
  modulateIndex(siteIndex);
  double siteValue = sites[siteIndex];
  codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
  bool EOC = advanceIndex(siteIndex, readDirection); // EOC = end of chromosome

  value = (int)((siteValue / alphabetSize) * (valueMax - valueMin)) + valueMin;
  return EOC;
}

// writes a value into a chromosome, uses a number of sites baised on valueMin
// and valueMax);
// return true if siteIndex went out of range
template <class T>
bool TemplatedChromosome<T>::writeInt(int &siteIndex, int value, int valueMin,
                                      int valueMax, bool readDirection) {
  bool EOC;
  if (valueMin > valueMax) {
    int temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  std::vector<T> decomposedValue;
  int writeValueBase = valueMax - valueMin + 1;
  value = value - valueMin;
  if (writeValueBase < value) {
    std::cout << "ERROR : attempting to write value to chromosome. \n value is too "
            "large :: (valueMax - valueMin + 1) < value!\n";
    exit(1);
  }
  while (
      writeValueBase >
      alphabetSize) { // load value in alphabetSize chunks into decomposedValue
    decomposedValue.push_back(value % ((int)alphabetSize));
    value = (int)((double)value / alphabetSize);
    writeValueBase = (int)((double)writeValueBase / alphabetSize);
  }
  decomposedValue.push_back(value);
  EOC = modulateIndex(siteIndex); // make sure that the index is in range
  while (decomposedValue.size() > 0 &&
         !EOC) { // starting with the last element in decomposedValue, copy into
                 // genome. NOTE: "!EOC" means that we stop at end of
                 // chromosome!
    sites[siteIndex] = decomposedValue[decomposedValue.size() - 1];
    EOC = EOC | advanceIndex(siteIndex, readDirection);
    decomposedValue.pop_back();
  }
  return EOC;
}

// when writing an int into a double genome, a number should be generated such
// that a read int will extract the same value.
// thus, value (an int) is scaled between valueMin and valueMax to a value
// between 0 and 1 and then scaled up by alphabet size.

template <>
bool TemplatedChromosome<double>::writeInt(int &siteIndex, int value,
                                           int valueMin, int valueMax,
                                           bool readDirection) {
  bool EOC;
  if (valueMin > valueMax) {
    int temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  valueMax += 1;
  EOC = modulateIndex(siteIndex); // make sure that the index is in range
  // cout << "  value: " << value << "  valueMin: " << valueMin << "  valueMax:
  // " << valueMax << "  final: " << (((double)(value - valueMin) /
  // (double)(valueMax - valueMin)) * alphabetSize) << endl;
  sites[siteIndex] =
      (((double)(value - valueMin) / (double)(valueMax - valueMin)) *
       alphabetSize);
  // cout << "  wrote: " << sites[siteIndex] << endl;
  EOC = EOC | advanceIndex(siteIndex, readDirection);
  return EOC;
}
// read one site of type T
// return true if siteIndex went out of range
template <class T>
bool TemplatedChromosome<T>::readSite(int &siteIndex, T &value,
                                      bool readDirection, int code,
                                      int CodingRegionIndex) {
  bool EOC = modulateIndex(siteIndex);
  value = sites[siteIndex];
  EOC = EOC | advanceIndex(siteIndex, readDirection);
  codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
  return EOC;
}

// version used with int, char and bool. valueMin will map to 0; valueMax will map to alphabetSize - 1
// function scales value using valueMin and valueMax to alphabetSize-1 and writes new value at siteIndex
// value - MIN(valueMin,valueMax) must be <= ABS(valueMax - valueMin)
template <class T>
bool TemplatedChromosome<T>::writeDouble(int &siteIndex, double value,
	double valueMin, double valueMax,
	bool readDirection) {
	bool EOC = modulateIndex(siteIndex);
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	if ((value - valueMin) > (valueMax - valueMin)) {
		std::cout << "Error: attempting to write double. given range is too small, "
			"value: "
			<< value << " is not < valueMax: " << valueMin
			<< " - valueMin: " << valueMin << "\n";
		exit(1);
	}
	value = ((value - valueMin) / (valueMax - valueMin)) * (alphabetSize - 1.0);
	sites[siteIndex] = (T)value;
	EOC = EOC | advanceIndex(siteIndex, readDirection);
	return EOC;
}

// version used with doubles, will simply scale value to alphabet size based on
// valueMin = 0 and valueMax = alphabetSize and write into genome at siteIndex
// (note, it is possible to write a value of aphabetsize)
template <>
bool TemplatedChromosome<double>::writeDouble(int &siteIndex, double value,
                                         double valueMin, double valueMax,
                                         bool readDirection) {
  bool EOC = modulateIndex(siteIndex);
  if (valueMin > valueMax) {
    double temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  if ((value - valueMin) > (valueMax - valueMin)) {
    std::cout << "Error: attempting to write double. given range is too small, "
            "value: "
         << value << " is not < valueMax: " << valueMin
         << " - valueMin: " << valueMin << "\n";
    exit(1);
  }
  value = ((value - valueMin) / (valueMax - valueMin)) * alphabetSize;
  sites[siteIndex] = value;
  EOC = EOC | advanceIndex(siteIndex, readDirection);
  return EOC;
}

// this is a scaling function - it will return a value in range valueMin/valueMax inclusive
// This version will be used with int, char and bool
// (note double specialized version is not inclusive on the upper bound)
template <class T>
bool TemplatedChromosome<T>::siteToDouble(int &siteIndex, double &value,
                                          double valueMin, double valueMax,
                                          bool readDirection, int code,
                                          int CodingRegionIndex) {
	std::cout << "not double read" << std::endl;
  if (valueMin > valueMax) {
    double temp = valueMin;
    valueMax = valueMin;
    valueMin = temp;
  }
  bool EOC = modulateIndex(siteIndex);
  value = (double)sites[siteIndex];
  codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
  EOC = EOC | advanceIndex(siteIndex, readDirection);

  //old version (did was not inclusive of upper bound):  value = (value * ((valueMax - valueMin) / alphabetSize)) + valueMin;

  value = (value / (alphabetSize - 1.0)) * (valueMax - valueMin) + valueMin;

  return EOC;
}

// double specialized version of siteToDouble. not inclusive on the upper bound
template <>
bool TemplatedChromosome<double>::siteToDouble(int &siteIndex, double &value,
	double valueMin, double valueMax,
	bool readDirection, int code,
	int CodingRegionIndex) {

	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	bool EOC = modulateIndex(siteIndex);
	value = (double)sites[siteIndex];
	codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	EOC = EOC | advanceIndex(siteIndex, readDirection);

	value = ((value / (alphabetSize)) * (valueMax - valueMin)) + valueMin;
	return EOC;
}

// resize a chromosome to length and fill will values from alphabet
template <class T> void TemplatedChromosome<T>::fillRandom(int length) {
  if (length == -1) {
    length = sites.size();
  }
  sites.resize(length);
  for (int i = 0; i < length; i++) {
    sites[i] = (T)Random::getDouble(alphabetSize);
  }
}

template <> void TemplatedChromosome<bool>::fillRandom(int length) {
  if (length == -1) {
    length = sites.size();
  }
  sites.resize(length);
  for (int i = 0; i < length; i++) {
    //std::cout << "alphabetSize: " << alphabetSize << "   "
    //     << Random::getDouble(alphabetSize) << std::endl;
    sites[i] = (bool)((int)Random::getDouble(alphabetSize));
  }
}

template <class T> void TemplatedChromosome<T>::fillRandom() {
  fillRandom(sites.size());
}

// starting with value start, fill this chromosome with acending values.
// This function is to make testing easy.
template <class T> void TemplatedChromosome<T>::fillAcending(int &start) {
  for (size_t i = 0; i < sites.size(); i++) {
    sites[i] = (T)(start % (int)alphabetSize);
    start++;
  }
}

// fill all the sites of this chromosome with value.
// This function is to make testing easy.
template <class T> void TemplatedChromosome<T>::fillConstant(const int value) {
  for (size_t i = 0; i < sites.size(); i++) {
    sites[i] = (T)(value % (int)alphabetSize);
  }
}

template <class T>
void TemplatedChromosome<T>::readChromosomeFromSS(std::stringstream &ss,
                                                  int _chromosomeLength) {
  char nextChar;
  std::string nextString;
  T value;
  sites.clear();

  bool streamNotEmpty(false);
  streamNotEmpty = static_cast<bool>(ss >> nextChar);
  for (int i = 0; i < _chromosomeLength; i++) {
    nextString = "";
    while (streamNotEmpty && (nextChar != ',')) {
      nextString += nextChar;
      streamNotEmpty = static_cast<bool>(ss >> nextChar);
    }
    convertString(nextString, value);
    // cout << nextString << " = " << value << ", ";
    sites.push_back(value);
    if (i < _chromosomeLength - 1) {
      streamNotEmpty = static_cast<bool>(ss >> nextChar);
    }
  }
  // cout << endl;
}

template <>
void TemplatedChromosome<unsigned char>::readChromosomeFromSS(
    std::stringstream &ss, int _chromosomeLength) {
  char nextChar;
  std::string nextString;
  int value;
  sites.clear();

  bool streamNotEmpty(false);
  streamNotEmpty = static_cast<bool>(ss >> nextChar);
  for (int i = 0; i < _chromosomeLength; i++) {
    nextString = "";
    while (streamNotEmpty && (nextChar != ',')) {
      nextString += nextChar;
      streamNotEmpty = static_cast<bool>(ss >> nextChar);
    }
    convertString(nextString, value);
    // cout << nextString << " = " << value << ", ";
    sites.push_back((char)value);
    if (i < _chromosomeLength - 1) {
      streamNotEmpty = static_cast<bool>(ss >> nextChar);
    }
  }
  // cout << endl;
}

// convert a chromosome to a string
template <class T> std::string TemplatedChromosome<T>::chromosomeToStr() {
  //	string S = "";
  //
  //	//S.reserve(((int)sites.size() * 2) + 10);
  //
  //	for (size_t i = 0; i < sites.size(); i++) {
  //		S.append(to_string(sites[i]) + FileManager::separator);
  //	}
  //	S.pop_back();  // clip off the leading separator
  //	return S;

  std::stringstream ss;
  ss << "";

  for (size_t i = 0; i < sites.size() - 1; i++) {
    ss << sites[i] << FileManager::separator;
  }
  ss << sites[sites.size() - 1];
  return ss.str();
}

template <> std::string TemplatedChromosome<unsigned char>::chromosomeToStr() {
  //	string S = "";
  //
  //	//S.reserve(((int)sites.size() * 2) + 10);
  //
  //	for (size_t i = 0; i < sites.size(); i++) {
  //		S.append(to_string(sites[i]) + FileManager::separator);
  //	}
  //	S.pop_back();  // clip off the leading separator
  //	return S;

  std::stringstream ss;
  ss << "";

  for (size_t i = 0; i < sites.size() - 1; i++) {
    ss << (int)sites[i] << FileManager::separator;
  }
  ss << (int)sites[sites.size() - 1];
  return ss.str();
}

template <class T> void TemplatedChromosome<T>::resize(int size) {
  sites.resize(size);
}
template <class T> int TemplatedChromosome<T>::size() {
  return (int)sites.size();
}

template <class T> DataMap TemplatedChromosome<T>::getFixedStats() {
  DataMap dataMap;
  dataMap.set("alphabetSize", alphabetSize);
  dataMap.setOutputBehavior("alphabetSize", DataMap::FIRST);
  return (dataMap);
}

template <class T> DataMap TemplatedChromosome<T>::getStats() {
  DataMap dataMap;
  dataMap.set("alphabetSize", alphabetSize);
  dataMap.setOutputBehavior("alphabetSize", DataMap::FIRST);
  dataMap.set("size", (int)sites.size());
  dataMap.setOutputBehavior("alphabetSize", DataMap::AVE);
  return (dataMap);
}

//////////////////////////////
// MUTATION RELATED METHODS //
//////////////////////////////

// create a new chromosome with a copied peice of this chromosome. The size of
// the copy
// will be random between minSize and maxSize. the copy location will be
// determined randomly
// but will at lease segmentSize before the end of the chromosome.
template <class T>
std::shared_ptr<AbstractChromosome> TemplatedChromosome<T>::getSegment(int minSize,
                                                                  int maxSize) {
  int segmentSize = Random::getInt(maxSize - minSize) + minSize;
  if (segmentSize > (int)sites.size()) {
    std::cout << "segmentSize = " << segmentSize
         << "  sites.size() = " << sites.size() << std::endl;
    std::cout << "maxSize:minSize" << maxSize << ":" << minSize << std::endl;
    std::cout << "ERROR: in Chromosome<T>::getSegement, segmentSize is > then "
            "sites.size()!\nExitting!"
         << std::endl;
    exit(1);
  }
  int segmentStart = Random::getInt(sites.size() - segmentSize);
  std::shared_ptr<TemplatedChromosome<T>> segment =
      std::make_shared<TemplatedChromosome<T>>(0, alphabetSize);
  auto it = sites.begin();
  segment->sites.insert(segment->sites.begin(), it + segmentStart,
                        it + segmentStart + segmentSize);
  // copy sites from this to segment (the new chromosome)
  // starting at segementStart and ending at segementStart+segmentSize
  return segment;
}

// insert from segement into this chromosome. The insertion location is random.
template <class T>
void TemplatedChromosome<T>::insertSegment(
    std::shared_ptr<AbstractChromosome> segment) {
  auto it = sites.begin();
  auto castSegment = std::dynamic_pointer_cast<TemplatedChromosome<T>>(segment);
  sites.insert(it + Random::getInt(sites.size()), castSegment->sites.begin(),
               castSegment->sites.end());
}

template <class T> void TemplatedChromosome<T>::mutatePoint() {
  sites[Random::getIndex(sites.size())] = (T)Random::getDouble(alphabetSize);
}

// mutate chromosome by getting a copy of a segment of this chromosome and
// inserting that segment randomly into this chromosome
template <class T>
void TemplatedChromosome<T>::mutateCopy(int minSize, int maxSize,
                                        int chromosomeSizeMax) {
  // cout << "C .. " << size() << " " << chromosomeSizeMax << ": ";
  if (size() < chromosomeSizeMax) {
    // cout << "size: " << sites.size() << "->";
    std::shared_ptr<TemplatedChromosome<T>> segment =
        std::dynamic_pointer_cast<TemplatedChromosome<T>>(
            getSegment(minSize, maxSize));
    insertSegment(segment);
    // cout << sites.size() << endl;
  }
}

// delete a random segement from the chromosome
template <class T>
void TemplatedChromosome<T>::mutateDelete(int minSize, int maxSize,
                                          int chromosomeSizeMin) {
  // cout << "D .. " << size() << " " << chromosomeSizeMin << ": ";
  if (size() > chromosomeSizeMin) {
    int segmentSize = Random::getInt(maxSize - minSize) + minSize;
    // cout << "segSize: " << segmentSize << "\tsize: " << sites.size() <<
    // "\t->\t";
    if (segmentSize > (int)sites.size()) {
      std::cout << "segmentSize = " << segmentSize
           << "  sites.size() = " << sites.size() << std::endl;
      std::cout << "maxSize : minSize   " << maxSize << " : " << minSize << std::endl;
      std::cout << "ERROR: in Chromosome<T>::mutateDelete, segmentSize is > then "
              "sites.size()!\nExitting!\n";
      exit(1);
    }
    int segmentStart = Random::getInt(sites.size() - segmentSize);
    sites.erase(sites.begin() + segmentStart,
                sites.begin() + segmentStart + segmentSize);
    // cout << sites.size() << endl;
  }
  // cout << endl;
}

// delete the sites of this chromosome. Then set sites to a crossed over
// chromosome made up of parents
template <class T>
void TemplatedChromosome<T>::crossover(
    std::vector<std::shared_ptr<AbstractChromosome>> parents, int crossCount) {
  sites.resize(0); // clear sites for this chromosome

  if (parents.size() == 0) {
    std::cout << "ERROR! in Chromosome crossover... attempt to cross 0 "
            "parents\nExiting!\n";
    exit(1);
  }
  if (parents.size() == 1) { // if there is only one parent, we are done.
    sites = std::dynamic_pointer_cast<TemplatedChromosome<T>>(parents[0])->sites;
  } else {

    // extract the sites list from each parent
    std::vector<std::vector<T>> parentSites;
    for (auto parent : parents) {
      parentSites.push_back(
          std::dynamic_pointer_cast<TemplatedChromosome<T>>(parent)->sites);
    }

    // randomly determine crossCount number crossLocations
    std::vector<double> crossLocations;
    for (int i = 0; i < crossCount;
         i++) { // get some cross locations (% of length of chromosome)
      crossLocations.push_back(Random::getDouble(1.0));
    }
    crossLocations.push_back(0.0); // add start and end
    crossLocations.push_back(1.0);

    sort(crossLocations.begin(), crossLocations.end()); // sort crossLocations

    //			cout << "crossing: ";
    //			for (auto location:crossLocations){
    //				cout << location << "  ";
    //			}
    //			cout << "size init: " << size() << " -> ";

    int pick;
    int lastPick = Random::getIndex(parents.size());
    for (size_t c = 0; c < crossLocations.size() - 1; c++) {
      // pick a chromosome to cross with. Make sure it's not the same
      // chromosome!
      pick = Random::getIndex(parents.size() - 1);
      if (pick == lastPick) {
        pick++;
      }
      lastPick = pick;
      // add the segment to this chromosome
      //				cout << "(" << parentSites[pick].size()
      //<< ") "<< (int)((double)parentSites[pick].size()*crossLocations[c]) << "
      //" << (int)((double)parentSites[pick].size()*crossLocations[c+1]) << " ";
      sites.insert(
          sites.end(),
          parentSites[pick].begin() +
              (int)((double)parentSites[pick].size() * crossLocations[c]),
          parentSites[pick].begin() +
              (int)((double)parentSites[pick].size() * crossLocations[c + 1]));
    }
    //			cout << "size post: " << size() << endl;
    //			cout << "      " << chromosomeToStr() << "\nfrom: " <<
    //parents[0]->chromosomeToStr() << "\n+\n and: " <<
    //parents[1]->chromosomeToStr() << endl << endl;
  }
  // sites = dynamic_pointer_cast<Chromosome<T>>(parents[0])->sites; //
  // uncomment to turn of crossover
}

template <class T> std::string TemplatedChromosome<T>::codingRegionsToString() {
  return codingRegions.codingRegionsToString();
}

////////////////////////
// SUPPORT FOR DOUBLE //
////////////////////////

template <> inline void TemplatedChromosome<double>::fillRandom(int length) {
  sites.resize(length);
  for (int i = 0; i < length; i++) {
    sites[i] = Random::getDouble(0, alphabetSize);
  }
}
template <> inline void TemplatedChromosome<double>::fillRandom() {
  TemplatedChromosome<double>::fillRandom(sites.size());
}

template <> inline void TemplatedChromosome<double>::mutatePoint() {
  sites[Random::getIndex(sites.size())] = Random::getDouble(alphabetSize);
}

// unsigned char constructors

template <> TemplatedChromosome<unsigned char>::TemplatedChromosome() {
  alphabetSize = 256;
}

template <>
TemplatedChromosome<unsigned char>::TemplatedChromosome(int chromosomeLength) {
  alphabetSize = 256;
  sites.resize(chromosomeLength);
}

template <>
TemplatedChromosome<unsigned char>::TemplatedChromosome(int chromosomeLength,
                                                        double _alphabetSize) {
  if (_alphabetSize > 256 || _alphabetSize < 2) {
    std::cout << "ERROR: in Chromosome constructor, alphabetSize for unsigned char "
            "must be 2 or greater and 256 or less!\n";
    exit(1);
  }
  alphabetSize = _alphabetSize;
  sites.resize(chromosomeLength);
}

// bool constructors

template <> TemplatedChromosome<bool>::TemplatedChromosome() {
  alphabetSize = 2;
}

template <>
TemplatedChromosome<bool>::TemplatedChromosome(int chromosomeLength) {
  alphabetSize = 2;
  sites.resize(chromosomeLength);
}

template <>
TemplatedChromosome<bool>::TemplatedChromosome(int chromosomeLength,
                                               double _alphabetSize) {
  if (_alphabetSize != 2) {
    std::cout << "ERROR: in Chromosome constructor, alphabetSize for bool must be "
            "2!\n";
    exit(1);
  }
  alphabetSize = 2;
  sites.resize(chromosomeLength);
}

// int constructors

template <>
TemplatedChromosome<int>::TemplatedChromosome(int chromosomeLength) {
  alphabetSize = 0;
  std::cout << "for Chromosome<int>, you must specify both chromosomeLength and "
          "alphabetSize\n";
  exit(1);
}

template <>
TemplatedChromosome<int>::TemplatedChromosome(int chromosomeLength,
                                              double _alphabetSize) {
  if (_alphabetSize < 2) {
    std::cout << "ERROR: in Chromosome constructor, alphabetSize for int must be 2 "
            "or greater!\n";
    exit(1);
  }
  alphabetSize = _alphabetSize;
  sites.resize(chromosomeLength);
}

// double constructors

template <>
TemplatedChromosome<double>::TemplatedChromosome(int chromosomeLength) {
  alphabetSize = 0;
  std::cout << "for Chromosome<double>, you must specify both chromosomeLength and "
          "alphabetSize\n";
}

template <>
TemplatedChromosome<double>::TemplatedChromosome(int chromosomeLength,
                                                 double _alphabetSize) {
  alphabetSize = _alphabetSize;
  sites.resize(chromosomeLength);
}
