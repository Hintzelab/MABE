// author : cgnitash
// Loader.h contains decl of Loader class

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <regex>

class Loader {

public:

  std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
  loadPopulation(const std::string &);

private:
  struct organism { // all of the organisms info pulled from organisms_files and
                    // data_files
    std::unordered_map<std::string, std::string> attributes;
    // long ID;	// not used, since ID is known from position in all_organisms
    std::string from_file;  // name of organism file this org was pulled from
    long orig_ID; // ID in the original file
    bool has_corresponding_data_file;
  };
  std::vector<organism> all_organisms; // literally

  const std::string tk_name = "__TK42PL__"; // (invisibly) reserved token names for temporaries
  // this needs a unique name guarantee - enforced
  // by requiring user names to NOT begin with __
  long tk_counter;           // used to create unique temporary token names

  std::vector<std::string>
      all_possible_file_names; // literally (see constructor)

  std::map<std::string, std::vector<std::vector<long>>>
      collection_org_lists; // list of orgs in each population of the collection

  // 	methods

  std::string loadFromFile(const std::string &);
  
  std::vector<std::string>
      expandFiles(std::string);// for user inputted wildcards
  std::pair<long, long>
      generatePopulation(std::string);
  // read MABE generated files and constructs organsims 
  // redundant function from MABE - should be cleaned
  //
  std::map<long, std::map<std::string, std::string>> getAttributeMap(
      const std::string &); // read file and construct partial organism

  void parseAllCommands(
      std::string); // read file and parse every assignment to user
                    // defined variable
  std::vector<std::vector<long>> parseExpression(
      std::string); // unpack and parse entire user-defined expression
  std::vector<std::vector<long>>
      parseCollection(std::string); // parse single user defined collection

  std::string
  cleanLines(std::ifstream &); // clean comments and check for invalid syntax
  std::string findAndGenerateAllFiles(std::string all_lines);
  // a level of indirection so that all possible files the user might need are
  // read exactly once
  bool balancedBraces(std::string); // simple syntactic check to avoid issues
                                     // when deep inside an evaluation
  void showFinalPopulation(std::vector<long>);

  // methods for all keywords
  std::vector<std::vector<long>> keywordCollapse(std::string);
  std::vector<std::vector<long>> keywordRandom(long number);
  std::vector<std::vector<long>> keywordDefault(long number);
  std::vector<std::vector<long>> keywordGreatest(size_t, std::string,
                                                  std::string);
  std::vector<std::vector<long>> keywordLeast(size_t, std::string,
                                               std::string);
  std::vector<std::vector<long>> keywordAny(size_t, std::string);
  std::vector<std::vector<long>> keywordMatch(std::string, std::string,
                                               std::string);

  void printOrganism(long); // strictly to debug all_organisms entries
};

