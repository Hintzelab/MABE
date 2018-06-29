//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <vector>
#include <regex>
#include <cmath>
#include <numeric>

inline auto get_var_typename(const bool &) { return "bool"; }

inline auto  get_var_typename(const std::string &) { return "string"; }

inline auto  get_var_typename(const int &) { return "int"; }

inline auto  get_var_typename(const double &) { return "double"; }

// not as efficient as it could be (should be an iterable range)
template <typename Match = std::smatch>
inline auto forEachRegexMatch(const std::string &s,
                                            const std::regex &r) {
  std::vector<Match> v;
  for (std::sregex_iterator end,
       i = std::sregex_iterator(s.begin(), s.end(), r);
       i != end; i++)
    v.push_back(Match(*i));
  return v;
}

inline auto nameSpaceToNameParts(const std::string &name_space) {
  std::vector<std::string> name_parts;
  static const std::regex valid_name_space(R"(^(\w+::)*$)");
  if (!std::regex_match(name_space, valid_name_space)) {
    std::cout
        << "  Error::in nameSpaceToNameParts(const string& nameSpace). name "
           "space is invalid.\n  Can't parse \""
        << name_space << "\"\n   Parameter name space must end in ::\nExiting."
        << std::endl;
    exit(1);
  }
  static const std::regex name_space_part(R"(\w+::)");
  for (auto &m : forEachRegexMatch(name_space, name_space_part))
    name_parts.push_back(m.str());

  return name_parts;
}

/*  correctly get modulo when numerator negative
 * return x % y were (-1 % y) = (y - 1)
 * (-2 % y) = (y - 2), etc.
 */
inline int loopMod(const int numerator, const int denominator) {
  return ((numerator % denominator) + denominator) % denominator;
}

//  correctly get floating point  modulo when numerator negative
inline double loopModDouble(const double numerator, const double denominator) {
  return std::fmod(std::fmod(numerator, denominator) + denominator,
                   denominator);
}

// returns 1 if "d" is greater than 0, else return 0
template <typename Type> inline int Bit(Type d) { return d > 0.0; }

// returns 1 if "d" is greater than 0
//         0 if "d" is equal to 0
//        -1 if "d" is less than 0
template <typename Type> inline int Trit(Type d) { return d < 0 ? -1 : d > 0; }

// Put an arbitrary value to the target variable, return false on conversion
template <class T>
inline static bool stringToValue(const std::string &source, T &target) {
  std::stringstream ss(source);
  std::string remaining;
  return ss >> target ? !(ss >> remaining) : false;
}

// try and convert a std::string to a particular type
// warning: no error if value is not  valid type
template <typename T> inline static auto stringTo(std::string source) {
  std::stringstream ss(source);
  T target;
  ss >> target;
  return target;
}

template <typename T>
inline static auto convertTo(std::vector<std::string> l) {
  std::vector<T> data;
  for (auto &e : l)
    data.push_back(stringTo<T>(e));
  return data;
}

// this is here so we can use to string and it will work even if we give it a
// string as input
inline std::string to_string(std::string str) { return str; }

// return the index of max_element in vector<T>
template <typename T> inline auto findGreatestInVector(std::vector<T> vec) {
  return std::distance(vec.begin(), std::max_element(vec.begin(), vec.end()));
}

// takes a vector of template values and a vector of indices.
// each indicated value is converted to a bit and the resulting bit string is
// packed into an int.
// if reverseOrder, than the last index is read first
//
// useful to generate values for lookups
// reverseOrder is useful for gate lookups as it maintains meaning even if the
// number of inputs to a gate changes
// i.e. since the first input is read last, 1, 10 and 100 (etc.) all return 1.
// (01)
// and 01, 010, 0100 (etc.) all return 2 (10)
// and 11, 110, 1100 (etc.) all return 3 (11)
// etc... hopefully you see the pattern
template <typename Type>
inline int vectorToBitToInt(const std::vector<Type> &nodes,
                            const std::vector<int> &nodeAddresses,
                            bool reverseOrder = false) {
  auto node_addresses = nodeAddresses;
  if (reverseOrder)
    std::reverse(node_addresses.begin(), node_addresses.end());
  return std::accumulate(
      node_addresses.begin(), node_addresses.end(), 0,
      [&nodes](int result, int na) { return result * 2 + Bit(nodes.at(na)); });
}

template <typename Type>
inline int vectorToTritToInt(const std::vector<Type> &nodes,
                             const std::vector<int> &nodeAddresses,
                             bool reverseOrder = false) {
  auto node_addresses = nodeAddresses;
  if (reverseOrder)
    std::reverse(node_addresses.begin(), node_addresses.end());
  return std::accumulate(node_addresses.begin(), node_addresses.end(), 0,
                         [&nodes](int result, int na) {
                           return result * 3 + Trit(nodes.at(na)) + 1;
                         });
}

// converts a ',' separated formatted string to sequence with duplicates removed
// single number -> just add that number
// x-y -> add from x to y
// x-y:z -> add from x to y on z
// x:z -> from x to defaultMax on z (if defaultMax is not defined, error... see
// below)
// :z -? from 0 to defaultMax on z (if defaultMax is not defined, error... see
// below)
//
// on error (see x:z and :z or if the str is not formatted correctly), return an
// empty vector (size 0)
//
// addZero true will insure that 0 is in the sequence
//
// function works by first parsing seqStr into a vector where each element if a
// vector with {start,end,step}
// next, each list is added to a new vector. This vector is convered to a set
// and back to a vector (which both
// remove duplicates, and sorts)
//
// example input   /   output
// 4                                      /   4
// 10-20                                  /   10,11,12,13,14,15,16,17,18,19,20
// 10-20:3                                /   10,13,16,19
// 5:5 (with defaultMax = 20)             /   5,10,15,20
// :3 (with defaultMax = 20)              /   0,3,6,9,12,15,18
// 4,10-15,30:2 ((with defaultMax = 40)   /
// 4,10,11,12,13,14,15,30,32,34,36,38,40
inline auto seq(const std::string sequence_string, int default_max = -1,
                bool add_zero = false) {
  std::set<int> result;
  // as described above
  static const std::regex commas(
      R"(([^,]+)(?:,|$))");
  static const std::regex sub_seq( // needs to be documented somewhere
      R"((\d+)|(?:(?:(?:(\d+)-)?(\d+))?(?::(\d+))?))");
  for (auto & sub_seq_match : forEachRegexMatch(sequence_string, commas)) {
	std::string sub = sub_seq_match[1].str();
    std::smatch m;
    if (!std::regex_match(sub, m, sub_seq)) {
      std::cout << " Error : sequence " << sequence_string
                << " cannot be parsed";
      exit(1);
    }
    if (!m[1].str().empty()) {
      result.insert(std::stoi(m[1].str()));
      continue;
    }
    auto step = m[4].str().empty() ? 1 : std::stoi(m[4].str());
    auto start = m[2].str().empty()
                     ? m[3].str().empty() ? 0 : std::stoi(m[3].str())
                     : std::stoi(m[2].str());
    auto finish = m[2].str().empty() ? default_max : std::stoi(m[3].str());
    if (finish == -1) {
      std::cout << " Error: Must provide default_max or specify end of range "
                << std::endl;
      return std::vector<int>();
    }
    for (; start <= finish; start += step)
      result.insert(start);
  }
  if (add_zero)
    result.insert(0);
  std::vector<int> v(result.begin(), result.end());
  return v;
}

