#include "neurocorrelates.h"
#include <string> 
using std::string; 
#include <vector> 
using std::vector; 
#include <map> 
using std::map; using std::pair; 
#include <algorithm> 
using std::copy; using std::min; using std::max; 
#include <cmath>
using std::log2; using std::pow;
#include <iostream>
using std::cout; using std::endl;

/*
double getTEMPLATE(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs; 
	map<int, int> memoryProbs;

	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	
	map<int, int> totalProbs;

	// Temp Vectors 
	vector<int> sensorTemp(sensorBits, 0);
	vector<int> environmentTemp(environmentBits, 0);
	vector<int> memoryTemp(memoryBits, 0);
	
	vector<int> environmentSensorTemp(environmentBits + sensorBits, 0);
	vector<int> memorySensorTemp(memoryBits + sensorBits, 0);
	vector<int> environmentMemoryTemp(environmentBits + memoryBits, 0);
	
	vector<int> totalTemp(environmentBits + sensorBits + memoryBits, 0);
	
	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		copy(row.begin(), row.begin() + sensorBits, sensorTemp.begin());
		sensorProbs[vectorBoolToInt(sensorTemp)] ++;
		
		copy(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, environmentTemp.begin());
		environmentProbs[vectorBoolToInt(environmentTemp)] ++;
		
		copy(row.begin() + sensorBits + environmentBits, row.end(), memoryTemp.begin());
		memoryProbs[vectorBoolToInt(memoryTemp)] ++;

		copy(row.begin(), row.begin() + sensorBits, environmentSensorTemp.begin());
		copy(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, environmentSensorTemp.begin() + sensorBits);
		environmentSensorProbs[vectorBoolToInt(environmentSensorTemp)] ++;

		copy(row.begin(), row.begin() + sensorBits, memorySensorTemp.begin());
		copy(row.begin() + sensorBits + environmentBits, row.end(), memorySensorTemp.begin() + sensorBits);
		memorySensorProbs[vectorBoolToInt(memorySensorTemp)] ++;

		copy(row.begin() + sensorBits + environmentBits, row.end(), environmentMemoryTemp.begin());
		copy(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, environmentMemoryTemp.begin() + memoryBits);
		environmentMemoryProbs[vectorBoolToInt(environmentMemoryTemp)] ++;
		
		copy(row.begin(), row.end(), totalTemp.begin());
		totalProbs[vectorBoolToInt(totalTemp)] ++;
		
	}

	calcEntropy(sensorProbs, c2); 
	calcEntropy(environmentProbs, c2);
	calcEntropy(memoryProbs, c2);
	
	calcEntropy(environmentSensorProbs, c2); 
	calcEntropy(memorySensorProbs, c2); 
	calcEntropy(environmentMemoryProbs, c2);
	
	calcEntropy(totalProbs, c2); 
	// Calculate final equation 
	return EQUATION;
}
*/

// R-Measure
// Information shared between the brain and the environment not shared with the sensors  
// r = H(S,E) + H(S,M) - H(S) - H(E,M,S)
double neurocorrelates::getR(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps 
	map<int, int> sensorProbs;
	map<int, int> totalProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;

		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;
	}
	
	// Calculate final equation 
	// r = H(S,E) + H(S,M) - H(S) - H(E,M,S)
	return calcEntropy(environmentSensorProbs, c2) + calcEntropy(memorySensorProbs, c2) - calcEntropy(sensorProbs, c2) - calcEntropy(totalProbs, c2);
}

