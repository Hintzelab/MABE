// author : cgnitash
// Loader.cpp contains implementation of population loading scripting language

#include "Loader.h"
#include "Utilities.h"
#include "CSV.h"


#include "zupply.h" // for x-platform filesystem

#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

auto  Loader::cleanLines_(std::ifstream &flines) {
  // read lines, strip out all trailing comments, and create one long string.
  std::string all_lines = " ", line;
  static const std::regex comments("#.*");
  while (getline(flines, line)) {
    if (!line.empty()) {
      std::string clean_line = std::regex_replace(line, comments, "");
      all_lines += " " + clean_line;
    }
  }

  static const std::regex garbage(R"(^\s*\w+\s*=.*$)");
  // check that file must start with an assignment
  if (!std::regex_match(all_lines, garbage)) {
    std::cout << " error : Loader file contains unrecognized text at beginning "
              << std::endl;
    exit(1);
  }
  return all_lines;
}


auto Loader::fromPLFfile_(std::string loader_file_name) {
  std::ifstream flines(loader_file_name);
  if (!flines.is_open()) {
    std::cout << " error: population loader file " << loader_file_name
              << " does not exist " << std::endl
              << " Run MABE with -l to generate population loader file"
              << std::endl;
    exit(1);
  }

  std::cout << "Creating population from " << loader_file_name << "\n";

  return cleanLines_(flines);
}


void Loader::showFinalPopulation_(std::vector<long> orgs) {
  std::cout << "\n"
            << "Loading "
            << std::accumulate(std::begin(orgs), std::end(orgs), 0,
                               [](long acc, long i) { return acc + (i == -1); })
            << " Random organisms\n";
  std::cout << "Loading "
            << std::accumulate(std::begin(orgs), std::end(orgs), 0,
                               [](long acc, long i) { return acc + (i == -2); })
            << " Default organisms\n";
  std::map<std::string, std::vector<long>> orgs_from_files;
  for (auto i : orgs)
    if (i > -1)
      orgs_from_files[all_organisms_[i].from_file].push_back(
          all_organisms_[i].orig_ID);
  for (auto &f : orgs_from_files) {
    std::cout << "From file " << f.first << "\n  Loading organisms with IDs : ";
    for (auto k : f.second)
      std::cout << k << " ";
    std::cout << "\n";
  }
}

bool Loader::balancedBraces_(std::string s) {
  long k = 0;
  for (auto &c : s)
    switch (c) {
    case '{':
      k++;
      break;
    case '}':
      k--;
      if (k < 0)
        return false;
      break;
    }
  return k == 0;
}

auto Loader::keywordDuplicate_(size_t value, std::string resource) {

  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(resource) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists_[resource]) {
    const auto &from_pop = p;
    auto num = value;
    while (num--)
      coll.push_back(from_pop);
  }
  return coll;
}

auto Loader::keywordMatch_(std::string attribute,
                                                     std::string value,
                                                     std::string resource) {

  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(resource) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists_.at(resource)) {
    const auto from_pop = p;
    for (const auto &o : from_pop)
      if (all_organisms_.at(o).attributes.find(attribute) ==
          all_organisms_.at(o).attributes.end()) {
        std::cout << "error: while trying to match " << value << "  "
                  << resource << " contains organisms without attribute "
                  << attribute << std::endl;
        exit(1);
      }
    std::vector<long> pop;
    std::copy_if(std::begin(from_pop), std::end(from_pop),
                 std::back_inserter(pop), [&](long index) {
                   return all_organisms_.at(index).attributes.at(attribute) ==
                          value;
                 });
    coll.push_back(pop);
  }
  return coll;
}

