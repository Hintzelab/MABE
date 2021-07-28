#pragma once


#include <vector>
#include <math.h>
#include <cmath>

#include <map>
#include <unordered_map>
#include <set>
#include "../Utilities/PowerSet.h"
#include "../Utilities/Data.h"
#include <iostream>
#include <string>
#include "entropy.h"


namespace FRAG {
	// return size of smallest partition with at least threshold % shared entropy with feature
	//      a value of -1 indicates that no partition contains atleast threshold shared entropy relitive to feature
	// predictor: vector of states containing the source data (i.e. what is used to make prediction)
	//     source and fettures must be the same size AND each element of source must be the same size AND each element of features must be the same size
	// feature: vector of states containing the data we are want to predict (i.e. we will ask how well source predicts features)
	// threshold: the first partition of source that has atleast this amount of shared entropy with feature as compaired with features total entropy will trigger a return
	// compairTo: If "feature", function works as decribed. If "shared", threshold comparison is made agaist max shared entropy as aposed to feature entropy (i.e. it will always succed unless feature entropy is 0)
	// maxPartitionSize: max size of partitions of source to consider, if -1 (defaut) consider all partitions
	int getFragmentation(const TS::intTimeSeries& feature, const TS::intTimeSeries& Predictor, double threshold = 1.0, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);

	// given a vector of features (TimeSeriess) and predictor (intTimeSeries) return a list of fragmentation for each feature
	// uses getFragmentation
	std::vector<int> getFragmentationSet(const std::vector<TS::intTimeSeries>& features, const TS::intTimeSeries& predictor, double threshold = 1.0, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);

	// given a feature(intTimeSeries) and predictor (intTimeSeries) return a list of fragmentation for each element of feature
	// this uses deconstructTimeSeries to convert features into a vector of TimeSeriess and then calls the alternet version of getFragmentionSet
	std::vector<int> getFragmentationSet(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, double threshold = 1.0, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);


	// this function takes features (vector<TimeSeriess>) and predictor (intTimeSeries) and calculates how much each partition of predictor "knows" about each feature.
	// first create a power set of index lists for all partitions of predictor (excluding the empty set)
	// next determin the shared entropy between each partition of predictor and each feature
	// return a pair where the first element is the list of partition indeces, and the second element is matrix of shared entropies
	//  each row of the matrix is the values for each parition relitive to one feautre
	// the parameter compareTo can be set to:
	//   "none" - show mutual entropy of feature and partition
	//   "feature" = show mutual entropy... / feature entropy
	//   "shared" = show mutual entropy... / mutual entropy of feature and whole predictor
	// in addtion, the last two columns and last row of the matrix provide additional information
	//   column[-2] = entropy of feature
	//   column[-1] = entropy mutual entropy of feature and whole predictor (i.e. largest partition)
	//   row[-1] = the entropy of each partition (last two elements are set to 0)
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> getFragmentationMatrix(const std::vector<TS::intTimeSeries>& features, const TS::intTimeSeries& predictor, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);

	// wrapper for getFragmentationMatrix which takes feature as a intTimeSeries rather then vector<intTimeSeries>
	// this uses deconstructTimeSeries to convert features into a vector of TimeSeriess and then calls the alternet version of getFragmentationMatrix
	std::pair<std::vector<std::vector<int>>, std::vector<std::vector<double>>> getFragmentationMatrix(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);

	// save a collection of fragmentation matrices derived from a set of time ranges
	void saveFragMatrixSet(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::vector<int>& lifeTimes, const std::vector<std::pair<double, double>>& lifeTimeRanges, const std::string& fileName, const std::string& compareTo = "feature", int maxPartitionSize = -1, bool reflectPartitions = false);

	// save a single fragmentation matrix
	void saveFragMatrix(const TS::intTimeSeries& features, const TS::intTimeSeries& predictor, const std::string& fileName, const std::string& compareTo = "feature", std::vector<std::string> featureNames = {}, int maxPartitionSize = -1, bool reflectPartitions = false);

}