// Sensor Reflection 
// Information shared between the brain and the sensors not shared with the environment 
// SensorReflection = H(E,S) + H(E,M) – H(E) – H(E,M,S)
double neurocorrelates::getSensorReflection(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps 
	map<int, int> environmentProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
	
	// Calculate final equation 
	// SensorReflection = H(E,S) + H(E,M) – H(E) – H(E,M,S)
	return calcEntropy(environmentSensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - calcEntropy(environmentProbs, c2) -	calcEntropy(totalProbs, c2); 
}

// WorldSensor 
// Information shared between the world and the sensors not shared with the memory 
// WorldSensor = H(M,E) + H(M,S) – H(M) – H(E,M,S)
double neurocorrelates::getWorldSensor(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> memoryProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}

	// Calculate final Equation 
	// WorldSensor = H(M,E) + H(M,S) – H(M) – H(E,M,S)
	return calcEntropy(memorySensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - calcEntropy(memoryProbs, c2) - calcEntropy(totalProbs, c2);
}

// Coherent Information 
// Information Shared exclusively between the memory, environment, and sensor 
// CoherentInfo = H(E,M,S) + H(S) + H(M)  + H(E) – H(M,E) – H(M,S) – H(S,E) 
double neurocorrelates::getCoherentInfo(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs; 
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;
		
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}

	// Calculate final equation 
	// CoherentInfo = H(E,M,S) + H(S) + H(M)  + H(E) – H(M,E) – H(M,S) – H(S,E)
	return calcEntropy(sensorProbs, c2) + calcEntropy(environmentProbs, c2) + calcEntropy(memoryProbs, c2) - calcEntropy(environmentSensorProbs, c2) - calcEntropy(memorySensorProbs, c2) - calcEntropy(environmentMemoryProbs, c2) + calcEntropy(totalProbs, c2);
}

// EnvironmentMemoryShared
// Information shared between the brain and the environment  
// EnvironmentMemoryShared = H(M)  + H(E) – H(M,E)
double neurocorrelates::getEnvironmentMemoryShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> environmentProbs;
	map<int, int> memoryProbs;
	map<int, int> environmentMemoryProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
	}

	// Calculate final equation 
	// EnvironmentMemoryShared = H(M) + H(E) – H(M, E)
	return calcEntropy(environmentProbs, c2) + calcEntropy(memoryProbs, c2) - calcEntropy(environmentMemoryProbs, c2);
}

// SensorMemoryShared 
// Information shared between the brain and the sensors
// SensorMemoryShared = H(S) + H(M) – H(M,S)
double neurocorrelates::getSensorMemoryShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> memoryProbs;
	map<int, int> memorySensorProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;

		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;
	}

	// Calculate final equation 
	// SensorMemoryShared = H(S) + H(M) – H(M,S)
	return calcEntropy(sensorProbs, c2) + calcEntropy(memoryProbs, c2) - calcEntropy(memorySensorProbs, c2);
}

// EnvironmentSensorShared  
// Information shared between the world and the sensors
// EnvironmentSensorShared = H(S)  + H(E) – H(S,E)
double neurocorrelates::getEnvironmentSensorShared(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs;
	map<int, int> environmentSensorProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;

		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
	}

	// Calculate final equation 
	// EnvironmentSensorShared = H(S)  + H(E) – H(S,E)
	return calcEntropy(sensorProbs, c2) + calcEntropy(environmentProbs, c2) - calcEntropy(environmentSensorProbs, c2);
}


// Total Correlate
// Information shared between the brain and the environment and the sensor and all pairs theirin 
// TotalCorrelate= H(E,S) + H(E,M) + H(S,M) – 2H(E,M,S)
double neurocorrelates::getTotalCorrelate(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}

	// Calculate final equation 
	// TotalCorrelate = H(E, S) + H(E, M) + H(S, M) – 2H(E, M, S)
	return calcEntropy(environmentSensorProbs, c2) + calcEntropy(memorySensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - 2 * calcEntropy(totalProbs, c2);
}

// Memory Arc 
// Information shared between the brain and either the environment or the sensors  
// MemoryArc = H(E,S) + H(M) – H(E,M,S)
double neurocorrelates::getMemoryArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
 
	// Calculate final equation 
	// MemoryArc = H(E,S) + H(M) – H(E,M,S)
	return calcEntropy(memoryProbs, c2) + calcEntropy(environmentSensorProbs, c2) - calcEntropy(totalProbs, c2);;
}

// Sensor Arc 
// Information shared between the sensors and either the environment or the memory  
// SensorArc = H(E,M) + H(S) – H(E,M,S)
double neurocorrelates::getSensorArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;
	
		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
 
	// Calculate final equation 
	// SensorArc = H(E,M) + H(S) – H(E,M,S)
	return calcEntropy(sensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - calcEntropy(totalProbs, c2);
}

// World Arc 
// Information shared between the environment and either the brain or the sensors  
// WorldArc = H(S,M)  + H(E) – H(E,M,S)
double neurocorrelates::getWorldArc(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> environmentProbs; 
	map<int, int> memorySensorProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
 
	// Calculate final equation 
	// WorldArc = H(S,M)  + H(E) – H(E,M,S)
	return calcEntropy(environmentProbs, c2) + calcEntropy(memorySensorProbs, c2) - calcEntropy(totalProbs, c2);
}

