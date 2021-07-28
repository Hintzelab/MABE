#pragma once


#include <vector>
#include <math.h>
#include <cmath>

#include <map>
#include <unordered_map>
#include <set>
#include <iostream>
#include <string>
#include <algorithm>    // std::find




#include "entropy.h"

namespace SMR {

	// Atomic R
	// R-value calculated for a specific world concept and node in the brain 
	// Same equation as R, but with individual M_i and E_j 
	double getAtomicR(size_t whichConcept, size_t whichBrainNode, const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainState);

	// Atomic R Array
	// Gets Atomic R values for all concepts and all brain nodes 
	std::vector<std::vector<double>> getAtomicRArray(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainState);


	// smearedness of different concepts across nodes 
	// nodes i, concepts j, k
	// S_N = sum_(i) sum_(j > k)  min(M_ji, Mki)
	double getSmearednessOfConcepts(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates );

	// smearedness of concepts across different nodes
	// concept i, nodes j, k
	// S_C = sum_(i) sum_(j > k)  min(M_ij, Mik)
	double getSmearednessOfNodes(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates);

	std::pair<double, double> getSmearednessConceptsNodesPair(const TS::intTimeSeries& inputStates, const TS::intTimeSeries& worldStates, const TS::intTimeSeries& brainStates);
}

