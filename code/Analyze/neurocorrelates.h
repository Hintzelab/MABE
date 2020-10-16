#pragma once
#include <string> 
#include <vector> 
#include <map> 


namespace neurocorrelates {
	// R-Measure
	// Information shared between the brain and the environment not shared with the sensors  
	// r = H(S,E) + H(S,M) - H(S) - H(E,M,S)
	double getR(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Reflection 
	// Information shared between the brain and the sensors not shared with the environment 
	// SensorReflection = H(E,S) + H(E,M) – H(E) – H(E,M,S)
	double getSensorReflection(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// WorldSensor 
	// Information shared between the world and the sensors not shared with the memory 
	// WorldSensor = H(M,E) + H(M,S) – H(M) – H(E,M,S)
	double getWorldSensor(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Coherent Information 
	// Information Shared exclusively between the memory, environment, and sensor 
	// CoherentInfo = H(E,M,S) + H(S) + H(M)  + H(E) – H(M,E) – H(M,S) – H(S,E) 
	double getCoherentInfo(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// EnvironmentMemoryShared
	// Information shared between the brain and the environment  
	// EnvironmentMemoryShared = H(M)  + H(E) – H(M,E)
	double getEnvironmentMemoryShared(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// SensorMemoryShared 
	// Information shared between the brain and the sensors
	// SensorMemoryShared = H(S) + H(M) – H(M,S)
	double getSensorMemoryShared(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// EnvironmentSensorShared  
	// Information shared between the world and the sensors
	// EnvironmentSensorShared = H(S)  + H(E) – H(S,E)
	double getEnvironmentSensorShared(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);


	// Total Correlate
	// Information shared between the brain and the environment and the sensor and all pairs therein 
	// TotalCorrelate= H(E,S) + H(E,M) + H(S,M) – 2H(E,M,S)
	double getTotalCorrelate(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Memory Arc 
	// Information shared between the brain and either the environment or the sensors  
	// MemoryArc = H(E,S) + H(M) – H(E,M,S)
	double getMemoryArc(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Arc 
	// Information shared between the sensors and either the environment or the memory  
	// SensorArc = H(E,M) + H(S) – H(E,M,S)
	double getSensorArc(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// World Arc 
	// Information shared between the environment and either the brain or the sensors  
	// WorldArc = H(S,M)  + H(E) – H(E,M,S)
	double getWorldArc(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Pair Correlates
	// Information shared between the brain, the environment, and the sensors but not all 3 together  
	// PairCorrelates = 2H(E,S) + 2H(E,M) + 2H(M,S) – H(S) – H(E) – H(M) – 3H(E,M,S)
	double getPairCorrelates(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Memory Pair 
	// Information shared between the brain and either the environment or the sensors but not both 
	// MemoryPair = 2H(E,S) + H(E,M) + H(S,M) – H(E)  – H(S) – 2H(E,M,S)
	double getMemoryPair(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Sensor Pair 
	// Information shared between the sensors and either the environment or the brain but not both   
	// SensorPair = H(E,S) + 2H(E,M) + H(M,S) – H(M) – H(E) – 2H(E,M,S) 
	double getSensorPair(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// World Pair 
	// Information shared between the environment and either the brain or the sensors but not both   
	// WorldPair = H(M,E) + H(S,E) + 2H(S,M) – H(S) – H(M) – 2H(E,M,S)
	double getWorldPair(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// Converts a vector of integers to size_t for indexing 
	size_t vectorBoolToInt(const std::vector<int> & myVec);

	// Converts part of a vector of integers to size_t for indexing 
	size_t vector1PartToInt(std::vector<int>::const_iterator start, std::vector<int>::const_iterator end);

	// Converts 2 parts of a vector of integers to size_t for indexing 
	size_t vector2PartsToInt(std::vector<int>::const_iterator start1, std::vector<int>::const_iterator end1, std::vector<int>::const_iterator start2, std::vector<int>::const_iterator end2);

	// returns entropy given a vector of probabilities 
	double calcEntropy(const std::map<int, int> & myVec, double probability);

	// Calulate All 
	// generates all correlates and returns a map of name, value pairs
	std::map<std::string,double> calculateAll(const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// returns a specific neurocorrelate by index 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared 
	double getNeurocorrelate(int whichCorrelate, const std::vector<std::vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits);

	// returns the string name for a neurocorrelate by index 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared 
	std::string getNeurocorrelateString(int whichCorrelate);

	// returns the maximum value (in bits) of the indexed neurocorrelate 
	// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
	// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
	// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared 
	double getMaxBits(int whichCorrelate, size_t sensorBits, size_t environmentBits, size_t memoryBits);

}