// Pair Correlates
// Information shared between the brain, the environment, and the sensors but not all 3 together  
// PairCorrelates = 2H(E,S) + 2H(E,M) + 2H(M,S) – H(S) – H(E) – H(M) – 3H(E,M,S)
double neurocorrelates::getPairCorrelates(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs; 
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;
	
	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;
		
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
 
	// Calculate final equation 
	// PairCorrelates = 2H(E,S) + 2H(E,M) + 2H(M,S) – H(S) – H(E) – H(M) – 3H(E,M,S)
	return 2 * calcEntropy(environmentSensorProbs, c2) + 2 * calcEntropy(memorySensorProbs, c2) + 2 * calcEntropy(environmentMemoryProbs, c2) - calcEntropy(sensorProbs, c2) - calcEntropy(environmentProbs, c2) - calcEntropy(memoryProbs, c2) - 3 * calcEntropy(totalProbs, c2);
}

// Memory Pair 
// Information shared between the brain and either the environment or the sensors but not both 
// MemoryPair = 2H(E,S) + H(E,M) + H(S,M) – H(E)  – H(S) – 2H(E,M,S)
double neurocorrelates::getMemoryPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs; 
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;
		
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
	
		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
	 
	// Calculate final equation 
	// MemoryPair = 2H(E,S) + H(E,M) + H(S,M) – H(E)  – H(S) – 2H(E,M,S)
	return 2 * calcEntropy(environmentSensorProbs, c2) + calcEntropy(memorySensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - calcEntropy(sensorProbs, c2) - calcEntropy(environmentProbs, c2) - 2 * calcEntropy(totalProbs, c2);
}

// Sensor Pair 
// Information shared between the sensors and either the environment or the brain but not both   
// SensorPair = H(E,S) + 2H(E,M) + H(M,S) – H(M) – H(E) – 2H(E,M,S) 
double neurocorrelates::getSensorPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> environmentProbs; 
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}

	// Calculate final equation 
	// SensorPair = H(E,S) + 2H(E,M) + H(M,S) – H(M) – H(E) – 2H(E,M,S) 
	return calcEntropy(environmentSensorProbs, c2) + calcEntropy(memorySensorProbs, c2) + 2 * calcEntropy(environmentMemoryProbs, c2) - calcEntropy(environmentProbs, c2) - calcEntropy(memoryProbs, c2) - 2 * calcEntropy(totalProbs, c2);
}

// World Pair 
// Information shared between the environment and either the brain or the sensors but not both   
// WorldPair = H(M,E) + H(S,E) + 2H(S,M) – H(S) – H(M) – 2H(E,M,S)
double neurocorrelates::getWorldPair(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;

		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}
 
	// Calculate final equation 
	// WorldPair = H(M,E) + H(S,E) + 2H(S,M) – H(S) – H(M) – 2H(E,M,S)
	return calcEntropy(environmentSensorProbs, c2) + 2 * calcEntropy(memorySensorProbs, c2) + calcEntropy(environmentMemoryProbs, c2) - calcEntropy(sensorProbs, c2) - calcEntropy(memoryProbs, c2) - 2 * calcEntropy(totalProbs, c2);
}

// Converts a vector of integers to size_t for indexing 
size_t neurocorrelates::vectorBoolToInt(const vector<int> & myVec) {
	size_t boolSum = 0;
	for (size_t i = 0; i < myVec.size(); i++) {
		boolSum = (boolSum << 1) + myVec[i];
	}
	return boolSum;
}

// Converts part of a vector of integers to size_t for indexing 
size_t neurocorrelates::vector1PartToInt(vector<int>::const_iterator start, vector<int>::const_iterator end) {
	size_t boolSum = 0;
	for (vector<int>::const_iterator temp = start; temp != end; temp++) {
		boolSum = (boolSum << 1) + *temp; 
	}
	return boolSum;
}

// Converts 2 parts of a vector of integers to size_t for indexing 
size_t neurocorrelates::vector2PartsToInt(vector<int>::const_iterator start1, vector<int>::const_iterator end1, vector<int>::const_iterator start2, vector<int>::const_iterator end2) {
	size_t boolSum = 0;
	for (vector<int>::const_iterator temp = start1; temp != end1; temp++) {
		boolSum = (boolSum << 1) + *temp;
	}
	for (vector<int>::const_iterator temp = start2; temp != end2; temp++) {
		boolSum = (boolSum << 1) + *temp;
	}
	return boolSum;
}


