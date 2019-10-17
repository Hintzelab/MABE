// author : cgnitash
// Loader.h contains decl of Loader class

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <regex>

typedef std::unordered_map<std::string,std::string> OrgAttributesMap;
typedef long OrgID;

class Loader {
public:
private:
  struct OrganismInfo { // all of the organisms info pulled from organisms_files and data_files
    OrgAttributesMap attributes_map;
    // long ID;	// not used, since ID is known from position in all_organism_infos
    std::string from_file;  // name of organism file this org was pulled from
    int orig_ID; // ID in the original file
  };

  std::vector<OrganismInfo> all_organism_infos;

  const std::string tk_name = "__TK42PL__"; // (invisibly) reserved token names for temporaries
  // this needs a unique name guarantee - enforced
  // by requiring user names to NOT begin with __
  long tk_counter;           // used to create unique temporary token names

  std::vector<std::string> all_possible_file_names; // literally (see constructor)

  std::map<std::string, std::vector<std::vector<long>>> collection_org_lists; // list of orgs in each population of the collection

  // 	methods

  std::string loadFromFile(const std::string &);
  
  std::vector<std::string> expandFiles(const std::string &);// for user inputted wildcards
  std::pair<long, long> generatePopulation(const std::string &);
	std::string findAndGenerateAllFiles(std::string /*all_lines*/);
  // read MABE generated files and constructs organsims 
  // redundant function from MABE - should be cleaned

  //std::map<long, std::map<std::string, std::string>> getAttributeMap( // considering removal of fn
  //    const std::string &); // read file and construct partial organism

  void parseAllCommands( std::string); // read file and parse every assignment to user defined variable
  std::vector<std::vector<long>> parseExpression( std::string); // unpack and parse entire user-defined expression
  std::vector<std::vector<long>> parseCollection(const std::string &); // parse single user defined collection

  std::string cleanLines(std::ifstream &);  // clean comments and check for invalid syntax
                                            // a level of indirection so that all possible files the user might need are
                                            // read exactly once
  bool balancedBraces(std::string); // simple syntactic check to avoid issues
                                    // when deep inside an evaluation
  void showFinalPopulation(std::vector<long>);

  // methods for all keywords
  std::vector<std::vector<long>> keywordCollapse(const std::string &);
  std::vector<std::vector<long>> keywordRandom(long number);
  std::vector<std::vector<long>> keywordDefault(long number);
  std::vector<std::vector<long>> keywordGreatest(size_t, std::string, const  std::string &);
  std::vector<std::vector<long>> keywordLeast(size_t, std::string, const std::string &);
  std::vector<std::vector<long>> keywordAny(size_t, const std::string &);
                            void checkTokenExistence(const std::string & /*token_name*/);
  std::vector<std::vector<long>> keywordDuplicate(size_t, const std::string &);
  std::vector<std::vector<long>> keywordMatch(std::string, std::string, const std::string &);

  void printOrganism(long); // strictly to debug all_organism_infos entries

public:
  std::vector<std::pair<OrgID, OrgAttributesMap>> loadPopulation(const std::string &);
};
