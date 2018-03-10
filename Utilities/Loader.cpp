// author : cgnitash
// Loader.cpp contains implementation of population loading scripting language


#include<iostream>
#include<string>
#include<map>
#include<unordered_map>
#include<vector>
#include<sstream>
#include<fstream>
#include<regex>
#include<numeric>
#include<random>
//#include <experimental/filesystem>
#include "zupply.h" // for x-platform filesystem
#include "Loader.h"

using std::cout;
using std::endl;
using zz::fs::Directory; // filesystem crawling

std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
Loader::load_population(const std::string &loader_file_name) {

  tk_counter = 0;

  std::ifstream flines(loader_file_name);
  if (!flines.is_open()) {
    std::cout << " error: population loader file " << loader_file_name
              << " does not exist " << std::endl
              << " Run MABE with -l to generate population loader file"
              << std::endl;
    exit(1);
  }

  std::cout << "Creating population from " << loader_file_name << std::endl;

  auto all_lines = clean_lines(flines);

  all_lines = find_and_generate_all_files(all_lines);

  parse_all_commands(all_lines);

  if (collection_org_lists.find("MASTER") == collection_org_lists.end()) {
    cout << "error: Must load from variable named MASTER" << endl;
    exit(1);
  }
  if (collection_org_lists.at("MASTER").size() != 1) {
    cout << "error: variable named MASTER must contain only one population" << endl;
    exit(1);
  }

  std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
      final_population; // needs to be constructed

  auto final_orgs = collection_org_lists.at("MASTER")[0];

  show_final_population(final_orgs);

  std::transform(
      final_orgs.begin(), final_orgs.end(),
      std::back_inserter(final_population), [this](long org) {
        return org < 0
                   ? make_pair(org,
                               std::unordered_map<std::string, std::string>())
                   : make_pair(org, all_organisms.at(org).attributes);
      });

  return final_population;
} // end Loader::load_population

void Loader::show_final_population(std::vector<long> orgs) {
  cout << endl
       << "Loading "
       << std::accumulate(orgs.begin(), orgs.end(), 0, [](long acc, long i) {
            return acc + (i == -1);
          }) << " Random organisms" << endl;
  cout << "Loading "
       << std::accumulate(orgs.begin(), orgs.end(), 0, [](long acc, long i) {
            return acc + (i == -2);
          }) << " Default organisms" << endl;
  std::map<std::string,std::vector<long>> orgs_from_files;
  for (auto i : orgs)
    if (i > -1)
      orgs_from_files[all_organisms.at(i).from_file].push_back(
          all_organisms.at(i).orig_ID);
  for (auto &f : orgs_from_files) {
    cout << "From file " << f.first << endl << "  Loading organisms with IDs : ";
    for (auto k : f.second)
      cout << k << " ";
	cout << endl;
  }
}

void Loader::parse_all_commands(std::string all_lines) {
  // split into commands. These are all collection-assignments
  std::regex command(
      R"((?:(\w+)\s*=)(.*?)(?=(?:(?:\w+\s*=))|$))");
  // match
  // one of the collection-assignmenta\s but don't capture
  // lookahead for another collection-assignment (without capturing) or end of
  // line
  for (std::sregex_iterator end,
       i = std::sregex_iterator(all_lines.begin(), all_lines.end(), command);
       i != end; i++) {
    std::smatch m = *i;
    auto expr = m[2].str();
    auto name = m[1].str();
    collection_org_lists[name] = parse_expression(expr);
  }
}

std::string Loader::clean_lines(std::ifstream &flines) {
  // read lines, strip out all trailing comments, and create one long string.
  std::string all_lines = " ", line;
  std::regex comments("#.*");
  while (getline(flines, line)) {
    if (!line.empty()) {
      std::string clean_line = std::regex_replace(line, comments, "");
      all_lines += " " + clean_line;
    }
  }

  std::regex garbage(R"(^\s*\w+\s*=.*$)");
  // check that file must start with an assignment
  if (!std::regex_match(all_lines, garbage)) {
    cout << " error : Loader file contains unrezognized text at beginning "
         << endl;
    exit(1);
  }
  return all_lines;
}

