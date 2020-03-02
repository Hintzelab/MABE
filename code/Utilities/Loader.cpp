// author : cgnitash, joryschossau
// Loader.cpp contains implementation of population loading scripting language

#include "Loader.h"
#include "Filesystem.h"
#include "CSV.h"

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
#include <utility>

std::string dataVersionOfFilename(const std::string& filename) {
    std::string ext(filename.substr(filename.rfind(".")));
    int orgposIfExist = filename.rfind("organisms");
    if (orgposIfExist != -1) return filename.substr(0,orgposIfExist)+"data"+filename.substr(orgposIfExist+9);
    else return filename.substr(0,filename.rfind("."))+"_data"+ext;
}


std::string Loader::loadFromFile(const std::string &loader_file_name) {
  std::ifstream flines(loader_file_name);
  if (!flines.is_open()) {
    std::cout << " error: population loader file " << loader_file_name
              << " does not exist " << std::endl
              << " Run MABE with -l to generate population loader file"
              << std::endl;
    exit(1);
  }
  std::cout << "Creating population from " << loader_file_name << "\n";
  return cleanLines(flines);
}


std::vector<std::pair<OrgID, OrgAttributesMap>> Loader::loadPopulation(const std::string &loader_option) {
  tk_counter = 0; // initialize temporary token counter
  static const std::regex plf_file(R"(.*\.plf)");
  std::string all_lines = std::regex_match(loader_option, plf_file)
                       ? loadFromFile(loader_option)
                       : "MASTER = " + loader_option;

  // replace all filenames (expanded or otherwise) with temporary tokens
  all_lines = findAndGenerateAllFiles(all_lines);

  parseAllCommands(all_lines);

  // .plf must contain a variable called MASTER
  if (collection_org_lists.find("MASTER") == collection_org_lists.end()) {
    std::cout << "error: Must load from variable named MASTER" << std::endl;
    exit(1);
  }

  // store the final population of organisms
  if (collection_org_lists.at("MASTER").size() != 1) {
    std::cout << "error: variable named MASTER must contain exactly one population"
              << std::endl;
    exit(1);
  }

  std::vector<std::pair<OrgID, OrgAttributesMap>> final_population;

  // store the final population of organisms
  std::vector<OrgID> org_ids = collection_org_lists.at("MASTER")[0];

  // inform the user which organisms have been loaded
  showFinalPopulation(org_ids);

  // convert the organims attributes_map into a form palatable? to main.cpp
  std::transform(
      org_ids.begin(), org_ids.end(), std::back_inserter(final_population),
      [this](OrgID ID) {
        if (ID < 0) return std::make_pair(ID, OrgAttributesMap()); // default organism
        else return std::make_pair(ID, all_organism_infos.at(ID).attributes_map); // loaded organism
      });

  return final_population;
} // end Loader::loadPopulation

void Loader::showFinalPopulation(std::vector<OrgID> org_ids) {

  // count and report number of random organisms
  auto randos_count = std::accumulate(org_ids.begin(), org_ids.end(), 0, [](long acc, long i) { return acc + (i == -1); }); // TODO MN
  std::cout << "\nLoading " << randos_count << " Random organisms\n";

  // count and report number of default organisms
  auto orgs_count = std::accumulate(org_ids.begin(), org_ids.end(), 0, [](long acc, long i) { return acc + (i == -2); }); // TODO MN
  std::cout << "Loading " << orgs_count << " Default organisms\n";

  std::map<std::string, std::vector<OrgID>> orgs_from_files; // TODO C
  for (auto org_index : org_ids)
    if (org_index > -1) {
      orgs_from_files[all_organism_infos.at(org_index).from_file].push_back( all_organism_infos.at(org_index).orig_ID );
    }
  for (auto &f : orgs_from_files) {
    std::cout << "From file " << f.first << "\n  Loading organisms with IDs : ";
    for (auto k : f.second) std::cout << k << " ";
    std::cout << "\n";
  }
}

  // split into commands. These are all collection to variable assignments
void Loader::parseAllCommands(std::string all_lines) {
  // split into commands. These are all collection-assignments
  static const std::regex command( R"((?:(\w+)\s*=)(.*?)(?=(?:(?:\w+\s*=))|$))" );
  // match one of the collection-assignmenta\s but don't capture
  // lookahead for another collection-assignment (without capturing) or end of line
  for (std::sregex_iterator end,
       i = std::sregex_iterator(all_lines.begin(), all_lines.end(), command);
       i != end; i++) {
    std::smatch m = *i;
    auto expr = m[2].str();
    auto name = m[1].str();
    collection_org_lists[name] = parseExpression(expr);
  }
}

