
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

class CSV {

  std::vector<std::vector<std::string>> data_;
  std::vector<std::string> headers_;
  std::string from_file_;
  char separator_, sep_expect_;	

public:
  auto parseCSVLine(std::string raw_line, const char separator = ',',
                    const char sep_except = '"');

  CSV(std::string file_name, char s, char se);
  CSV(std::string file_name) : CSV(file_name, ',', '"') {}

  std::string
  CSVLookUp(std::map<std::string, std::vector<std::string>> csv_table,
            const std::string &lookup_key, const std::string &lookup_value,
            const std::string &return_key);

  std::map<long, std::map<std::string, std::string>>
  getAttributeMapByID(const std::string &file_name);

};

