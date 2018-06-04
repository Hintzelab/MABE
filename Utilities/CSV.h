
#pragma once

#include "Utilities.h"

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
  std::regex item_;

public:
  CSVReader() : CSVReader(',', '"') {}
  CSVReader(char s, char se)
      : delimiter_(1, s), quotation_(1, se),
        item_(R"((.*?)()" + delimiter_ + "|" + quotation_ + R"(|$))") {}

  template <typename T = std::string>
  auto parseLine(std::string &raw_line) const;
};

// parses a csv file and stores in memory.
// the first line of the file is treated as the column headers
// The delimiter and quotation character can be spedified:
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

  // look up a value in a colmn and return the value in the corresponding row of
  // the other column
  auto lookUp(std::string lookup_column, std::string value,
              std::string return_column) const;

  // number of rows in the file
  auto row_count() const { return rows_.size(); }

  // number of columns in the file
  auto columns() const { return columns_; }

  // return all values corresponding to a single column
  auto singleColumn(std::string);

  // return all rows in the file
  auto rows() const { return rows_; }

  // merges another csv file. Only extra columns are updated. All values in the
  // specified column must also exist in the same column in the other file.
  // only rows in the other file that have matching values in this
  // file are merged
  void merge(CSV csv, std::string column);

  // return csv file name
  auto fileName() const { return file_name_; }

  // check existence of a column
  bool hasColumn(std::string name) const {
    return std::find(std::begin(columns_), std::end(columns_), name) !=
           std::end(columns_);
  }
};

// parse a csv line into a vector<T>
template <typename T> auto CSVReader::parseLine(std::string &raw_line) const {
  std::vector<T> data;
  auto current = 0u;

  while (true) {
    // find next delimiter
    auto delim = raw_line.find_first_of(delimiter_, current + 1);
    // if line is completely parsed
    if (delim == std::string::npos) {
      data.push_back(stringTo<T>(raw_line.substr(current)));
      break;
    }
    // find next quotation
    auto quote = raw_line.find_first_of(quotation_, current);
    // if quotation is not in the way
    if (delim < quote) {
      data.push_back(stringTo<T>(raw_line.substr(current, delim - current)));
      current = delim + 1;
    } else {
      // if quotation is in the way, find next quote
      auto nested_quote = raw_line.find_first_of(quotation_, quote + 1);
      // find first delimiter after that; ignore delimiter inside of quotation
      auto nested_delim = raw_line.find_first_of(quotation_, nested_quote);
      data.push_back(
          stringTo<T>(raw_line.substr(current, nested_delim - current + 1)));
      current = nested_delim + 2;
    }
  }

  return data;
}

auto CSV::singleColumn(std::string column) {
  if (!hasColumn(column)) {
    std::cout << " Error : could not find column " << column
              << " to merge from file " << file_name_ << std::endl;
    exit(1);
  }

  auto const column_pos =
      std::find(std::begin(columns_), std::end(columns_), column) -
      std::begin(columns_);

  std::vector<std::string> values;
  std::transform(std::begin(rows_), std::end(rows_), std::back_inserter(values),
                 [column_pos](auto const &row) { return row[column_pos]; });

  return values;
}

auto CSV::lookUp(std::string lookup_column, std::string value,
                 std::string return_column) const {

  if (!hasColumn(lookup_column)) {
    std::cout << " Error : could not find column " << lookup_column
              << " in file " << file_name_ << std::endl;
    exit(1);
  }

  if (!hasColumn(return_column)) {
    std::cout << " Error : could not find column " << return_column
              << " in file " << file_name_ << std::endl;
    exit(1);
  }

  // find position of lookup column
  auto const column_pos =
      std::find(std::begin(columns_), std::end(columns_), lookup_column) -
      std::begin(columns_);

  // find number of values in lookup column
  auto const value_count = std::count_if(
      std::begin(rows_), std::end(rows_),
      [value, column_pos](auto &row) { return row[column_pos] == value; });

  if (!value_count) {
    std::cout << "Error : could not find requested lookup value" << value
              << " from column " << lookup_column << " from file " << file_name_
              << std::endl;
    exit(1);
  }

  if (value_count > 1) {
    std::cout << "Error : multiple entries found for requested lookup value"
              << value << " from column " << lookup_column << " from file "
              << file_name_ << std::endl;
    exit(1);
  }

  // find row where lookup column has value
  auto const value_pos = std::find_if(std::begin(rows_), std::end(rows_),
                                      [value, column_pos](auto &row) {
                                        return row[column_pos] == value;
                                      }) -
                         std::begin(rows_);

  // find position of return column
  auto const return_pos =
      std::find(std::begin(columns_), std::end(columns_), return_column) -
      std::begin(columns_);

  return rows_[value_pos][return_pos];
}

CSV::CSV(std::string fn, char s, char se) : file_name_(fn), reader_(s, se) {

  std::ifstream file(file_name_);
  if (!file.is_open()) {
    std::cout << " Error: readColumnsFromCSVFile cannot open file "
              << file_name_ << std::endl;
    exit(1);
  }

  std::string raw_line;

  // read header line
  getline(file, raw_line);
  columns_ = reader_.parseLine(raw_line);

  // ensure column names are unique
  std::set<std::string> uniq_headers(std::begin(columns_), std::end(columns_));
  if (uniq_headers.size() != columns_.size()) {
    std::cout << "Error: CSV file " << file_name_
              << " does not have unique Header names" << std::endl;
    exit(1);
  }

  // read remaining rows
  while (getline(file, raw_line)) {
    auto data_line = reader_.parseLine(raw_line);
    // ensure all rows have correct number of columns
    if (columns_.size() != data_line.size()) {
      std::cout << " Error: incorrect number of columns in CSV file "
                << file_name_ << std::endl;
      exit(1);
    }
    rows_.push_back(data_line);
  }
}

void CSV::merge(CSV merge_csv, std::string column) {

  if (!hasColumn(column)) {
    std::cout << " Error : could not find column " << column << " in file "
              << file_name_ << std::endl;
    exit(1);
  }

  if (!merge_csv.hasColumn(column)) {
    std::cout << " Error : could not find column " << column
              << " to merge from file " << merge_csv.fileName() << std::endl;
    exit(1);
  }

  // ensure lookup column has distinct values
  auto lookop_column = singleColumn(column);
  std::set<std::string> lookup_values(std::begin(lookop_column),
                                      std::end(lookop_column));
  if (lookup_values.size() != rows_.size()) {
    std::cout << "Error: CSV file " << file_name_
              << " does not have unique values in column " << column
              << std::endl;
    exit(1);
  }

  // ensure column in second file has matching values for all values in this
  // file
  auto merge_column = merge_csv.singleColumn(column);
  std::set<std::string> merge_values(std::begin(merge_column),
                                     std::end(merge_column));
  if (!std::includes(std::begin(merge_values), std::end(merge_values),
                     std::begin(lookup_values), std::end(lookup_values))) {
    std::cout << "Error: CSV file " << merge_csv.fileName()
              << " does not have some matching values for column " << column
              << std::endl;
    exit(1);
  }

  // find position of lookup column
  auto const column_pos =
      std::find(std::begin(columns_), std::end(columns_), column) -
      std::begin(columns_);

  // merge columns from second file
  for (auto const &merge_column : merge_csv.columns()) {
    // only add additional columns
    if (std::find(std::begin(columns_), std::end(columns_), merge_column) ==
        std::end(columns_)) {
      columns_.push_back(merge_column);
      // for each column add value to every row
      for (auto &row : rows_)
        row.push_back(merge_csv.lookUp(column, row[column_pos], merge_column));
    }
  }
}