std::string Loader::cleanLines(std::ifstream &flines) {
  // read lines, strip out all trailing comments, and create one long string.
  std::string all_lines = " ", line;
  static const std::regex comments("#.*"); // matches comment char '#'and everything after on the line
  while (getline(flines, line)) {
    if (!line.empty()) {
      std::string clean_line = std::regex_replace(line, comments, ""); // replace comment with empty string
      all_lines += " " + clean_line;
    }
  }

  // check that file must start with an assignment
  static const std::regex garbage( R"(^\s*\w+\s*=.*$)" );
  if (!std::regex_match(all_lines, garbage)) {
    std::cout << " error : Loader file contains unrezognized text at beginning " << std::endl;
    exit(1);
  }
  return all_lines;
}

std::string Loader::findAndGenerateAllFiles(std::string all_lines) {

  std::map<std::string, std::vector<std::string>> collection_of_files; // stores all file names that correspond to each collection
  std::set<std::string> actual_files; // every file the user might refer to

  // get all file names . These must all be in single-qoutes and can be wildcarded
  static const std::regex quoted_files("'([^']*)'"); // make a match group that is anything between single quotes and doesn't have single quotes between them ex: 'afile.csv' works but 'afile'csv' would match "afile"
  std::smatch m;
  while (std::regex_search( all_lines, m, quoted_files)) {
    // create a new token, and bump the token counter
    std::string new_tk = tk_name + std::to_string(tk_counter++);
    // expand the filename in case there are wildcards
    auto exp_files = expandFiles(m[1].str());
    //  update the local list of files associated with collection name
    collection_of_files[new_tk] = exp_files;
    // add list of files to global file list
    actual_files.insert(exp_files.begin(), exp_files.end());
    // replace quoted file names with temporary token name
    all_lines = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
  }

  // simple map from filename -> (start index of org in all_organism_infos, number of
  // sequential organisms from start) efficiently keeps track of organisms
  std::map<std::string, std::pair<long, long>> file_contents;

  // each file is read once and each organism from every file is assigned a unique ID
  // this creates a SINGLE list of organisms that can be efficiently accessed
  for (const auto &file : actual_files) {
    std::cout << "Parsing file " << file << "..." << std::endl;
    file_contents[file] = generatePopulation(file);
  }

  // create all collections from file names
  // Note: collections internally only hold on to the unique index
  for (const auto &cf : collection_of_files) {
    for (const auto &f : cf.second) {
      // create all indices within the range generated for this file
      std::vector<long> sinf(file_contents.at(f).second);
      std::iota(sinf.begin(), sinf.end(), file_contents.at(f).first);
      // add this collection(indices) to global list
      collection_org_lists[cf.first].push_back(sinf);
    }
  }
  // returns .plf script with all filenames removed
  return all_lines;
}

// implements a recursive parser, that stores sub-expressions in a stack, and
// evaluates them bottom-up
std::vector<std::vector<long>> Loader::parseExpression(std::string expr) {

  // a simple linear time check for balanced braces. Strictly speaking not
  // necessary, but it's a very cheap check that gives a much better error
  // message, than if caught during parsing
  if (!balancedBraces(expr)) {
    std::cout << " Expression " << expr << " does not have balanced braces " << std::endl;
    exit(1);
  }

  // make the top of local stack evaluate to the correct expression
  expr = "{" + expr + "}";

  // stores all sub-expressions into temporary tokens
  std::vector<std::pair<std::string, std::string>> local_tk_stack;
  // match lowest-level (non-decomposable) sub-expression
  static const std::regex braces( R"(\{([^\{\}]*)\})" );
  std::smatch m;
  // for each nested sub-expression - resolve braces ...
  while (std::regex_search(expr, m, braces)) { // resolve braced by ...
    // create a new token, and bump token counter
    std::string new_tk = tk_name + std::to_string(tk_counter++); // creating new token
    // add token to local stack of tokens
    local_tk_stack.emplace_back(new_tk, m[1].str()); // adding token to local stack of tokens
    // replace braced sub-expression with token
    expr = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
  } // repeat for all braces ..

  // evaluate tokens in local stack
  static const std::regex colon_sep( R"((.+?)(?:\:|$))" );
  for (const auto &tk : local_tk_stack) {
    // collection that needs to be filled
    std::vector<std::vector<long>> coll;
    // expression corresponding to the token
    std::string tkn = tk.second;
    //	evaluate each sub expression
    for (std::sregex_iterator end,
         i = std::sregex_iterator(tkn.begin(), tkn.end(), colon_sep);
         i != end; i++) {
      // extract the expression text
      std::smatch sub_expr = *i;
      std::string token = sub_expr[1].str();
      // remove syntactic whitespace
      token.erase(std::remove_if(token.begin(), token.end(), ::isspace),
                  token.end());
      auto p = collection_org_lists.find(token) != collection_org_lists.end()
                   ? collection_org_lists[token] // if sub-expression is already evaluated (e.g. repeated filename)
                   : // expression is already evaluated collection
                   parseCollection(
                       sub_expr[1].str()); // expression needs to be parsed

      // insert the population corresponding to the expression into the collection
      coll.insert(coll.end(), p.begin(), p.end());
    }
    // add population to global collections
    collection_org_lists[tk.first] = coll;
  }

  // return collection at top of stack - this must be the complete RHS
  // expression from a user-defined variable assignment
  return collection_org_lists[local_tk_stack.back().first];
} // end  Loader::parseExpression

