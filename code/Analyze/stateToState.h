#pragma once


#include <vector>
#include <math.h>
#include <cmath>

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "timeSeries.h"
#include "../Utilities/Data.h"

namespace S2S {
	void saveStateToState(const std::vector<TS::intTimeSeries>& nodes, const std::vector<TS::intTimeSeries>& links, std::vector<int> lifeTimes, std::string fileName);
}
