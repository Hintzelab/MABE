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
      loadPopulation(std::string);

private:
  // all of the organisms info pulled from organisms_files and data_files
  struct Organism_ {
    std::unordered_map<std::string, std::string> attributes;
    // name of organism file this org was pulled from
    std::string from_file;
    // ID in the original file
    long orig_ID;
    bool has_corresponding_data_file;
  };
  std::vector<Organism_> all_organisms_;

  // (invisibly) reserved token names for temporaries
  // this needs a unique name guarantee - enforced
  // by requiring user names to NOT begin with __
  const std::string tk_name_ = "__TK42PL__";
  
  // used to create unique temporary token names
  long tk_counter_ = 0; 

 // literally (see constructor)
  std::vector<std::string>
      all_possible_file_names_;

  // list of orgs in each population of the collection
  std::map<std::string, std::vector<std::vector<long>>>
      collection_org_lists_; 

  // 	methods

  // clean comments and check for invalid syntax
  auto  cleanLines_(std::ifstream &);

  // read script from plf file
  auto  fromPLFfile_(std::string);

  // display population that will be loaded to cout
  void showFinalPopulation_(std::vector<long>);

  // simple syntactic check to avoid issues  when deep inside an evaluation
  bool balancedBraces_(std::string);

  // methods for all keywords
  auto keywordCollapse_(std::string);
  auto keywordRandom_(long number);
  auto keywordDefault_(long number);
  auto keywordGreatest_(size_t, std::string, std::string);
  auto keywordLeast_(size_t, std::string, std::string);
  auto keywordAny_(size_t, std::string);
  auto keywordDuplicate_(size_t, std::string);
  auto keywordMatch_(std::string, std::string, std::string);
 
  // parse single user defined collection
  auto parseCollection_(std::string);

  // unpack and parse entire user-defined expression
  auto parseExpression_(std::string);

  // read file and parse every assignment to user
  // defined variable
  void parseAllCommands_(std::string);

  // for user inputted wildcards
  auto  expandFiles_(std::string);

  // generate population after merging from both organism and data files
  auto generatePopulation_(std::string);

  // a level of indirection so that all possible files the user might need are
  // read at most once
  auto findAndGenerateAllFiles_(std::string all_lines);

  // read MABE generated files and constructs organsims 
  // redundant function from MABE - should be cleaned
  //
 // std::map<long, std::map<std::string, std::string>>
   //   getAttributeMap_(std::string); // read file and construct partial organism


};

