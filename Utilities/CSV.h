
#pragma once


#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

// can parse a csv string into a vector of strings
// The delimiter and quotation character can be spedified:
// by default  , and "
// csv lines can also be directly converted to a vector of a
// templated type. Note: Behaviour is uspecified if all values
// in the csv string cannot be converted to the templated type

class CSVReader {

  std::string delimiter_, quotation_;
 
  // try and convert a std::string to a particular type
  // warning: no error if value is not  valid type
  template <typename T > inline static auto stringTo(std::string source) {
    std::stringstream ss(source);
    T target;
    ss >> target;
    return target;
  }

public:
  CSVReader() : CSVReader(',', '"') {}
  CSVReader(char d, char oq) : delimiter_(1, d), quotation_(1, oq) {}

  // template <class T> auto  stringTo(std::string source);
  // parse a csv line into a vector<T>
 //***
 //shouldn't be doing type conversion
// ***
 
  template <typename T= std::string>
  std::vector<T> parseLine(std::string raw_line) {
    std::vector<T> data;

	if (raw_line.empty())
		return data;
    
	auto current = 0u;

    while (true) {
      // find next delimiter
      auto delim = raw_line.find_first_of(delimiter_, current);
      // find next open quotation
      auto open_quote = raw_line.find_first_of(quotation_, current);
      // if the next open quotation comes before the next delimiter
      if (open_quote < delim) {
        //  find close quotation
        auto close_quote =
            raw_line.find_first_of(quotation_, open_quote + 1);
        // find first delimiter after that; ignoring delimiter inside of
        // quotation >> warning: assumes that quotes come in pairs
        delim = raw_line.find_first_of(delimiter_, close_quote);
      }
      // if line is completely parsed
      if (delim == std::string::npos) {
        break;
      }
      // add the next field
      data.push_back(stringTo<T>(raw_line.substr(current, delim - current)));
      // search for the next field
      current = delim + 1;
    }

    // add the last field
    data.push_back(stringTo<T>(raw_line.substr(current)));

    return data;
  }
};

// parses a csv file and stores in memory.
// the first line of the file is treated as the column headers
// The delimiter and quotation character can be specified:
// by default  , and "
class CSV {

  // to do the reading
  CSVReader reader_;
  // the 1st row; column headers
  std::vector<std::string> columns_;
  // remaning rows of the file
  std::vector<std::vector<std::string>> rows_;

  std::string file_name_;

public:
  CSV(std::string fn, char s, char se);
  CSV(std::string fn) : CSV(fn, ',', '"') {}

  // return csv file name
  auto fileName() const { return file_name_; }

  // number of columns in the file
  auto column_count() const { return columns_.size(); }

  // number of rows in the file
  auto row_count() const { return rows_.size(); }
  
  // return all columns in the file
  auto columns() const { return columns_; }

  // return all rows in the file
  auto rows() const { return rows_; }

  // return all values corresponding to a single column
  std::vector<std::string> singleColumn(std::string column);

  // look up a value in a column and return the value in the corresponding row of
  // the other column
  std::string lookUp(std::string lookup_column, std::string value,
                     std::string return_column) const;

  // merges another csv file. Only extra columns are added. All values in the
  // specified column must also exist in the same column in the other file.
  // only rows in the other file that have matching values in this
  // file are merged
  void merge(CSV csv, std::string column);

  // check existence of a column
  bool hasColumn(std::string name) const {
    return std::find(std::begin(columns_), std::end(columns_), name) !=
           std::end(columns_);
  }
};