auto
Loader::keywordGreatest_(size_t number, std::string attribute,
                        std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(resource) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists_.at(resource)) {
    const auto from_pop = p;
    if (from_pop.size() < number) {
      std::cout << "error: trying to get greatest " << number
                << " from collection, but  " << resource
                << " contains a population that does not have sufficient "
                << " organisms" << std::endl;
      exit(1);
    }
    for (const auto &o : from_pop)
      if (all_organisms_.at(o).attributes.find(attribute) ==
          all_organisms_.at(o).attributes.end()) {
        std::cout << "error:  trying to get greatest " << number
                  << " from collection, but  " << resource
                  << " contains organisms without attribute " << attribute
                  << std::endl;
        exit(1);
      }
    std::vector<long> pop(number);
    std::partial_sort_copy(
        from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organisms_.at(lhs).attributes.at(attribute)) >
                 std::stod(all_organisms_.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

auto Loader::keywordLeast_(size_t number,
                                                     std::string attribute,
                                                     std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(resource) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists_.at(resource)) {
    const auto from_pop = p;
    if (from_pop.size() < number) {
      std::cout << "error: trying to get least" << number
                << " from collection, but  " << resource
                << " contains a population that does not have sufficient "
                << " organisms" << std::endl;
      exit(1);
    }
    for (const auto &o : from_pop)
      if (all_organisms_.at(o).attributes.find(attribute) ==
          all_organisms_.at(o).attributes.end()) {
        std::cout << "error:  trying to get least" << number
                  << " from collection, but  " << resource
                  << " contains organisms without attribute " << attribute
                  << std::endl;
        exit(1);
      }
    std::vector<long> pop(number);
    std::partial_sort_copy(
        from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organisms_.at(lhs).attributes.at(attribute)) <
                 std::stod(all_organisms_.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

auto Loader::keywordAny_(size_t number,
                                                  std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(resource) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists_.at(resource)) {
    auto from_pop = p;
    if (from_pop.size() < number) {
      std::cout << "error: trying to get any" << number
                << " from collection, but  " << resource
                << " contains a population that does not have sufficient "
                << " organisms" << std::endl;
      exit(1);
    }
	// Latent bug - not reproducible randomness :(
    std::shuffle(from_pop.begin(), from_pop.end(), std::random_device());
    std::vector<long> pop(from_pop.begin(), from_pop.begin() + number);
    coll.push_back(pop);
  }
  return coll;
}

auto
Loader::keywordCollapse_(std::string name) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists_.find(name) == collection_org_lists_.end()) {
    std::cout << "Unrecognised token " << name << std::endl;
    exit(1);
  }
  std::vector<long> pop;
  for (const auto &c : collection_org_lists_.at(name)) {
    pop.insert(pop.end(), c.begin(), c.end());
  }
  coll.push_back(pop);
  return coll;
}

auto Loader::keywordRandom_(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -1);
  coll.push_back(pop);
  return coll;
}

auto Loader::keywordDefault_(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -2);
  coll.push_back(pop);
  return coll;
}

