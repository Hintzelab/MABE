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
#include <climits> // UINT_MAX

namespace Random {

using Generator = std::mt19937;

// for borrowed EMPIRICAL code support
static const int32_t _BINOMIAL_TO_NORMAL = 50;     // if < n*p*(1-p)
static const int32_t _BINOMIAL_TO_POISSON = 1000;  // if < n && !Normal approx Engine

// Gives you access to the random number generator in general use
inline Generator &getCommonGenerator() {
  // to seed, do get_common_generator().seed(value);
  static Generator
      common; // This creates "common" which is a (random number) generator.
  // Since it is static, it is only created the first time this function is
  // called
  // after this, each time the function is called, a reference to the same
  // "common" is returned
  return common;
}

// result = Random::getDouble(7.2, 9.5);
// result is in [7.2, 9.5)
inline double getDouble(const double lower, const double upper,
                        Generator &gen = getCommonGenerator()) {
  return std::uniform_real_distribution<double>(lower, upper)(gen);
}

// result = Random::getDouble(9.5);
// result is in [0, 9.5)
inline double getDouble(const double upper,
                        Generator &gen = getCommonGenerator()) {
  return getDouble(0, upper, gen);
}

// result = Random::getInt(7, 9);
// result is in [7, 9]
inline int getInt(const int lower, const int upper,
                  Generator &gen = getCommonGenerator()) {
  return std::uniform_int_distribution<int>(lower, upper)(gen);
}

// result = Random::getInt(9);
// result is in [0, 9]
inline int getInt(const int upper, Generator &gen = getCommonGenerator()) {
  return getInt(0, upper, gen);
}

// Returns a random valid index of a container which has "container_size"
// elements.
inline int getIndex(const int container_size,
                    Generator &gen = getCommonGenerator()) {
  return getInt(0, container_size - 1, gen);
}

// Returns true with "probability" probability
inline bool P(const double probability, Generator &gen = getCommonGenerator()) {
  return std::bernoulli_distribution(probability)(gen);
}

/**
 * (FROM EMPIRICAL)
 * Generate a random variable drawn from a Poisson distribution.
 *
 * @param mean The mean of the distribution.
 **/
inline uint32_t EmpGetRandPoisson(const double mean, Generator &gen = getCommonGenerator()) {
	// Draw from a Poisson Dist with mean; if cannot calculate, return UINT_MAX.
	// Uses Rejection Method
	const double a = exp(-mean);
	if (a <= 0) return UINT_MAX; // cannot calculate, so return UINT_MAX
	uint32_t k = 0;
	double u = getDouble(1.0, gen);
	while (u >= a) {
		u *= getDouble(1.0, gen);
		++k;
	}
	return k;
}

/**
 * (FROM EMPIRICAL)
 * Generate a random variable drawn from a Poisson distribution.
 **/
inline uint32_t EmpGetRandPoisson(const double n, double p, Generator &gen = getCommonGenerator()) {
  //emp_assert(p >= 0.0 && p <= 1.0, p);
  // Optimizes for speed and calculability using symetry of the distribution
  if (p > .5) return (uint32_t)n - EmpGetRandPoisson(n * (1 - p), gen);
  else return EmpGetRandPoisson(n * p, gen);
}

/**
 * (FROM EMPIRICAL)
 * Generate a random variable drawn from a Binomial distribution.
 *
 * This function is exact, but slow.
 * @see Random::GetApproxRandBinomial
 * @see emp::Binomial in source/tools/Distribution.h
 **/
inline uint32_t EmpGetFullRandBinomial(const double n, const double p, Generator &gen = getCommonGenerator()) {
  //emp_assert(p >= 0.0 && p <= 1.0, p);
  //emp_assert(n >= 0.0, n);
  // Actually try n Bernoulli events, each with probability p
  uint32_t k = 0;
  for (uint32_t i = 0; i < n; ++i) if (P(p)) k++;
  return k;
}

/**
 * (FROM EMPIRICAL)
 * Generate a random variable drawn from a Binomial distribution.
 *
 * This function is faster than @ref Random::GetFullRandBinomial(), but
 * uses some approximations.  Note that for repeated calculations with
 * the same n and p, the Binomial class provides a much faster and more
 * exact interface.
 *
 * @see Random::GetFullRandBinomial
 * @see emp::Binomial in source/tools/Distribution.h
 **/
inline uint32_t EmpGetRandBinomial(const int n, const double p, Generator &gen = getCommonGenerator()) {
  //emp_assert(p >= 0.0 && p <= 1.0, p);
  //emp_assert(n >= 0.0, n);
  // Approximate Binomial if appropriate

  // if np(1-p) is large, we might be tempted to use a Normal approx, but it is giving poor results.
  // if (n * p * (1 - p) >= _BINOMIAL_TO_NORMAL) {
  //   return static_cast<uint32_t>(GetRandNormal(n * p, n * p * (1 - p)) + 0.5);
  // }

  // If n is large, use a Poisson approx
  if (n >= _BINOMIAL_TO_POISSON) {
    uint32_t k = EmpGetRandPoisson(n, p, gen);
    if (k < UINT_MAX) return k; // if approx worked
  }

  // otherwise, actually generate the randBinomial
  return EmpGetFullRandBinomial(n, p, gen);
}

// Returns how many successes you get by doing "tests" number of trials
// with "probability" of success
inline int getBinomial(const int tests, const double probability,
                       Generator &gen = getCommonGenerator()) {
  return EmpGetRandBinomial(tests, probability, gen);
  //return std::binomial_distribution<>(tests, probability)(gen);
}

// Returns a double drawn from a normal (Gaussian) distribution with mean "mu"
// and
// standard deviation "sigma".
inline double getNormal(const double mu, const double sigma,
                        Generator &gen = getCommonGenerator()) {
  return std::normal_distribution<>(mu, sigma)(gen);
}
}
