#pragma once


#include <vector>
#include <math.h>
#include <cmath>

#include <map>
#include <unordered_map>
#include <set>
#include <iostream>
#include <string>
#include <numeric>      // std::accumulate

#include "Utilities/Utilities.h"

namespace TS {
	typedef std::vector<std::vector<int>> intTimeSeries;
	typedef std::vector<std::vector<double>> TimeSeries;
	enum class Position { FIRST, LAST }; // used with trimTimeSeries and extendTimeSeries
	enum class RemapRules { INT, BIT, TRIT, NEAREST_INT, NEAREST_BIT, NEAREST_TRIT, MEDIAN, UNIQUE }; // used with remapTimeSeries

	// convert one sample from an intTimeSeries to a string, sep will be placed between elements
	std::string TimeSeriesSampleToString(const std::vector<int>& sample, const std::string& sep = " ");
	std::string TimeSeriesSampleToString(const std::vector<double>& sample, const std::string& sep = " ");

	// convert an intTimeSeries to a vector<string>, sep will be placed between elements
	std::vector<std::string> TimeSeriesToVectString(const intTimeSeries& X, const std::string& sep = " ");
	std::vector<std::string> TimeSeriesToVectString(const TimeSeries& X, const std::string& sep = " ");

	// convert an intTimeSeries to a string, sepSample will be placed between samples, and sepElement between elements in each sample
	std::string TimeSeriesToString(const intTimeSeries& X, const std::string& sepElement = " ", const std::string& sepSample = "\n");
	std::string TimeSeriesToString(const TimeSeries& X, const std::string& sepElement = " ", const std::string& sepSample = "\n");

	// given a intTimeSeries and a list of indices, return a new intTimeSeries where each state only has elements indicated by indices
	// example indices: {0,1,3}
	// old intTimeSeries    new intTimeSeries
	// 4,5,3,2,1    ->    4,5,2
	// 3,1,2,1,1    ->    3,1,1
	// 9,9,1,2,3    ->    9,9,2
	intTimeSeries subSetTimeSeries(const intTimeSeries& X, const std::vector<int>& indices);

	// given a intTimeSeries (i.e. vector<vector<int>>) X return a vector<intTimeSeries> were each element of states of X is now it's own intTimeSeries
	// (used to isolate features, for example, when calculating fragmentation)
	std::vector<intTimeSeries> deconstructTimeSeries(const intTimeSeries& X);

	// given 2 TimeSeriess (X and Y) return a new intTimeSeries where each state is {X0,X1,..,Xn,Y0,Y1,...,Yn}
	intTimeSeries Join(const intTimeSeries& X, const intTimeSeries& Y);

	// given a vector of TimeSeriess (X and Y) return a new intTimeSeries where each state is {X0,X1,..,Xn,Y0,Y1,...,Yn}
	intTimeSeries Join(const std::vector<intTimeSeries>& data);

	// given a intTimeSeries (experience), a range (START,END) from [0.0,...,1.0], and either a number of lives or a list of lifeTimes
	// return new intTimeSeries where for each life states before START and after END are removed
	// If lives is used, (experience.size() / lives) must be an int (assumes all lives are the same length).
	// If lifeTimes is used, then sum(lifeTimes) must equal experiance.size()
	intTimeSeries trimTimeSeries(const intTimeSeries& experience, const std::pair<double, double>& range, const std::vector<int>& lifeTimes);
	intTimeSeries trimTimeSeries(const intTimeSeries& experience, const std::pair<double, double>& range, size_t lives);

	// remove n samples the start (FIRST) or end (LAST) from each lifetime of an intTimeSeries and remove a new time series
	// If lives is used, (experience.size() / lives) must be an int (assumes all lives are the same length).
	// If lifeTimes is used, then sum(lifeTimes) must equal experiance.size()
	//   - OR -
	//     sum(lifeTimes) + lifeTimes.size() == experiance.size() - which case, each lifeTime will be assued to be 1 larger
	//     this is to address the issue casued by oversized TS - note, n = 1 in this case will result in a new TS that matches lifeTimes
	//     i.e. hidden is generaly recorded at T+1, but on the first update must also be recored at T - resutling in 1 extra sample per lifetime
	intTimeSeries trimTimeSeries(const intTimeSeries& experience, const Position& removeWhich, const std::vector<int>& lifeTimes, int n = 1);
	intTimeSeries trimTimeSeries(const intTimeSeries& experience, const Position& removeWhich, size_t lives, int n = 1);

	// given intTimeSeries X and state Y, concat Y at the begining or end of every life in X , n times
	intTimeSeries extendTimeSeries(const intTimeSeries& X, const std::vector<int>& lifeTimes, const std::vector<int> Y, Position addWhere, int n = 1);
	intTimeSeries extendTimeSeries(const intTimeSeries& X, const size_t lives, const std::vector<int> Y, Position addWhere, int n = 1);

	// given a lifeTimes list (i.e. list of lifeTimes) add n to each lifetime (n may be negative) and return a new lifeTimes list
	std::vector<int> updateLifeTimes(const std::vector<int>& lifeTimes, int n);

	// given a TimeSeries X and a mapping rule, return a new intTimeSeries based on rule
	intTimeSeries remapToIntTimeSeries(const TimeSeries& X, RemapRules rule, std::vector<double> ruleParameter = { -1 });
}