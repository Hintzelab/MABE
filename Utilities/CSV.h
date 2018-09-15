
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
// The delimiter and quotation character can be specified:
// by default  , and "

class CSVReader {

  char delimiter_ = ',', quotation_ = '"';
  std::string current_string_;
  std::vector<std::string> fields_;
  enum class input { chars, delim, quote, wh_sp };
  enum class state { precw, field, delim, quote, succw, openq, CRASH };

  std::array<std::array<state, 4>, 6> Transition{{
      //  chars  		delim   		quote  		wh_sp
      {state::field, state::delim, state::quote, state::precw}, // precw
      {state::field, state::delim, state::succw, state::succw}, // field
      {state::field, state::delim, state::quote, state::precw}, // delim
      {state::openq, state::openq, state::succw, state::openq}, // openq
      {state::CRASH, state::delim, state::CRASH, state::succw}, // succw
      {state::openq, state::openq, state::succw, state::openq}  // nestq
      //{state::CRASH ,  state::CRASH ,  state:CRASH: , state::CRASH } KABOOM
  }};

  auto symbol(char);
  auto doStateAction(state, char);

public:
  CSVReader(char d) : delimiter_(d) {}
  CSVReader(char d, char oq) : delimiter_(d), quotation_(oq) {}
  auto parseLine(const std::string &);
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
    return std::find(std::begin(columns_), std::end(columns_), name) !=
           std::end(columns_);
  }
};

