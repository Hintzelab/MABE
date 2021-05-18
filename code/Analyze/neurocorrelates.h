#pragma once
#include <string> 
using std::string;
#include <vector> 
using std::vector;
#include <map> 
using std::map; using std::pair;


namespace neurocorrelates {
	// R-Measure
	// Information shared between the brain and the environment not shared with the sensors  
	// r = H(S,E) + H(S,M) - H(S) - H(E,M,S)
	double getR(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Reflection 
	// Information shared between the brain and the sensors not shared with the environment 
	// SensorReflection = H(E,S) + H(E,M) – H(E) – H(E,M,S)
	double getSensorReflection(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// WorldSensor 
	// Information shared between the world and the sensors not shared with the memory 
	// WorldSensor = H(M,E) + H(M,S) – H(M) – H(E,M,S)
	double getWorldSensor(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Coherent Information 
	// Information Shared exclusively between the memory, environment, and sensor 
	// CoherentInfo = H(E,M,S) + H(S) + H(M)  + H(E) – H(M,E) – H(M,S) – H(S,E) 
	double getCoherentInfo(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// EnvironmentMemoryShared
	// Information shared between the brain and the environment  
	// EnvironmentMemoryShared = H(M)  + H(E) – H(M,E)
	double getEnvironmentMemoryShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// SensorMemoryShared 
	// Information shared between the brain and the sensors
	// SensorMemoryShared = H(S) + H(M) – H(M,S)
	double getSensorMemoryShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// EnvironmentSensorShared  
	// Information shared between the world and the sensors
	// EnvironmentSensorShared = H(S)  + H(E) – H(S,E)
	double getEnvironmentSensorShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);


	// Total Correlate
	// Information shared between the brain and the environment and the sensor and all pairs therein 
	// TotalCorrelate= H(E,S) + H(E,M) + H(S,M) – 2H(E,M,S)
	double getTotalCorrelate(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Memory Arc 
	// Information shared between the brain and either the environment or the sensors  
	// MemoryArc = H(E,S) + H(M) – H(E,M,S)
	double getMemoryArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Arc 
	// Information shared between the sensors and either the environment or the memory  
	// SensorArc = H(E,M) + H(S) – H(E,M,S)
	double getSensorArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// World Arc 
	// Information shared between the environment and either the brain or the sensors  
	// WorldArc = H(S,M)  + H(E) – H(E,M,S)
	double getWorldArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Pair Correlates
	// Information shared between the brain, the environment, and the sensors but not all 3 together  
	// PairCorrelates = 2H(E,S) + 2H(E,M) + 2H(M,S) – H(S) – H(E) – H(M) – 3H(E,M,S)
	double getPairCorrelates(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Memory Pair 
	// Information shared between the brain and either the environment or the sensors but not both 
	// MemoryPair = 2H(E,S) + H(E,M) + H(S,M) – H(E)  – H(S) – 2H(E,M,S)
	double getMemoryPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Pair 
	// Information shared between the sensors and either the environment or the brain but not both   
	// SensorPair = H(E,S) + 2H(E,M) + H(M,S) – H(M) – H(E) – 2H(E,M,S) 
	double getSensorPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// World Pair 
	// Information shared between the environment and either the brain or the sensors but not both   
	// WorldPair = H(M,E) + H(S,E) + 2H(S,M) – H(S) – H(M) – 2H(E,M,S)
	double getWorldPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// R Norm 
	// R normalized by the amount of information in the environment minus shared with sensors   
	// RNorm = R / ( H(E,S) - H(S) ) 
	double getRNorm(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Environment Info 
	// Information in the environment minus information also in the sensors   
	// EnvirInfo = H(E,S) - H(S) 
	double getEnvironmentInfo(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Atomic R
	// R-value calculated for a specific world concept and node in the brain 
	// Same equation as R, but with individual M_i and E_j 
	double getAtomicR(size_t whichConcept, size_t whichBrainNode, const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Atomic R Array
	// Gets Atomic R values for all concepts and all brain nodes 
	vector<vector<double>> getAtomicRArray(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// smearedness of different concepts across nodes 
	// nodes i, concepts j, k
	// S_N = sum_(i) sum_(j > k)  min(M_ji, Mki)
	double getSmearednessOfConcepts(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// smearedness of concepts across different nodes
	// concept i, nodes j, k
	// S_C = sum_(i) sum_(j > k)  min(M_ij, Mik)
	double getSmearednessOfNodes(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	pair<double, double> getSmearednessConceptsNodesPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Converts a vector of integers to size_t for indexing 
	size_t vectorBoolToInt(const vector<int> & myVec);

	// Converts part of a vector of integers to size_t for indexing 
	size_t vector1PartToInt(vector<int>::const_iterator start, vector<int>::const_iterator end);

	// Converts 2 parts of a vector of integers to size_t for indexing 
	size_t vector2PartsToInt(vector<int>::const_iterator start1, vector<int>::const_iterator end1, vector<int>::const_iterator start2, vector<int>::const_iterator end2);

	size_t vectorPlusBoolToInt(vector<int>::const_iterator start, vector<int>::const_iterator end, int mbool);
	size_t vectorPlus2BoolToInt(vector<int>::const_iterator start, vector<int>::const_iterator end, int mbool1, int mbool2);

	// Converts 3 parts of a vector of integers (1 chunk already converted, 2 individual positions) to size_t for indexing 
	size_t seedPlus2BoolToInt(int seed, int mbool1, int mbool2);

	// Converts 2 parts of a vector of integers (1 chunk already converted, 1 individual position) to size_t for indexing 
	size_t seedPlusBoolToInt(int seed, int mbool);

	// returns entropy given a vector of probabilities 
	double calcEntropy(const map<int, int> & myVec, double probability);

	// Calulate All 
	// generates all correlates and returns a map of name, value pairs
	map<string, double> calculateAll(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// returns a specific neurocorrelate by index 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared, 15 - Smeardness of Concepts, 16 -  Smearedness of Nodes
	double getNeurocorrelate(int whichCorrelate, const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// returns the string name for a neurocorrelate by index 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared 
	string getNeurocorrelateString(int whichCorrelate);

	// returns the maximum value (in bits) of the indexed neurocorrelate 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared 
	double getMaxBits(int whichCorrelate, size_t sensorBits, size_t environmentBits, size_t memoryBits);


	// converts the vector<vector<double>> from a brain with non-bit hidden values to the vector<vector<int>> stateset format by finding the 
	// median value of each hidden node and classifying everything as either above (1) or below (0) the median. 
	// Uses the bit function to set sensor and environment bits
	vector<vector<int>> convertToBitByMedian(const vector<vector<double>> & oldStateSet);

}
