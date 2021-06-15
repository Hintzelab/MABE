#pragma once


#include <vector>
#include <math.h>
#include <cmath>

#include <map>
#include <unordered_map>
#pragma once

#include <set>
#include <iostream>
#include <string>
#include <algorithm>    // std::find




#include "timeSeries.h"
#include "entropy.h"
#include "fragmentation.h"
#include "../Brain/AbstractBrain.h"

namespace BRAINTOOLS {
	void saveStateToState(std::shared_ptr<AbstractBrain> brain, std::string(fileName), TS::RemapRules remapRule = TS::RemapRules::BIT, std::vector<double> ruleParameter = { -1.0 });
	double getR(std::shared_ptr<AbstractBrain> brain, TS::intTimeSeries worldFeatures, TS::RemapRules remapRule = TS::RemapRules::BIT, std::vector<double> ruleParameter = { -1.0 });
}