// returns entropy given a vector of probabilities 
double neurocorrelates::calcEntropy(const map<int, int> & myVec, double probability) {
	double entropySum = 0.0;
	double temp = 0;
	for (pair<int, int> mpair : myVec) {
		temp = probability * mpair.second;
		entropySum += (temp * log2(temp));
	}
	return -1 * entropySum;
}

// Calulate All 
// generates all correlates and returns a map of name, value pairs
map<string,double> neurocorrelates::calculateAll(const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	// Probability Maps
	map<int, int> sensorProbs;
	map<int, int> environmentProbs; 
	map<int, int> memoryProbs;
	map<int, int> environmentSensorProbs;
	map<int, int> memorySensorProbs;
	map<int, int> environmentMemoryProbs;
	map<int, int> totalProbs;

	// Temp Vectors 
	vector<int> sensorTemp(sensorBits, 0);
	vector<int> environmentTemp(environmentBits, 0);
	vector<int> memoryTemp(memoryBits, 0);
	vector<int> environmentSensorTemp(environmentBits + sensorBits, 0);
	vector<int> memorySensorTemp(memoryBits + sensorBits, 0);
	vector<int> environmentMemoryTemp(environmentBits + memoryBits, 0);
	vector<int> totalTemp(environmentBits + sensorBits + memoryBits, 0);
	
	double c2 = 1.0 / stateSet.size();

	for (auto const & row : stateSet) {
		// Copy into temp vectors and add to probabilities 
		sensorProbs[vector1PartToInt(row.begin(), row.begin() + sensorBits)] ++;
		
		environmentProbs[vector1PartToInt(row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		memoryProbs[vector1PartToInt(row.begin() + sensorBits + environmentBits, row.end())] ++;

		environmentSensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;

		memorySensorProbs[vector2PartsToInt(row.begin(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits, row.end())] ++;
		
		environmentMemoryProbs[vector2PartsToInt(row.begin() + sensorBits + environmentBits, row.end(), row.begin() + sensorBits, row.begin() + sensorBits + environmentBits)] ++;
		
		totalProbs[vector1PartToInt(row.begin(), row.end())] ++;
	}

	// Map to return 
	map<string, double> allCorrelates; 
	
	double h_s = calcEntropy(sensorProbs, c2);
	double h_e = calcEntropy(environmentProbs, c2); 
	double h_m = calcEntropy(memoryProbs, c2); 
	
	double h_m_s = calcEntropy(memorySensorProbs, c2);
	double h_m_e = calcEntropy(environmentMemoryProbs, c2);
	double h_e_s = calcEntropy(environmentSensorProbs, c2); 
	
	double h_e_m_s = calcEntropy(totalProbs, c2); 
	
	// Calculate CoherentInfo = H(E,M,S) + H(S) + H(M)  + H(E) – H(M,E) – H(M,S) – H(S,E)
	allCorrelates["CoherentInfo"] = (h_s + h_e + h_m - h_e_s - h_m_s - h_m_e + h_e_m_s);

	// Calculate R = H(S,E) + H(S,M) - H(S) - H(E,M,S)
	allCorrelates["R"] = (h_e_s + h_m_s - h_s - h_e_m_s);

	// Calculate SensorReflection = H(E,S) + H(E,M) – H(E) – H(E,M,S)
	allCorrelates["SensorReflection"] = (h_e_s + h_m_e - h_e -	h_e_m_s); 

	// Calculate WorldSensor = H(M,E) + H(M,S) – H(M) – H(E,M,S)
	allCorrelates["WorldSensor"] = (h_m_s + h_m_e - h_m - h_e_m_s);

	// Calculate TotalCorrelate = H(E,S) + H(E,M) + H(S,M) – 2H(E,M,S)
	allCorrelates["TotalCorrelate"] = (h_e_s + h_m_s + h_m_e - 2 * h_e_m_s);

	// Calculate MemoryArc = H(E,S) + H(M) – H(E,M,S)
	allCorrelates["MemoryArc"] = (h_m + h_e_s - h_e_m_s);

	// Calculate SensorArc = H(E,M) + H(S) – H(E,M,S)
	allCorrelates["SensorArc"] = (h_s + h_m_e - h_e_m_s);

	// Calculate WorldArc = H(S,M)  + H(E) – H(E,M,S)
	allCorrelates["WorldArc"] = (h_e + h_e_s - h_e_m_s);
	
	// Calculate PairCorrelates = 2H(E,S) + 2H(E,M) + 2H(M,S) – H(S) – H(E) – H(M) – 3H(E,M,S)
	allCorrelates["PairCorrelates"] = (2 * h_e_s + 2 * h_m_s + 2 * h_m_e - h_s - h_e - h_m - 3 * h_e_m_s);

	// Calculate MemoryPair = 2H(E,S) + H(E,M) + H(S,M) – H(E)  – H(S) – 2H(E,M,S)
	allCorrelates["MemoryPair"] = (2 * h_e_s + h_m_s + h_m_e - h_s - h_e - 2 * h_e_m_s);

	// Calculate SensorPair = H(E,S) + 2H(E,M) + H(M,S) – H(M) – H(E) – 2H(E,M,S) 
	allCorrelates["SensorPair"] = (h_e_s + h_m_s + 2 * h_m_e - h_e - h_m - 2 * h_e_m_s);

	// Calculate WorldPair = H(M,E) + H(S,E) + 2H(S,M) – H(S) – H(M) – 2H(E,M,S)
	allCorrelates["WorldPair"] = (h_e_s + 2 * h_m_s + h_m_e - h_s - h_m - 2 * h_e_m_s);

	// Calculate EnvironmentMemoryShared = H(M)  + H(E) – H(M,E)
	allCorrelates["EnvironmentMemoryShared"] = (h_m + h_e + h_m_e); 

	// Calculate SensorMemoryShared = H(S) + H(M) – H(M,S)
	allCorrelates["SensorMemoryShared"] = (h_s + h_m - h_m_s);

	// Calculate EnvironmentSensorShared = H(S)  + H(E) – H(S,E)
	allCorrelates["EnvironmentSensorShared"] = (h_s + h_e - h_e_s);
	
	return allCorrelates; 
}

// returns a specific neurocorrelate by index 
// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared
double neurocorrelates::getNeurocorrelate(int whichCorrelate, const vector<vector<int>> & stateSet, size_t sensorBits, size_t environmentBits, size_t memoryBits) {
	switch (whichCorrelate) {
	case 0:
		// 0 - rMeasure
		return getR(stateSet, sensorBits, environmentBits, memoryBits);
	case 1:
		// 1 - SensorReflection
		return getSensorReflection(stateSet, sensorBits, environmentBits, memoryBits);
	case 2:
		// 2 - WorldSensor
		return getWorldSensor(stateSet, sensorBits, environmentBits, memoryBits);
	case 3:
		// 3 - Coherent Information
		return getCoherentInfo(stateSet, sensorBits, environmentBits, memoryBits);
	case 4:
		// 4 - Total Correlate
		return getTotalCorrelate(stateSet, sensorBits, environmentBits, memoryBits);
	case 5:
		// 5 - Memory Arc
		return getMemoryArc(stateSet, sensorBits, environmentBits, memoryBits);
	case 6:
		// 6 - Sensor Arc
		return getSensorArc(stateSet, sensorBits, environmentBits, memoryBits);
	case 7:
		// 7 - World Arc
		return getWorldArc(stateSet, sensorBits, environmentBits, memoryBits);
	case 8:
		// 8 - Pair Correlates
		return getPairCorrelates(stateSet, sensorBits, environmentBits, memoryBits);
	case 9:
		// 9 - Memory Pair
		return getMemoryPair(stateSet, sensorBits, environmentBits, memoryBits);
	case 10:
		// 10 - Sensor Pair
		return getSensorPair(stateSet, sensorBits, environmentBits, memoryBits);
	case 11:
		// 11 - World Pair
		return getWorldPair(stateSet, sensorBits, environmentBits, memoryBits);
	case 12: 
		// 12 - EnvironmentMemoryShared
		return getEnvironmentMemoryShared(stateSet, sensorBits, environmentBits, memoryBits); 
	case 13: 
		// 13 - SensorMemoryShared
		return getSensorMemoryShared(stateSet, sensorBits, environmentBits, memoryBits); 
	case 14: 
		// 14 - EnvironmentSensorShared
		return getEnvironmentSensorShared(stateSet, sensorBits, environmentBits, memoryBits); 
	default:
		cout << "Error. Not using valid neurocorrelate. Exiting..." << endl;
		exit(1);
	}
}

// returns the string name for a neurocorrelate by index 
// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared
string neurocorrelates::getNeurocorrelateString(int whichCorrelate) {
	switch (whichCorrelate) {
	case 0:
		// 0 - rMeasure
		return "RMeasure";
	case 1:
		// 1 - SensorReflection
		return "SensorReflection";
	case 2:
		// 2 - WorldSensor
		return "WorldSensor";
	case 3:
		// 3 - Coherent Information
		return "CoherentInfo";
	case 4:
		// 4 - Total Correlate
		return "TotalCorrelate";
	case 5:
		// 5 - Memory Arc
		return "MemoryArc";
	case 6:
		// 6 - Sensor Arc
		return "SensorArc";
	case 7:
		// 7 - World Arc
		return "WorldArc";
	case 8:
		// 8 - Pair Correlates
		return "PairCorrelates";
	case 9:
		// 9 - Memory Pair
		return "MemoryPair";
	case 10:
		// 10 - Sensor Pair
		return "SensorPair";
	case 11:
		// 11 - World Pair
		return "WorldPair";
	case 12:
		// 12 - EnvironmentMemoryShared
		return "EnvironmentMemoryShared";
	case 13:
		// 13 - SensorMemoryShared
		return "SensorMemoryShared"; 
	case 14:
		// 14 - EnvironmentSensorShared
		return "EnvironmentSensorShared"; 
	default:
		cout << "Error. Not using valid neurocorrelate. Exiting..." << endl;
		exit(1);
	}
}

// returns the maximum value (in bits) of the indexed neurocorrelate 
// 0 - rMeasure, 1 - SensorReflection, 2 - WorldSensor, 3 - Coherent Information, 4 - Total Correlate,
// 5 - Memory Arc, 6 - Sensor Arc, 7 - World Arc, 8 - Pair Correlates, 9 - Memory Pair, 10 - Sensor Pair, 11 - World Pair
// 12 - EnvironmentMemoryShared, 13 - SensorMemoryShared, 14 - EnvironmentSensorShared
double neurocorrelates::getMaxBits(int whichCorrelate, size_t sensorBits, size_t environmentBits, size_t memoryBits){
	switch (whichCorrelate) {
	case 0:
		// 0 - rMeasure
		return min(environmentBits, memoryBits);
	case 1:
		// 1 - SensorReflection
		return min(memoryBits, sensorBits);
	case 2:
		// 2 - WorldSensor
		return min(environmentBits, sensorBits);
	case 3:
		// 3 - Coherent Information
		return min(environmentBits, min(memoryBits, sensorBits));
	case 4:
		// 4 - Total Correlate
		return min(min(environmentBits, min(memoryBits, sensorBits)) + max(min(memoryBits, sensorBits), min(max(memoryBits, sensorBits), environmentBits)), max(environmentBits, max(memoryBits, sensorBits)));
	case 5:
		// 5 - Memory Arc
		return min(memoryBits, sensorBits + environmentBits);
	case 6:
		// 6 - Sensor Arc
		return min(sensorBits, memoryBits + environmentBits);
	case 7:
		// 7 - World Arc
		return min(environmentBits, memoryBits + sensorBits);
	case 8:
		// 8 - Pair Correlates
		return min(min(environmentBits, min(memoryBits, sensorBits)) + max(min(memoryBits, sensorBits), min(max(memoryBits, sensorBits), environmentBits)), max(environmentBits, max(memoryBits, sensorBits)));
	case 9:
		// 9 - Memory Pair
		return min(memoryBits, sensorBits + environmentBits);
	case 10:
		// 10 - Sensor Pair
		return min(sensorBits, memoryBits + environmentBits);
	case 11:
		// 11 - World Pair
		return min(environmentBits, memoryBits + sensorBits);
	case 12:
		// 12 - EnvironmentMemoryShared
		return min(environmentBits, memoryBits);
	case 13:
		// 13 - SensorMemoryShared
		return min(memoryBits, sensorBits);
	case 14:
		// 14 - EnvironmentSensorShared
		return min(environmentBits, sensorBits);
	default:
		cout << "Error. Not using valid neurocorrelate. Exiting..." << endl;
		exit(1);
	}
}
