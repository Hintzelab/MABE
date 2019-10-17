
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
#include <array>

// can parse a csv string into a vector of strings
// The delimiter and quotation character can be specified:
// by default  , and "
// works by using Transition to manage state transitions 
// (the state-to-state transition table) and appending 
// characters to strings as needed.

class CSVReader {

  char delimiter_ = ',', quotation_ = '"';
  std::string current_string_;
  std::vector<std::string> fields_;
  enum class input { chars, delim, quote, wh_sp };
  enum class state {
                      precw, // leading whitespace
                      field, // non quoted field
                      delim, // found delim 
                      quote, // reading quoted field
                      succw, // trailing whitespace 
                      openq, // first quote of quoted field
                      CRASH
                    };

  // cast state to int to index into more
  // readable state-to-state transition table
  std::array<std::array<state, 4>, 6> Transition = {{
  //  chars          delim         quote         wh_sp
      {state::field, state::delim, state::quote, state::precw}, // precw 0 
      {state::field, state::delim, state::succw, state::succw}, // field 1
      {state::field, state::delim, state::quote, state::precw}, // delim 2
      {state::openq, state::openq, state::succw, state::openq}, // quote 3
      {state::CRASH, state::delim, state::CRASH, state::succw}, // succw 4
      {state::openq, state::openq, state::succw, state::openq}  // openq 5
    //{state::CRASH, state::CRASH, state::CRASH, state::CRASH}  // CRASH 6
  }};

  CSVReader::input symbol(char);
  void doStateAction(state, char, const std::string&, const int&);
  void showLineAndErrorChar(const std::string&, const int&);

public:
  CSVReader() = default ;
  CSVReader(char d) : delimiter_(d) {}
  CSVReader(char d, char oq) : delimiter_(d), quotation_(oq) {}
  std::vector<std::string> parseLine(const std::string &);
};

// parses a csv file and stores in memory.
// the first line of the file is treated as the column headers
// The delimiter and quotation character can be specified:
// by default  , and "
class CSV {

  // to do the reading
  CSVReader reader_;
  // the 1st row; column headers
  std::vector<std::string> column_names_;
  // remaining rows of the file
  std::vector<std::vector<std::string>> rows_;

  std::string file_name_;

public:
  CSV(std::string fn, char s, char se);
  CSV(std::string fn) : CSV(fn, ',', '"') {}

  // return csv file name
  std::string fileName() const { return file_name_; }

  // number of columns in the file
  auto column_count() const { return column_names_.size(); }

  // number of rows in the file
  auto row_count() const { return rows_.size(); }

  // return all columns in the file
  std::vector<std::string> column_names() const { return column_names_; }

  // return all rows in the file
  std::vector<std::vector<std::string>> rows() const { return rows_; }

  // return all values corresponding to a single column
  std::vector<std::string> singleColumn(std::string column);

  // look up a value in a column and return the value in the corresponding row
  // of the other column
  std::string lookUp(std::string lookup_column, std::string value,
                     std::string return_column) const;

  // merges another csv file. Only extra columns are added. All values in the
  // specified column must also exist in the same column in the other file.
  // only rows in the other file that have matching values in this
  // file are merged
  void merge(CSV csv, std::string column);

  // check existence of a column
  bool hasColumn(std::string name) const {
    return std::find(std::begin(column_names_), std::end(column_names_), name) !=
           std::end(column_names_);
  }
};

