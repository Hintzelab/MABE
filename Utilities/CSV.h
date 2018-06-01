
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

	public:

  std::vector<std::string> parseCSVLine(std::string raw_line,
                                        const char separator = ',',
                                        const char sep_except = '"');
std::map<std::string, std::vector<std::string>>
readColumnsFromCSVFile(const std::string &file_name, const char separator = ',',
                       const char sep_except = '"') ;
std::string
CSVLookUp(std::map<std::string, std::vector<std::string>> csv_table,
          const std::string &lookup_key, const std::string &lookup_value,
          const std::string &return_key) ;
};