bool Loader::balancedBraces(std::string s) {
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

std::vector<std::vector<long>>
Loader::parseCollection(const std::string &expr) {

  // each sub-expression can have one of the following keyword commands
  {
    static const std::regex command_collapse( R"(^\s*collapse\s+(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_collapse))
      return keywordCollapse(match[1].str());
  }

  {
    static const std::regex command_random( R"(^\s*random\s+(\d+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_random))
      return keywordRandom(stol(match[1].str()));
  }

  {
    static const std::regex command_default( R"(^\s*default\s+(\d+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_default))
      return keywordDefault(stol(match[1].str()));
  }

  {
    static const std::regex command_greatest( R"(^\s*greatest\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_greatest))
      return keywordGreatest(stoul(match[1].str()), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_least( R"(^\s*least\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_least))
      return keywordLeast(stol(match[1].str()), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_any( R"(^\s*any\s+(\d+)\s+from\s+(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_any))
      return keywordAny(stol(match[1].str()), match[2].str());
  }

  {
    static const std::regex command_match( R"(^\s*match\s+(\w+)\s+where\s+(\S+)\s+from\s+(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_match))
      return keywordMatch(match[1].str(), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_duplicate( R"(^\s*(\d+)\s*\*\s*(\w+)\s*$)" );
    std::smatch match;
    if (std::regex_match(expr, match, command_duplicate))
      return keywordDuplicate(stoul(match[1].str()), match[2].str());
  }

  // if no keyword matches
  std::cout << " error: syntax error while trying to resolve " << std::endl
            << expr << std::endl;
  exit(1);

} // end Loader::parse_token

void Loader::checkTokenExistence(const std::string &token_name) {
  if (collection_org_lists.find(token_name) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << token_name << std::endl;
    exit(1);
  }
}

std::vector<std::vector<long>> Loader::keywordDuplicate(size_t value, const std::string &resource) {

  checkTokenExistence(resource);

  std::vector<std::vector<long>> coll;
  // make 'value' number of copies for each population in the collection
  for (const auto &pop : collection_org_lists[resource]) {
    auto num = value;
    while (num--)
      coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keywordMatch(std::string attribute,
                                                    std::string value,
                                                    const std::string &resource) {

  checkTokenExistence(resource);

  std::vector<std::vector<long>> coll;
  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
    for (const auto &o : from_pop)
      if (all_organism_infos.at(o).attributes_map.find(attribute) ==
          all_organism_infos.at(o).attributes_map.end()) {
        std::cout << "error: while trying to match " << value << "  "
                  << resource << " contains organisms without attribute "
                  << attribute << std::endl;
        exit(1);
      }
    std::vector<long> pop;
    std::copy_if(std::begin(from_pop), std::end(from_pop),
                 std::back_inserter(pop), [&](long index) {
                   return all_organism_infos.at(index).attributes_map.at(attribute) ==
                          value;
                 });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>>
Loader::keywordGreatest(size_t number, std::string attribute, const std::string &resource) {

  checkTokenExistence(resource);

  std::vector<std::vector<long>> coll;
  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
    if (from_pop.size() < number) {
      std::cout << "error: trying to get greatest " << number
                << " from collection, but  " << resource
                << " contains a population that does not have sufficient "
                << " organisms" << std::endl;
      exit(1);
    }
    for (const auto &o : from_pop)
      if (all_organism_infos.at(o).attributes_map.find(attribute) ==
          all_organism_infos.at(o).attributes_map.end()) {
        std::cout << "error:  trying to get greatest " << number
                  << " from collection, but  " << resource
                  << " contains organisms without attribute " << attribute
                  << std::endl;
        exit(1);
      }
    std::vector<long> pop(number);
    std::partial_sort_copy( from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organism_infos.at(lhs).attributes_map.at(attribute)) >
                 std::stod(all_organism_infos.at(rhs).attributes_map.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>>
Loader::keywordLeast(size_t number, std::string attribute, const std::string &resource) {
  checkTokenExistence(resource);

  std::vector<std::vector<long>> coll;

  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
    if (from_pop.size() < number) {
      std::cout << "error: trying to get least" << number
                << " from collection, but  " << resource
                << " contains a population that does not have sufficient "
                << " organisms" << std::endl;
      exit(1);
    }
    for (const auto &o : from_pop)
      if (all_organism_infos.at(o).attributes_map.find(attribute) == all_organism_infos.at(o).attributes_map.end()) {
        std::cout << "error:  trying to get least" << number
                  << " from collection, but  " << resource
                  << " contains organisms without attribute " << attribute
                  << std::endl;
        exit(1);
      }
    std::vector<long> pop(number);
    std::partial_sort_copy( from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organism_infos.at(lhs).attributes_map.at(attribute)) <
                 std::stod(all_organism_infos.at(rhs).attributes_map.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keywordAny(size_t number, const std::string &resource) {
  checkTokenExistence(resource);

  std::vector<std::vector<long>> coll;

  for (const auto &p : collection_org_lists.at(resource)) {
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

std::vector<std::vector<long>> Loader::keywordCollapse(const std::string &resource) {

  checkTokenExistence(resource);
  std::vector<std::vector<long>> coll;
  std::vector<long> pop;
  for (const auto &c : collection_org_lists.at(resource)) {
    pop.insert(pop.end(), c.begin(), c.end());
  }
  coll.push_back(pop);
  return coll;
}

// these 2 functions are intended to have different semantics, but may not be
// distinguished by main.cpp
std::vector<std::vector<long>> Loader::keywordRandom(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -1);
  coll.push_back(pop);
  return coll;
}

std::vector<std::vector<long>> Loader::keywordDefault(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -2);
  coll.push_back(pop);
  return coll;
}

std::vector<std::string> Loader::expandFiles(const std::string &file) {

  //	store file names after wildcard expansion
  std::vector<std::string> result;
  // expand wildcards
  getFilesMatchingRelativePattern(file, result);

  if (result.empty()) {
    std::cout << " error: " << file << " does not match any files" << std::endl;
    exit(1);
  }
  return result;
} // end Loader::expandFiles

std::pair<long, long> Loader::generatePopulation(const std::string &file_name) {

  // store organism file in memory-mapped CSV
  CSV org_file_data = CSV(file_name);
  // setup the range of indices needed to identify all organisms from this file
  std::pair<long,long> file_contents_pair = std::make_pair(long(all_organism_infos.size()), long(org_file_data.row_count()));

  // search for the _data version of the organism file
  std::string data_file_name(dataVersionOfFilename(file_name));
  if (fileExists(data_file_name)) {
    // store _data file in memory_mapped CSV
	  auto data_file_data = CSV(data_file_name);
    // merge the _data file data into the organism file data
	  org_file_data.merge(data_file_data, "ID");
  }

  // for each ID in the organism+_data data (in future, we will be able to assume it's in the org file)
  for (const std::string &id : org_file_data.singleColumn("ID")) {
    // create an internal organism
    OrganismInfo org_info;
    org_info.orig_ID = std::stoi(id);
    org_info.from_file = file_name;
    // stick all the attributes_map into the organism
    for (const std::string &attribute : org_file_data.column_names()) {
      // making sure to use the per-file-unique-ID
      org_info.attributes_map.insert(std::make_pair(attribute, org_file_data.lookUp("ID", id, attribute)));
    }
    // Make sure the original ID,File,Update show up in the first generation's datamap store the original id
    org_info.attributes_map.insert(std::make_pair("loadedFrom.ID",id));
    // store the orginal file from which it was pulled
    org_info.attributes_map.insert(std::make_pair("loadedFrom.File",file_name));
    // store the orginal update
    if (org_info.attributes_map.find("update") != org_info.attributes_map.end()) { 
      org_info.attributes_map.insert(std::make_pair("loadedFrom.Update",org_file_data.lookUp("ID", id, "update")));
    }
    all_organism_infos.push_back(org_info);
  }

  return file_contents_pair;
} // end Loader::generatePopulation


void Loader::printOrganism(long i) {

  // strictly for debugging purposes 
  if (i != -1)
    std::cout << "\tID: " << all_organism_infos.at(i).orig_ID << " from file "
              << all_organism_infos.at(i).from_file << std::endl;
  else
    std::cout << "\trandom default organism" << std::endl;
} // end Loader::printOrganism
