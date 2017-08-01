//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

// This file provides a wrapper around C++11's style of generating random
// numbers. We provide a "common" number generator so the entire code
// base can work with a global seed if they want, as well as some
// utility functions for getting common number types easily.

#pragma once

#include <random>

namespace Random {
using namespace std;
using Generator=mt19937;

// Gives you access to the random number generator in general use
inline Generator& getCommonGenerator() {
	// to seed, do get_common_generator().seed(value);
	static Generator common;  // This creates "common" which is a (random number) generator.
							  // Since it is static, it is only created the first time this function is called
							  // after this, each time the function is called, a reference to the same "common" is returned
	return common;
}

// result = Random::getDouble(7.2, 9.5);
// result is in [7.2, 9.5)
inline double getDouble(const double lower, const double upper, Generator& gen = getCommonGenerator()) {
	return uniform_real_distribution<double>(lower, upper)(gen);
}

// result = Random::getDouble(9.5);
// result is in [0, 9.5)
inline double getDouble(const double upper, Generator& gen = getCommonGenerator()) {
	return getDouble(0, upper, gen);
}

// result = Random::getInt(7, 9);
// result is in [7, 9]
inline int getInt(const int lower, const int upper, Generator& gen = getCommonGenerator()) {
	return uniform_int_distribution<int>(lower, upper)(gen);
}

// result = Random::getInt(9);
// result is in [0, 9]
inline int getInt(const int upper, Generator& gen = getCommonGenerator()) {
	return getInt(0, upper, gen);
}

// Returns a random valid index of a container which has "container_size" elements.
inline int getIndex(const int container_size, Generator& gen = getCommonGenerator()) {
	return getInt(0, container_size - 1, gen);
}

// Returns how many successes you get by doing "tests" number of trials
// with "probability" of success
inline int getBinomial(const int tests, const double probability, Generator& gen = getCommonGenerator()) {
	return binomial_distribution<>(tests, probability)(gen);
}

// Returns true with "probability" probability
inline bool P(const double probability, Generator& gen = getCommonGenerator()) {
	return bernoulli_distribution(probability)(gen);
}

// Returns a double drawn from a normal (Gaussian) distribution with mean "mu" and
// standard deviation "sigma".
inline double getNormal(const double mu, const double sigma, Generator& gen = getCommonGenerator()) {
	return normal_distribution<>(mu, sigma)(gen);
}

}
