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

inline std::string get_var_typename(const bool &) { return "bool"; }

inline std::string get_var_typename(const std::string &) { return "string"; }

inline std::string get_var_typename(const int &) { return "int"; }

inline std::string get_var_typename(const double &) { return "double"; }

// not as efficient as it could be (should be an iterable range)
template <typename Match = std::smatch>
inline std::vector<Match> forEachRegexMatch(const std::string &s,
                                            const std::regex &r) {
  std::vector<Match> v;
  for (std::sregex_iterator end,
       i = std::sregex_iterator(s.begin(), s.end(), r);
       i != end; i++)
    v.push_back(Match(*i));
  return v;
}

inline std::vector<std::string>
nameSpaceToNameParts(const std::string &name_space) {
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


inline std::vector<std::string> parseCSVLine(std::string raw_line,
                                             const char separator = ',',
                                             const char sep_except = '"') {
  std::vector<std::string> data_line;
  std::string s(1,separator), se(1,sep_except);
  const std::regex piece(R"((.*?)()" + s + "|" + se + R"(|$))");
  bool in_quotes = false;
  std::string quoted_string;
  for (auto &m : forEachRegexMatch(raw_line, piece)) {
    if (m[2].str() == se) {
      if (!in_quotes) {
        data_line.push_back(m[1].str());
        in_quotes = true;
      } else {
        quoted_string += m[1].str();
        data_line.push_back(quoted_string);
        quoted_string = "";
        in_quotes = false;
      }
    } else {
      if (!in_quotes)
        data_line.push_back(m[1].str());
      else
        quoted_string += m[0].str();
    }
  }
  data_line.erase(std::remove_if(data_line.begin(), data_line.end(),
                                 [](std::string s) { return s == ""; }),
                  data_line.end());
  return data_line;
}


inline std::map<std::string, std::vector<std::string>>
readColumnsFromCSVFile(const std::string &file_name, const char separator = ',',
                       const char sep_except = '"') {
  std::map<std::string, std::vector<std::string>> data; // the final map
  std::ifstream file(file_name);
  if (!file.is_open()) {
    std::cout << " Error: readColumnsFromCSVFile cannot open file " << file_name
              << std::endl;
    exit(1);
  }

  std::string raw_line;
  getline(file, raw_line);
  auto attribute_names = parseCSVLine(raw_line, separator, sep_except);

  while (getline(file, raw_line)) {
    auto data_line = parseCSVLine(raw_line, separator, sep_except);
    for (auto i = 0u; i < data_line.size(); i++)
      data[attribute_names.at(i)].push_back(data_line.at(i));
  }

  return data;
}


// extract a value from a map<string,vector<string>>
// given a value from one vector, return the value in another vector at the same
// index
inline std::string
CSVLookUp(std::map<std::string, std::vector<std::string>> csv_table,
          const std::string &lookup_key, const std::string &lookup_value,
          const std::string &return_key) {

  if (csv_table.find(lookup_key) == csv_table.end()) {
    std::cout << " Error : CSVLookup could not find requested lookup key"
              << std::endl;
    exit(1);
  }

  if (csv_table.find(return_key) == csv_table.end()) {
    std::cout << "Error : CSVLookup could not find requested return key "
              << std::endl;
    exit(1);
  }

  auto iter = std::find(csv_table[lookup_key].begin(),
                        csv_table[lookup_key].end(), lookup_value);
  if (iter == csv_table[lookup_key].end()) {
    std::cout << "Error : CSVLookup could not find requested lookup value"
              << std::endl;
    exit(1);
  }

  size_t pos = std::distance(csv_table[lookup_key].begin(), iter);
  if (csv_table[return_key].size() <= pos) {
    std::cout
        << "Error : CSVLookup could not find extract lookup value index from "
           "return_key"
        << std::endl;
    exit(1);
  }

  return csv_table[return_key][pos];
}


template <class T>
inline static bool load_value(const std::string &value, T &target) {
  std::stringstream ss(value);
  std::string remaining;
  return ss >> target ? !(ss >> remaining) : false;
}

// Put an arbitrary value to the target variable, return false on conversion
// failure (COPIES FUNCTION OF load_value()!)
template <class T>
inline static bool stringToValue(const std::string &source, T &target) {
  std::stringstream ss(source);
  std::string remaining;
  return ss >> target ? !(ss >> remaining) : false;
}


// converts a vector of string to a vector of type of returnData
template <class T>
inline void convertCSVListToVector(std::string string_data,
                                   std::vector<T> &return_data,
                                   const char separator = ',',
                                   const char sep_except = '"') {
  return_data.clear();
  // check all uses of this function to see if leading and trailing quotes are
  // needed
  static const std::regex stripoff_qoute(R"(^"(.*?)?"$)");
  static const std::regex stripoff_square_brackets(R"(^\[(.*?)\]$)");
  std::smatch m_quote;
  string_data = std::regex_match(string_data, m_quote, stripoff_qoute)
                    ? m_quote[1].str()
                    : string_data;
  std::smatch m_square;
  string_data = std::regex_match(string_data, m_square, stripoff_square_brackets)
                    ? m_square[1].str()
                    : string_data;

  T temp; // immediately assign from stringToValue
  for (auto &s : parseCSVLine(string_data, separator, sep_except)) {
    if (!stringToValue(s, temp)) {
      std::cout << " --- while parsing: " << string_data << " .... "
                << std::endl;
      std::cout << " In convertCSVListToVector() attempt to convert string "
                << s << " to  value failed\n " << std::endl;
      exit(1);
    }
    return_data.push_back(temp);
  }
}

// this is here so we can use to string and it will work even if we give it a
// string as input
inline std::string to_string(std::string str) { return (str); }

/*
 * getBestInVector(vector<T> vec)
 * given W (a list of scores), return the index of the highest score
 * if more then one genome has max score, return the first one.
 */
template <typename Type>
inline int findGreatestInVector(std::vector<Type> vec) {
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

inline std::vector<int> seq(const std::string sequence_string,
                            int default_max = -1, bool add_zero = false) {
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

// load a line from FILE. IF the line is empty or a comment (starts with #),
// skip line.
// if the line is not empty/comment, clean ss and load line.
// rawLine is the string version of the same data as ss
inline bool loadLineToSS(std::ifstream &file, std::string &rawLine,
                         std::stringstream &ss) {
  rawLine.resize(0);
  if (file.is_open() && !file.eof()) {
    while ((rawLine.size() == 0 || rawLine[0] == '#') && !file.eof()) {
      getline(file, rawLine);
    }
    ss.clear();
    ss.str(std::string());
    ss << rawLine;
  } else if (!file.eof()) {
    std::cout << "in loadSS, file is not open!\n  Exiting." << std::endl;
    exit(1);
  }
  // cout << "from file:  " << rawLine << endl;
  return file.eof();
}


inline std::map<long, std::map<std::string, std::string>>
getAttributeMapByID(const std::string &file_name) {

  std::map<long, std::map<std::string, std::string>> result;

  std::ifstream file(file_name);
  if (!file.is_open()) {
    std::cout << " Error: in getAttributeMapByID unable to open " << file_name
              << std::endl;
    exit(1);
  }

  std::string raw_line;
  getline(file, raw_line);
  auto attribute_names = parseCSVLine(raw_line);

  if (std::find(attribute_names.begin(), attribute_names.end(), "ID") ==
      attribute_names.end()) {
    std::cout << " Error: in getAttributeMapByID no ID column in file "
              << file_name << std::endl;
    exit(1);
  }

  while (getline(file, raw_line)) {
    auto data_line = parseCSVLine(raw_line);
    std::map<std::string, std::string> data;
    for (auto i = 0u; i < data_line.size(); i++)
      data[attribute_names.at(i)] = data_line.at(i);
    result[std::stol(data.at("ID"))] = data;
  }

  return result;
}

