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




#include "timeSeries.h"

namespace ENT {
	// calculate entropy for a intTimeSeries X
	double Entropy(const TS::intTimeSeries& X);
	double MutualEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y);
	double ConditionalEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y);
	double ConditionalMutualEntropy(const TS::intTimeSeries& X, const TS::intTimeSeries& Y, const TS::intTimeSeries& Z);
}

