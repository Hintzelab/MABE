
#include"CSV.h"
#include"Utilities.h"


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

// converts a vector of string to a vector of type of returnData
template <class T>
inline void CSVListToVector(std::string string_data,
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
// extract a value from a map<string,vector<string>>
// given a value from one vector, return the value in another vector at the same
// index
std::string
CSV::CSVLookUp(std::map<std::string, std::vector<std::string>> csv_table,
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
std::map<std::string, std::vector<std::string>>
CSV::readColumnsFromCSVFile(const std::string &file_name, const char separator , 
                       const char sep_except ) {
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

std::vector<std::string> CSV::parseCSVLine(std::string raw_line,
                                             const char separator,
                                             const char sep_except) {
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
