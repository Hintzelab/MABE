// author : cgnitash
// parser.cpp contains decl of loader class 

#pragma once 

#include<string>
#include<map>
#include<unordered_map>
#include<vector>
#include<sstream>
#include<set>
#include<fstream>
#include<experimental/filesystem>

using std::vector;
using std::map;
using std::set;
using std::string;
using std::pair;
//namespace fs = experimental::filesystem;

struct organism{	//	convenience struct to keep track of loaded organism & assoc data 

	std::unordered_map<string,string> attributes;
	//long ID;	// not used, since ID is known from position in all_organisms
	string from_file;
	long orig_ID;
	bool has_corresponding_data_file;
};


class loader{	

//	const vector<string> KEY_WORDS;	// reserved keywords for MABE loading script
//	vector<string>  commands;	// user commands

	vector<string> all_possible_file_names;	// literally (see constructor)
	set<string> actual_files;				// every file the user might refer to

	map<string,pair<long,long>> file_contents;	// inclusive range of orgs the file contains
	map<string,vector<string>> file_attributes;	// columns in a file

	vector<string> variable_names;	// user variables
	map<string,vector<string>> variable_files;	// list of files assoc with a variable 

	vector<organism> all_organisms; // literally

	vector<string> token_names;	// user defined and temporary tokens
	map<string,vector<long>> token_org_list;	// ist of orgs assoc with a tokem

	const string tk_name;	// (invisibly) reserved token names for temporaries
	long tk_counter;	// used to create unique temporary token names

// 	methods
	vector<string> expand_files(string);	// for user inputted wildcards
	void generate_population(string); // MUST blend with MABE
	std::map<long,std::map<std::string,std::string>> get_attribute_map(const std::string&);
	

	void cLOAD(const string &, const string &);
	void cCHOOSE(const string &, const string &);
	
	vector<long> parse_expression(string);
	vector<long> parse_token(string);

	void print_organism(long); // Must blend with MABE

public:

	loader() :
		// KEY_WORDS({"load","choose","init_pop"}),
		 tk_name( "TK42PLACEHOLDER_"),
		 tk_counter (0) {
		for(auto &p : std::experimental::filesystem::recursive_directory_iterator("./")) {
			all_possible_file_names.push_back(std::experimental::filesystem::path(p));
		}
/*		for(auto & n : all_possible_file_names) {
			cout << n << endl;
		}
*/ 

	}
	
	vector<pair<long,std::unordered_map<string,string>>> load_population(const string &); 

};