// semantics of collection choosing
auto Loader::parseCollection_(std::string expr) {

  {
    static const std::regex command_collapse(R"(^\s*collapse\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_collapse))
      return keywordCollapse_(match[1].str());
  }

  {
    static const std::regex command_random(R"(^\s*random\s+(\d+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_random))
      return keywordRandom_(stol(match[1].str()));
  }

  {
    static const std::regex command_default(R"(^\s*default\s+(\d+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_default))
      return keywordDefault_(stol(match[1].str()));
  }

  {
    static const std::regex command_greatest(
        R"(^\s*greatest\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_greatest))
      return keywordGreatest_(stoul(match[1].str()), match[2].str(),
                             match[3].str());
  }

  {
    static const std::regex command_least(
        R"(^\s*least\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_least))
      return keywordLeast_(stol(match[1].str()), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_any(
        R"(^\s*any\s+(\d+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_any))
      return keywordAny_(stol(match[1].str()), match[2].str());
  }

  {
    static const std::regex command_match(
        R"(^\s*match\s+(\w+)\s+where\s+(\S+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_match))
      return keywordMatch_(match[1].str(), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_duplicate(
        R"(^\s*(\d+)\s*\*\s*(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_duplicate))
      return keywordDuplicate_(stoul(match[1].str()), match[2].str());
  }

  // if no keyword matches
  std::cout << " error: syntax error while trying to resolve " << std::endl
            << expr << std::endl;
  exit(1);

} // end Loader::parseCollection_

auto Loader::parseExpression_(std::string expr) {

  if (!balancedBraces_(expr)) {
    std::cout << " Expression " << expr << " does not have balanced braces "
              << std::endl;
    exit(1);
  }

  expr = "{" + expr + "}"; // makes the top of local stack evaluate to the
                           // correct expression

  std::vector<std::pair<std::string, std::string>> local_tk_stack;
  static const std::regex braces(R"(\{([^\{\}]*)\})");
  std::smatch m;
  while (std::regex_search(expr, m, braces)) { // resolve braced by ...
    std::string new_tk =
        tk_name_ + std::to_string(tk_counter_++); // creating new token
    local_tk_stack.emplace_back(new_tk, m[1].str()); // adding token to local stack of tokens
    expr = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
    // replacing brace sub-expr with token
  } // repeat for all braces ..

  // evaluate tokens in local stack
  static const std::regex colon_sep(R"((.+?)(?:\:|$))");
  for (const auto &tk : local_tk_stack) {
    std::vector<std::vector<long>> coll;
    std::string tkn = tk.second;
    for (std::sregex_iterator end,
         i = std::sregex_iterator(tkn.begin(), tkn.end(), colon_sep);
         i != end; i++) { //	evaluate each sub expression
      std::smatch sub_expr = *i;
      std::string token = sub_expr[1].str();
      token.erase(std::remove_if(token.begin(), token.end(), ::isspace),
                  token.end());
      auto p = collection_org_lists_.find(token) != collection_org_lists_.end()
                   ? collection_org_lists_[token]
                   : // expression is already evaluated collection
                   parseCollection_(
                       sub_expr[1].str()); // expression needs to be parsed

      coll.insert(coll.end(), p.begin(), p.end());
    }
    collection_org_lists_[tk.first] =
        coll; // add population to global collections
  }

  // return population  at top of stack
  return collection_org_lists_[local_tk_stack.back().first];
} // end  Loader::parseExpression


void Loader::parseAllCommands_(std::string all_lines) {
  // split into commands. These are all collection-assignments
  static const std::regex command(
      R"((?:(\w+)\s*=)(.*?)(?=(?:(?:\w+\s*=))|$))");
  // match one of the collection-assignments but don't capture. lookahead for
  // another collection-assignment (without capturing) or end of line
  for (auto &m : forEachRegexMatch(all_lines, command)) {
    auto expr = m[2].str();
    auto name = m[1].str();
    collection_org_lists_[name] = parseExpression_(expr);
  }
}

auto Loader::expandFiles_(std::string f) {

  std::vector<std::string> result;
  static const std::regex wildcard(R"(\*)");
  std::string file_name = std::regex_replace(f, wildcard, R"([^/]*)");
  static const std::regex valid_path_names("^" + file_name + "$");
  static const std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");

  std::copy_if(all_possible_file_names_.begin(), all_possible_file_names_.end(),
               std::back_inserter(result), [](const std::string &s) {
                 return std::regex_match(s, valid_path_names) &&
                        std::regex_match(s, valid_org_name);
               });

  if (result.empty()) {
    std::cout << " error: " << f << " does not match any files" << std::endl;
    exit(1);
  }
  return result;
} // end Loader::expandFiles

auto Loader::generatePopulation_(std::string file_name) {
  auto org_file_data = CSV(file_name);
  auto file_contents_pair =
      std::make_pair(long(all_organisms_.size()), long(org_file_data.row_count()));

  static const std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");
  std::smatch match_org;
  std::regex_match(file_name, match_org, valid_org_name);
  auto data_file_name = std::regex_replace(file_name, valid_org_name,
                                           match_org[1].str() + "_data" +
                                               match_org[2].str() + ".csv");

  if (std::find(all_possible_file_names_.begin(), all_possible_file_names_.end(),
                data_file_name) != all_possible_file_names_.end()) {
  auto  data_file_data = CSV(data_file_name);
  org_file_data.merge(data_file_data,"ID");
  }

  for (auto const &id : org_file_data.singleColumn("ID")) {
    Organism_ org;
    org.orig_ID = std::stol(id);
    org.from_file = file_name;
    for (const auto &attribute : org_file_data.columns()) {
      org.attributes.insert(
          make_pair(attribute, org_file_data.lookUp("ID", id, attribute)));
    }
    all_organisms_.push_back(org);
  }

  return file_contents_pair;
} // end Loader::generatePopulation

auto Loader::findAndGenerateAllFiles_(std::string all_lines) {

  zz::fs::Directory mabe_org_dir("./", "*organisms*.csv", true); // true=recursive
  for (auto const &p : mabe_org_dir) {
  	  all_possible_file_names_.push_back(p.relative_path());
  }
  
  zz::fs::Directory mabe_data_dir("./", "*data*.csv", true); // true=recursive
  for (auto const &p : mabe_data_dir) {
  	  all_possible_file_names_.push_back(p.relative_path());
  }

  std::map<std::string, std::vector<std::string>>
      collection_of_files;            // all file names for a collection
  std::set<std::string> actual_files; // every file the user might refer to

  // get all file names . These must all be in single-qoutes and can be
  // wildcarded
  static const std::regex quoted_files("'([^']*)'");
  std::smatch m;
  while (std::regex_search(
      all_lines, m,
      quoted_files)) { // replace quoted file names with temporary token name
    std::string new_tk =
        tk_name_ + std::to_string(tk_counter_++); // creating new token
    all_lines = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
    auto exp_files = expandFiles_(m[1].str()); // expand the filename
    collection_of_files[new_tk] =
        exp_files; //  update list of files assoc with collection name
    actual_files.insert(exp_files.begin(),
                        exp_files.end()); // add files to global file list
  }

  std::map<std::string, std::pair<long, long>> file_contents;
  // filename -> (start index of org in all_organisms, number of sequential
  // organisms from start)

  for (const auto &file :
       actual_files) { // load all populations from file ONCE.
    // this creates a SINGLE list of organisms that can be efficiently accessed
    // should be done in parallel
    file_contents[file] = generatePopulation_(file);
  }

  // create all collections from file names
  for (const auto &cf : collection_of_files) {
    for (const auto &f : cf.second) {
      std::vector<long> sinf(file_contents[f].second);
      std::iota(sinf.begin(), sinf.end(), file_contents[f].first);
      collection_org_lists_[cf.first].push_back(sinf);
    }
  }
  return all_lines;
}

std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
Loader::loadPopulation(std::string loader_option) {

  static const std::regex plf_file(R"(.*\.plf)");

  auto all_lines = std::regex_match(loader_option, plf_file)
                       ? fromPLFfile_(loader_option)
                       : loader_option;

  all_lines = findAndGenerateAllFiles_(all_lines);

  parseAllCommands_(all_lines);

  if (collection_org_lists_.find("MASTER") == std::end(collection_org_lists_)) {
    std::cout << "error: Must load from variable named MASTER" << std::endl;
    exit(1);
  }
  if (collection_org_lists_["MASTER"].size() != 1) {
    std::cout << "error: variable named MASTER must contain exactly one population"
              << std::endl;
    exit(1);
  }

  std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
      final_population; // needs to be constructed

  auto final_orgs = collection_org_lists_["MASTER"][0];

  showFinalPopulation_(final_orgs);

  std::transform(
      std::begin(final_orgs), std::end(final_orgs),
      std::back_inserter(final_population), [this](long org) {
        return org < 0
                   ? make_pair(org,
                               std::unordered_map<std::string, std::string>())
                   : make_pair(org, all_organisms_[org].attributes);
      });

  return final_population;
} // end Loader::loadPopulation

/*
// reads organisms or data file. return key of ID to map of attributes to values
// attributes do NOT include ID
std::map<long, std::map<std::string, std::string>>
Loader::getAttributeMap_(std::string file_name) {

  std::map<long, std::map<std::string, std::string>> result;

  // check if organsims or data file
  static const std::regex org_or_data(R"((.*)_(data|organisms)(_\d+)?.csv$)");
  std::smatch match_org;
  if (!std::regex_match(file_name, match_org, org_or_data)) {
    std::cout << " error: unrecognized file name format " << file_name
              << std::endl
              << " Was this file generated by MABE? " << std::endl;
    exit(1);
  }
  std::ifstream file(file_name);
  if (!file.is_open()) {
    std::cout << " error: unable to load" << file_name << std::endl;
    exit(1);
  }

  std::string attr_names;
  getline(file, attr_names);
  static const std::regex each_attribute(R"([\w|:]+)");
  std::vector<std::string> attribute_names;

  for (std::sregex_iterator end,
       i = std::sregex_iterator(attr_names.begin(), attr_names.end(),
                                each_attribute);
       i != end; i++) {
    attribute_names.push_back((*i).str());
  }

  if (std::find(attribute_names.begin(), attribute_names.end(), "ID") ==
      attribute_names.end()) {
    std::cout << " error: no ID for organisms in file " << file_name
              << std::endl;
    exit(1);
  }

  // checking for MABE csv-ness
  static const std::regex mabe_csv_regex(
      R"((([-\.\d]+)(?:,|$))|("\[)|(([-\.\d]+\]")(?:,|$)))");
  //	std::regex mabe_csv_regex(R"(("[^"]+"|[^,]+)(,|$))");  // does not work
  //because of
  //	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61582
  std::string org_details;
  while (getline(file, org_details)) {
    std::map<std::string, std::string> temp_result;
    long k = 0;
    auto in_quotes = false;
    std::string quote_str;
    for (std::sregex_iterator end,
         i = std::sregex_iterator(org_details.begin(), org_details.end(),
                                  mabe_csv_regex);
         i != end; i++) {
      std::smatch m = *i;

      if (m[1].length())
        if (in_quotes == false)
          temp_result[attribute_names.at(k++)] = m[2].str();
        else
          quote_str += m[1].str();
      else if (m[3].length()) {
        in_quotes = true;
        quote_str += m[3].str();
      } else if (m[5].length()) {
        quote_str += m[5].str();
        temp_result[attribute_names.at(k++)] = quote_str;
        in_quotes = false;
        quote_str = "";
      } else {
        std::cout << " error : something wrong with mabe csv-ness "
                  << std::endl;
        exit(1);
      }
    }
    auto orig_ID = std::stol(temp_result.at("ID"));
    temp_result.erase("ID");
    result[orig_ID] = temp_result;
  }
  file.close();
  return result;
} // end Loader::getAttributeMap
*/


