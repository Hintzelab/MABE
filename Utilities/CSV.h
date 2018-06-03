
#pragma once 

#include "Utilities.h"

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

class CSVReader {

  std::string separator_, sep_except_;	
  std::regex item_;

	public:
  CSVReader() : CSVReader(',','"') {}
  CSVReader(char s, char se)
      : separator_(1, s), sep_except_(1, se),
        item_(R"((.*?)()" + separator_ + "|" + sep_except_ + R"(|$))") {}

  template <typename T = std::string>
  auto parseLine(std::string &raw_line) const;
};

class CSV {

  CSVReader reader_;
  std::vector<std::vector<std::string>> rows_;
  std::vector<std::string> columns_;
  std::string file_name_;

public:

  CSV(std::string fn, char s, char se);
  CSV(std::string fn) : CSV(fn, ',', '"') {}

  auto  lookUp(std::string lookup_column, std::string value,
                     std::string return_column)const;

  auto row_count() const{ return rows_.size(); }

  auto columns() const{ return columns_; }
 
  auto singleColumn(std::string);

  auto rows() const{ return rows_; }

  void merge(CSV csv, std::string column);

  auto fileName() const{ return file_name_; }

  bool hasColumn(std::string name) const {
    return std::find(std::begin(columns_), std::end(columns_), name) !=
           std::end(columns_);
  }
};

template <typename T> auto CSVReader::parseLine(std::string &raw_line) const {
  std::vector<T> data_line;
  T value;
  bool in_quotes = false;
  std::string quoted_string;
  for (auto &m : forEachRegexMatch(raw_line, item_)) {
    if (m[2].str() == sep_except_) {
      if (!in_quotes) {
        stringToValue(m[1].str(), value);
        data_line.push_back(value);
        in_quotes = true;
      } else {
        quoted_string += m[1].str();
        stringToValue(quoted_string, value);
        data_line.push_back(value);
        quoted_string = "";
        in_quotes = false;
      }
    } else {
      if (!in_quotes) {
        stringToValue(m[1].str(), value);
        data_line.push_back(value);
      } else {
        quoted_string += m[0].str();
      }
    }
  }
  data_line.erase(std::remove_if(data_line.begin(), data_line.end(),
                                 [](std::string s) { return s == ""; }),
                  data_line.end());

  return data_line;
}

auto CSV::singleColumn(std::string column) {
  if (!hasColumn(column)) {
    std::cout << " Error : could not find column " << column
              << " to merge from file " << file_name_<< std::endl;
    exit(1);
  }

  auto const column_pos =
      std::find(std::begin(columns_), std::end(columns_), column) -
      std::begin(columns_);

  std::vector<std::string> values;
  std::transform(std::begin(rows_), std::end(rows_),
                 std::back_inserter(values),
                 [column_pos](auto const &row) { return row[column_pos]; });

  return values;
}

auto CSV::lookUp(std::string lookup_column, std::string value,
                 std::string return_column)const {

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
  getline(file, raw_line);
  columns_ = reader_.parseLine(raw_line);

  std::set<std::string> uniq_headers(std::begin(columns_), std::end(columns_));
  if (uniq_headers.size() != columns_.size()) {
    std::cout << "Error: CSV file " << file_name_
              << " does not have unique Header names" << std::endl;
    exit(1);
  }

  while (getline(file, raw_line)) {
    auto data_line = reader_.parseLine(raw_line);
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

  auto const column_pos =
      std::find(std::begin(columns_), std::end(columns_), column) -
      std::begin(columns_);

  std::set<std::string> uniq_values;
  std::transform(std::begin(rows_), std::end(rows_),
                 std::inserter(uniq_values, std::begin(uniq_values)),
                 [column_pos](auto const &row) { return row[column_pos]; });

  if (uniq_values.size() != rows_.size()) {
    std::cout << "Error: CSV file " << file_name_
              << " does not have unique values in column " << column
              << std::endl;
    exit(1);
  }

  for (auto const &merge_column : merge_csv.columns()) {
    if (std::find(std::begin(columns_), std::end(columns_), merge_column) !=
        std::end(columns_)) {
      columns_.push_back(merge_column);
      for (auto &row : rows_)
        row.push_back(merge_csv.lookUp(column, row[column_pos], merge_column));
    }
  }
}