std::string Loader::find_and_generate_all_files(std::string all_lines) {

  /*
  for (auto &p :
       std::experimental::filesystem::recursive_directory_iterator("./")) {
    all_possible_file_names.push_back(std::experimental::filesystem::path(p).generic_string());
  }
  */
  Directory mabeDir("./", true); // true=recursive
  for (auto p : mabeDir) {
  	  all_possible_file_names.push_back(p.relative_path());
  }

  std::map<std::string, std::vector<std::string>> collection_of_files; // all file names for a collection
  std::set<std::string> actual_files;               // every file the user might refer to
  // get all file names . These must all be in single-qoutes and can be
  // wildcarded
  std::regex quoted_files("'([^']*)'");
  std::smatch m;
  while (std::regex_search(
      all_lines, m,
      quoted_files)) { // replace quoted file names with temporary token name
    std::string new_tk =
        tk_name + std::to_string(tk_counter++); // creating new token
    all_lines = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
    auto exp_files = expand_files(m[1].str()); // expand the filename
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
    file_contents[file] =  generate_population(file);
  }

  	// create all collections from file names
  for (const auto &cf : collection_of_files) {
    for (const auto &f : cf.second) {
      std::vector<long> sinf(file_contents.at(f).second);
      std::iota(sinf.begin(), sinf.end(), file_contents.at(f).first);
      collection_org_lists[cf.first].push_back(sinf);
    }
  }
  return all_lines;
}

std::vector<std::vector<long>> Loader::parse_expression(std::string expr) {


  if (!balanced_braces(expr)) {
    cout << " Expression " << expr << " does not have balanced braces " << endl;
    exit(1);
  }

  expr = "{" + expr + "}"; // makes the top of local stack evaluate to the
                           // correct expression

  std::vector<std::pair<std::string, std::string>> local_tk_stack;
  std::regex braces(R"(\{([^\{\}]*)\})");
  std::smatch m;
  while (std::regex_search(expr, m, braces)) { // resolve braced by ...
    std::string new_tk =
        tk_name + std::to_string(tk_counter++); // creating new token
    local_tk_stack.push_back(
        {new_tk, m[1].str()}); // adding token to local stack of tokens
    expr = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
    // replacing brace sub-expr with token
  } // repeat for all braces ..

  // evaluate tokens in local stack
  std::regex colon_sep(R"((.+?)(?:\:|$))");
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
      auto p = collection_org_lists.find(token) != collection_org_lists.end()
                   ? collection_org_lists[token]
                   : // expression is already evaluated collection
                   parse_collection(
                       sub_expr[1].str()); // expression needs to be parsed

      coll.insert(coll.end(), p.begin(), p.end());
    }
    collection_org_lists[tk.first] =
        coll; // add population to global collections
  }

  // return population  at top of stack
  return collection_org_lists[local_tk_stack.back().first];
} // end  Loader::parse_expression

bool Loader::balanced_braces(std::string s) {
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
  return k ? false : true;
}

