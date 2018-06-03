
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

  auto current_sep = 0u;
  while (true) {
    auto next_sep = raw_line.find_first_of(separator_, current_sep + 1);
   //   std::cout << " next_sep:" << next_sep;
	if (next_sep == std::string::npos) {
          stringToValue(raw_line.substr(current_sep), value);
       //   std::cout << value << std::endl;
      data_line.push_back(value);
	  break;
	}	
	auto next_sep_exp = raw_line.find_first_of(sep_except_, current_sep );
    //  std::cout << " next_sep_exp:" << next_sep_exp;
    if (next_sep < next_sep_exp) {
      stringToValue(raw_line.substr(current_sep, next_sep - current_sep),
                    value);
	  //std::cout << value << std::endl;
      data_line.push_back(value);
      current_sep = next_sep+1;
    } else {
      auto next_next_sep_exp =
          raw_line.find_first_of(sep_except_, next_sep_exp + 1);
  //    std::cout << " next_next_sep_exp:" << next_next_sep_exp;
      auto next_next_sep =
          raw_line.find_first_of(sep_except_, next_next_sep_exp );
//	  std::cout << " next_next_sep:" << next_next_sep ;
      stringToValue(raw_line.substr(current_sep, next_next_sep - current_sep +1),
                    value);
	 // std::cout << value << std::endl;
      data_line.push_back(value);
      current_sep = next_next_sep+2;
    }
//	std::cout << " value: " << value << std::endl;
  }
  
  /*
  for (auto &m : forEachRegexMatch(raw_line, item_)) {
		std::cout << quoted_string  << "#" << in_quotes<< std::endl;
    if (m[2].str() == sep_except_) {
      if (!in_quotes) {
        stringToValue(m[1].str(), value);
        data_line.push_back(value);
        in_quotes = true;
      } else {
        quoted_string += m[1].str();
        stringToValue(quoted_string, value);
        data_line.push_back("\"" + value + "\"");
        quoted_string.clear();
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
  */
 /*
  data_line.erase(std::remove_if(std::begin(data_line), std::end(data_line),
                                 [](std::string s) { return s.empty(); }),
                  std::end(data_line));
*/
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
  //for (auto &i:columns_) std::cout << i << "#";
//	std::cout << columns_.size() << std::endl;
  std::set<std::string> uniq_headers(std::begin(columns_), std::end(columns_));
  if (uniq_headers.size() != columns_.size()) {
    std::cout << "Error: CSV file " << file_name_
              << " does not have unique Header names" << std::endl;
    exit(1);
  }

  while (getline(file, raw_line)) {
    auto data_line = reader_.parseLine(raw_line);
 // for (auto &i:data_line) std::cout << i << "#"; 
//	std::cout << data_line.size() << std::endl;
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