std::vector<std::vector<long>>
Loader::parse_collection(std::string expr) {
	//semantics of collection choosing

//	cout << expr << endl;

        {
          std::regex command_collapse(R"(^\s*collapse\s+(\w+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_collapse))
            return keyword_collapse(match[1].str());
        }
 
 		{
          std::regex command_random(R"(^\s*random\s+(\d+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_random))
            return keyword_random(stol(match[1].str()));
        }
 
 		{
          std::regex command_default(R"(^\s*default\s+(\d+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_default))
            return keyword_default(stol(match[1].str()));
        }
 
 		{
          std::regex command_greatest(
              R"(^\s*greatest\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_greatest))
            return keyword_greatest(stoul(match[1].str()), match[2].str(),
                                match[3].str());
        }
 
 		{
          std::regex command_least(
              R"(^\s*least\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_least))
            return keyword_least(stol(match[1].str()),
                                 match[2].str(), match[3].str());
        }
      
	  	{
          std::regex command_any(R"(^\s*any\s+(\d+)\s+from\s+(\w+)\s*$)");
          std::smatch match;
          if (std::regex_match(expr, match, command_any))
            return keyword_any(stol(match[1].str()), match[2].str());
        }
      
	  	// if nothing matches 
	cout << " error: syntax error while trying to resolve " << endl
		<< expr<< endl;
	exit(1);

} // end Loader::parse_token

std::vector<std::vector<long>> Loader::keyword_greatest(size_t number,
                                                        std::string attribute,
                                                        std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    cout << "Unrecognised token " << resource << endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
  	if (from_pop.size() < number) {
      cout << " Collection " << resource
           << " contains a population that does not have " << number
           << " organisms" << endl;
      exit(1);
    }
    for (const auto &o : from_pop)
      if (all_organisms.at(o).attributes.find(attribute) ==
          all_organisms.at(o).attributes.end()) {
        cout << "error: " << resource
             << " contains organisms without attribute " << attribute << endl;
        exit(1);
      }
    std::vector<long> pop(number);
    std::partial_sort_copy(
        from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organisms.at(lhs).attributes.at(attribute)) >
                 std::stod(all_organisms.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keyword_least(size_t number,
                                                     std::string attribute,
                                                     std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    cout << "Unrecognised token " << resource << endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
    if (from_pop.size() < number) {
      cout << " Collection " << resource
           << " contains a population that does not have " << number
           << " organisms" << endl;
      exit(1);
    }
    std::vector<long> pop(number);
    std::partial_sort_copy(
        from_pop.begin(), from_pop.end(), pop.begin(), pop.end(),
        [&](long lhs, long rhs) {
          return std::stod(all_organisms.at(lhs).attributes.at(attribute)) >
                 std::stod(all_organisms.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keyword_any(size_t number,
                                                   std::string resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    cout << "Unrecognised token " << resource << endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists.at(resource)) {
    auto from_pop = p;
    if (from_pop.size() < number) {
      cout << " Collection " << resource
           << " contains a population that does not have " << number
           << " organisms" << endl;
      exit(1);
    }
    std::shuffle(from_pop.begin(), from_pop.end(), std::random_device());
    std::vector<long> pop(from_pop.begin(), from_pop.begin() + number);
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keyword_collapse(std::string name) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(name) == collection_org_lists.end()) {
    cout << "Unrecognised token " << name << endl;
    exit(1);
  }
  std::vector<long> pop;
  for (const auto &c : collection_org_lists.at(name)) {
    pop.insert(pop.end(), c.begin(), c.end());
  }
  coll.push_back(pop);
return coll;
}

std::vector<std::vector<long>> Loader::keyword_random(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -1);
  coll.push_back(pop);
  return coll;
}

std::vector<std::vector<long>> Loader::keyword_default(long number) {
  std::vector<std::vector<long>> coll;
  std::vector<long> pop(number, -2);
  coll.push_back(pop);
  return coll;
}

std::vector<std::string> Loader::expand_files(const std::string f) {

  std::vector<std::string> result;
  std::regex wildcard(R"(\*)");
  std::string file_name = std::regex_replace(f, wildcard, R"(\w*)");
  std::regex valid_path_names("^" + file_name + "$");
  std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");

  std::copy_if(all_possible_file_names.begin(), all_possible_file_names.end(),
               std::back_inserter(result),
               [&valid_org_name,&valid_path_names](const std::string &s) {
                 return std::regex_match(s, valid_path_names) &&
                        std::regex_match(s, valid_org_name);
               });

  if (result.empty()) {
    cout << " warning: " << f<< " does not match any files" << endl;
    exit(1);
  }
  return result;
} // end Loader::expand_files

std::pair<long, long> Loader::generate_population(const std::string file_name) {

  auto org_file_data = get_attribute_map(file_name);
  auto file_contents_pair =
      std::make_pair(all_organisms.size(), org_file_data.size());

  std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");
  std::smatch match_org;
  std::regex_match(file_name,match_org,valid_org_name);
  auto data_file_name = std::regex_replace(file_name, valid_org_name,
                                 match_org[1].str() + "_data" +
                                     match_org[2].str() + ".csv");

  std::map<long, std::map<std::string, std::string>> data_file_data;
  if (std::find(all_possible_file_names.begin(), all_possible_file_names.end(),
                data_file_name) != all_possible_file_names.end()) {
    data_file_data = get_attribute_map(data_file_name);
  }

  //Note - no checking for overlapping columns
  for (const auto &org_data : org_file_data) {
    organism org;
    org.orig_ID = org_data.first;
    org.from_file = file_name;
    org.attributes.insert(org_data.second.begin(), org_data.second.end());
    if (data_file_data.find(org.orig_ID) != data_file_data.end()) {
      org.has_corresponding_data_file = true;
      org.attributes.insert(data_file_data.at(org.orig_ID).begin(),
                            data_file_data.at(org.orig_ID).end());
    } else {
      org.has_corresponding_data_file = false;
// should Warnings be silenced?
/*
	  cout << " warning: org " << org.orig_ID << " from file " << file_name
           << " does not have a corresponding entry in " << file_name << endl
           << " Was this file generated by MABE? " << endl;
*/
 	}
	org.attributes.insert(make_pair("ID",std::to_string(org.orig_ID)));
    all_organisms.push_back(org);
  }

  return file_contents_pair;
} // end Loader::generate_population

// reads organisms or data file. return key of ID to map of attributes to values 
// attributes do NOT include ID
std::map<long, std::map<std::string, std::string>>
Loader::get_attribute_map(const std::string &file_name) {

  std::map<long, std::map<std::string, std::string>> result;

  // check if organsims or data file
  std::regex org_or_data(R"((.*)_(data|organisms)(_\d+)?.csv$)");
  std::smatch match_org;
  if (!std::regex_match(file_name, match_org, org_or_data)) {
    cout << " error: unrecognized file name format " << file_name << endl
         << " Was this file generated by MABE? " << endl;
    exit(1);
  }
  std::ifstream file(file_name);
  if (!file.is_open()) {
    cout << " error: unable to load" << file_name << endl;
    exit(1);
  }

  std::string attr_names;
  getline(file, attr_names);
  std::regex each_attribute(R"([\w|:]+)");
  std::vector<std::string> attribute_names;

  for (std::sregex_iterator end,
       i = std::sregex_iterator(attr_names.begin(), attr_names.end(),
                                each_attribute);
       i != end; i++) {
    attribute_names.push_back((*i).str());
  }

  if (std::find(attribute_names.begin(), attribute_names.end(), "ID") ==
      attribute_names.end()) {
    cout << " error: no ID for organisms in file " << file_name << endl;
    exit(1);
  }
  
  // checking for MABE csv-ness
  std::regex mabe_csv_regex(R"((([-\.\d]+)(?:,|$))|("\[)|(([-\.\d]+\]")(?:,|$)))");
  //	std::regex mabe_csv_regex(R"(("[^"]+"|[^,]+)(,|$))");  // does not work because of 
  //	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61582
  std::string org_details;
  while (getline(file, org_details)) {
    std::map<std::string, std::string> temp_result;
    long k = 0;
    auto in_quotes = false;
    std::string quote_str = "";
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
        cout << " error : something wrong with mabe csv-ness " << endl;
        exit(1);
      }
    }
	auto orig_ID = std::stol(temp_result.at("ID"));
	temp_result.erase("ID");
    result[orig_ID] = temp_result;
  }
  file.close();
  return result;
} // end Loader::get_attribute_map

void Loader::print_organism(long i) {

  if (i != -1)
    std::cout << "\tID: " << all_organisms.at(i).orig_ID << " from file "
              << all_organisms.at(i).from_file << std::endl;
  else
    std::cout << "\trandom default organism" << endl;
} // end Loader::print_organism
