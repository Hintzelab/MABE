// author : cgnitash, joryschossau
// Loader.cpp contains implementation of population loading scripting language
#if defined(unix)        || defined(__unix)      || defined(__unix__) \
	|| defined(linux) || defined(__linux) || defined(__linux__) \
	|| defined(sun) || defined(__sun) \
	|| defined(BSD) || defined(__OpenBSD__) || defined(__NetBSD__) \
	|| defined(__FreeBSD__) || defined (__DragonFly__) \
	|| defined(sgi) || defined(__sgi) \
	|| (defined(__MACOSX__) || defined(__APPLE__)) \
	|| defined(__CYGWIN__)
  #define OS_UNIX	1	//!< Unix like OS(POSIX compliant)
  #undef OS_WINDOWS
  #elif defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) \
  	|| defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    #define OS_WINDOWS	1	//!< Microsoft Windows
  #undef OS_UNIX
#endif

#if defined(OS_UNIX)
  #include <sys/types.h> // linux only
  #include <dirent.h> // linux only
  #include <sys/stat.h> // linux only (stat, lstat)
#elif defined(OS_WINDOWS)
  #include <windows.h>
#endif

#include "Loader.h"

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
#include <string.h> // strcmp
#include <algorithm> // for split
#include <iterator> // for split

std::string dataVersionOfFilename(const std::string& filename) {
    std::string ext(filename.substr(filename.rfind(".")));
    int orgposIfExist = filename.rfind("organisms");
    if (orgposIfExist != -1) return filename.substr(0,orgposIfExist)+"data"+filename.substr(orgposIfExist+9);
    else return filename.substr(0,filename.rfind("."))+"_data"+ext;
}

bool fileExists(const std::string& filename) {
#if defined(OS_UNIX)
    struct stat statbuf; // linux only
    return (stat(filename.c_str(), &statbuf) == 0);
#elif defined(OS_WINDOWS)
    DWORD fileAttr;
    fileAttr = GetFileAttributesA(filename.c_str());
    return (0xFFFFFFFF != fileAttr);
#endif
}

bool isDirectory(const std::string& dirname) {
#if defined(OS_UNIX)
    struct stat statbuf; // linux only
    stat(dirname.c_str(), &statbuf);
	 return (S_ISDIR(statbuf.st_mode));
#elif defined(OS_WINDOWS)
    DWORD fileAttr;
    fileAttr = GetFileAttributesA(dirname.c_str());
	 return (fileAttr & FILE_ATTRIBUTE_DIRECTORY);
#endif
}

template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

bool wildstrcmp(const char *first, const char *second) {
    if (*first == '\0' && *second == '\0') // If we reach the end of both std::strings, we are done
        return true;
    if (*first == '*' && *(first+1) != '\0' && *second == '\0') // Make sure the characters after '*' are present in second std::string. This function assumes that the first std::string will not contain two consecutive '*'
        return false;
    if (*first == '?' || *first == *second) // If the first std::string contains '?', or current characters of both std::strings wildstrcmp
        return wildstrcmp(first+1, second+1);
    if (*first == '*') // If there is *, then there are two possibilities a) We consider current character of second std::string b) We ignore current character of second std::string.
        return wildstrcmp(first+1, second) || wildstrcmp(first, second+1);
    return false;
}

void followPathAndCollectFiles(std::string& curPath, unsigned int depthIntoFilterPathParts, std::vector<std::string>& filterPathParts, std::vector<std::string>& collectedFiles) {
    std::string padding(depthIntoFilterPathParts*2,' ');
#if defined(OS_UNIX)
    struct dirent *fileinfo;
    struct stat statbuf;
    struct stat lstatbuf;
    DIR *dir;
    dir = opendir(curPath.c_str());
    while( (fileinfo=readdir(dir)) ) {
        if (strcmp(fileinfo->d_name,".")==0) continue;
        //if (strcmp(fileinfo->d_name,"..")==0) continue;
        //cout << padding << fileinfo->d_name;
        std::string filePath(curPath+"/"+fileinfo->d_name);
        if (curPath == "./") filePath = fileinfo->d_name;
        stat(filePath.c_str(),&statbuf);
        lstat(filePath.c_str(),&lstatbuf);
        if (S_ISDIR(statbuf.st_mode)) {
            //cout << "/" << endl; // found a dir
            if ( wildstrcmp(filterPathParts[depthIntoFilterPathParts].c_str(), fileinfo->d_name) ) {
                std::string newPath(curPath+"/"+fileinfo->d_name);
                if (curPath == "./") newPath = fileinfo->d_name;
                followPathAndCollectFiles(newPath, depthIntoFilterPathParts+1, filterPathParts, collectedFiles);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
           //cout << endl;
           if ( wildstrcmp(filterPathParts[depthIntoFilterPathParts].c_str(), fileinfo->d_name) ) {
               if (depthIntoFilterPathParts == filterPathParts.size()-1) {
                   std::string newPath(fileinfo->d_name);
						 if (curPath != "./") newPath = curPath+"/"+fileinfo->d_name;
                   collectedFiles.push_back(newPath);
               }
           }
        }
    }
    closedir(dir);
#elif defined(OS_WINDOWS)
    WIN32_FIND_DATA data;
    HANDLE hFind;
    std::string newCurPath(curPath);
    DWORD curPathftyp = GetFileAttributesA(curPath.c_str());
    if (curPathftyp & FILE_ATTRIBUTE_DIRECTORY) newCurPath = curPath+"\\*";
    if ((hFind = FindFirstFile(newCurPath.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(data.cFileName,".")==0) continue;
            //cout << data.cFileName;
            std::string filePath(curPath+"\\"+data.cFileName);
            if (curPath == ".") filePath = data.cFileName;
            DWORD ftyp = GetFileAttributesA(filePath.c_str());
            if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
                //cout <<  "\\" << endl; // found a dir
                if ( wildstrcmp(filterPathParts[depthIntoFilterPathParts].c_str(), data.cFileName) ) {
                    std::string newPath(curPath+"\\"+data.cFileName);
                    if (curPath == ".") newPath = data.cFileName;
                    followPathAndCollectFiles(newPath, depthIntoFilterPathParts+1, filterPathParts, collectedFiles);
                }
            } else { // regular file
                //cout << endl;
                if ( wildstrcmp(filterPathParts[depthIntoFilterPathParts].c_str(), data.cFileName) ) {
                    if (depthIntoFilterPathParts == filterPathParts.size()-1) {
								std::string newPath(data.cFileName);
								if (curPath != ".") newPath = curPath+"\\"+data.cFileName;
                        collectedFiles.push_back(newPath);
                    }
                }
            }
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
#endif
}

void getFilesMatchingRelativePattern(const std::string& pattern, std::vector<std::string>& files) {
#if defined(OS_UNIX)
    std::string pathToStart("./");
    char dirSep='/';
#elif defined(OS_WINDOWS)
    std::string pathToStart(".");
    char dirSep='\\';
#endif
    std::vector<std::string> filterPathParts; // filterPath split by dir sep
    split(pattern, filterPathParts, dirSep); // split filterPath into its parts
    followPathAndCollectFiles(pathToStart, 0, filterPathParts, files);
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

std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
Loader::loadPopulation(const std::string &loader_option) {

  tk_counter = 0;

  static const std::regex plf_file(R"(.*\.plf)");

  auto all_lines = std::regex_match(loader_option, plf_file)
                       ? loadFromFile(loader_option)
                       : loader_option;

  all_lines = findAndGenerateAllFiles(all_lines);

  parseAllCommands(all_lines);

  if (collection_org_lists.find("MASTER") == collection_org_lists.end()) {
    std::cout << "error: Must load from variable named MASTER" << std::endl;
    exit(1);
  }
  if (collection_org_lists.at("MASTER").size() != 1) {
    std::cout << "error: variable named MASTER must contain exactly one population"
              << std::endl;
    exit(1);
  }

  std::vector<std::pair<long, std::unordered_map<std::string, std::string>>>
      final_population; // needs to be constructed

  auto final_orgs = collection_org_lists.at("MASTER")[0];

  showFinalPopulation(final_orgs);

  std::transform(
      final_orgs.begin(), final_orgs.end(),
      std::back_inserter(final_population), [this](long org) {
        return org < 0
                   ? make_pair(org,
                               std::unordered_map<std::string, std::string>())
                   : make_pair(org, all_organisms.at(org).attributes);
      });

  return final_population;
} // end Loader::loadPopulation

void Loader::showFinalPopulation(std::vector<long> orgs) {
  std::cout << "\n"
            << "Loading "
            << std::accumulate(orgs.begin(), orgs.end(), 0,
                               [](long acc, long i) { return acc + (i == -1); })
            << " Random organisms\n";
  std::cout << "Loading "
            << std::accumulate(orgs.begin(), orgs.end(), 0,
                               [](long acc, long i) { return acc + (i == -2); })
            << " Default organisms\n";
  std::map<std::string, std::vector<long>> orgs_from_files;
  for (auto i : orgs)
    if (i > -1)
      orgs_from_files[all_organisms.at(i).from_file].push_back(
          all_organisms.at(i).orig_ID);
  for (auto &f : orgs_from_files) {
    std::cout << "From file " << f.first << "\n  Loading organisms with IDs : ";
    for (auto k : f.second)
      std::cout << k << " ";
    std::cout << "\n";
  }
}

void Loader::parseAllCommands(std::string all_lines) {
  // split into commands. These are all collection-assignments
  static const std::regex command(
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
    collection_org_lists[name] = parseExpression(expr);
  }
}

std::string Loader::cleanLines(std::ifstream &flines) {
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
    std::cout << " error : Loader file contains unrezognized text at beginning "
              << std::endl;
    exit(1);
  }
  return all_lines;
}

std::string Loader::findAndGenerateAllFiles(std::string all_lines) {

  ///*
  //for (auto &p :
  //     std::experimental::filesystem::recursive_directory_iterator("./")) {
  //  all_possible_file_names.push_back(std::experimental::filesystem::path(p).generic_string());
  //}
  //*/
  //zz::fs::Directory mabe_org_dir("./", "*organisms*.csv", true); // true=recursive
  //for (auto const &p : mabe_org_dir) {
  //	  all_possible_file_names.push_back(p.relative_path());
  //}
  //
  //zz::fs::Directory mabe_data_dir("./", "*data*.csv", true); // true=recursive
  //for (auto const &p : mabe_data_dir) {
  //	  all_possible_file_names.push_back(p.relative_path());
  //}

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
        tk_name + std::to_string(tk_counter++); // creating new token
    all_lines = m.prefix().str() + " " + new_tk + " " + m.suffix().str();
    auto exp_files = expandFiles(m[1].str()); // expand the filename
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
    file_contents[file] = generatePopulation(file);
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

std::vector<std::vector<long>> Loader::parseExpression(std::string expr) {

  if (!balancedBraces(expr)) {
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
        tk_name + std::to_string(tk_counter++); // creating new token
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
      auto p = collection_org_lists.find(token) != collection_org_lists.end()
                   ? collection_org_lists[token]
                   : // expression is already evaluated collection
                   parseCollection(
                       sub_expr[1].str()); // expression needs to be parsed

      coll.insert(coll.end(), p.begin(), p.end());
    }
    collection_org_lists[tk.first] =
        coll; // add population to global collections
  }

  // return population  at top of stack
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
  // semantics of collection choosing

  //	cout << expr << endl;

  {
    static const std::regex command_collapse(R"(^\s*collapse\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_collapse))
      return keywordCollapse(match[1].str());
  }

  {
    static const std::regex command_random(R"(^\s*random\s+(\d+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_random))
      return keywordRandom(stol(match[1].str()));
  }

  {
    static const std::regex command_default(R"(^\s*default\s+(\d+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_default))
      return keywordDefault(stol(match[1].str()));
  }

  {
    static const std::regex command_greatest(
        R"(^\s*greatest\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_greatest))
      return keywordGreatest(stoul(match[1].str()), match[2].str(),
                             match[3].str());
  }

  {
    static const std::regex command_least(
        R"(^\s*least\s+(\d+)\s+by\s+(\w+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_least))
      return keywordLeast(stol(match[1].str()), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_any(
        R"(^\s*any\s+(\d+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_any))
      return keywordAny(stol(match[1].str()), match[2].str());
  }

  {
    static const std::regex command_match(
        R"(^\s*match\s+(\w+)\s+where\s+(\S+)\s+from\s+(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_match))
      return keywordMatch(match[1].str(), match[2].str(), match[3].str());
  }

  {
    static const std::regex command_duplicate(
        R"(^\s*(\d+)\s*\*\s*(\w+)\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, command_duplicate))
      return keywordDuplicate(stoul(match[1].str()), match[2].str());
  }

  // if no keyword matches
  std::cout << " error: syntax error while trying to resolve " << std::endl
            << expr << std::endl;
  exit(1);

} // end Loader::parse_token

std::vector<std::vector<long>>
Loader::keywordDuplicate(size_t value, const std::string &resource) {

  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists[resource]) {
    const auto &from_pop = p;
    auto num = value;
    while (num--)
      coll.push_back(from_pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keywordMatch(std::string attribute,
                                                    std::string value,
                                                    const std::string &resource) {

  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

  for (const auto &p : collection_org_lists.at(resource)) {
    const auto from_pop = p;
    for (const auto &o : from_pop)
      if (all_organisms.at(o).attributes.find(attribute) ==
          all_organisms.at(o).attributes.end()) {
        std::cout << "error: while trying to match " << value << "  "
                  << resource << " contains organisms without attribute "
                  << attribute << std::endl;
        exit(1);
      }
    std::vector<long> pop;
    std::copy_if(std::begin(from_pop), std::end(from_pop),
                 std::back_inserter(pop), [&](long index) {
                   return all_organisms.at(index).attributes.at(attribute) ==
                          value;
                 });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>>
Loader::keywordGreatest(size_t number, std::string attribute,
                        const std::string &resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

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
      if (all_organisms.at(o).attributes.find(attribute) ==
          all_organisms.at(o).attributes.end()) {
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
          return std::stod(all_organisms.at(lhs).attributes.at(attribute)) >
                 std::stod(all_organisms.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>>
Loader::keywordLeast(size_t number, std::string attribute,
                     const std::string &resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

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
      if (all_organisms.at(o).attributes.find(attribute) ==
          all_organisms.at(o).attributes.end()) {
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
          return std::stod(all_organisms.at(lhs).attributes.at(attribute)) <
                 std::stod(all_organisms.at(rhs).attributes.at(attribute));
        });
    coll.push_back(pop);
  }
  return coll;
}

std::vector<std::vector<long>> Loader::keywordAny(size_t number,
                                                  const std::string &resource) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(resource) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << resource << std::endl;
    exit(1);
  }

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

std::vector<std::vector<long>>
Loader::keywordCollapse(const std::string &name) {
  std::vector<std::vector<long>> coll;
  if (collection_org_lists.find(name) == collection_org_lists.end()) {
    std::cout << "Unrecognised token " << name << std::endl;
    exit(1);
  }
  std::vector<long> pop;
  for (const auto &c : collection_org_lists.at(name)) {
    pop.insert(pop.end(), c.begin(), c.end());
  }
  coll.push_back(pop);
  return coll;
}

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

std::vector<std::string> Loader::expandFiles(const std::string &f) {

  std::vector<std::string> result;
  //static const std::regex wildcard(R"(\*)");
  //std::string file_name = std::regex_replace(f, wildcard, R"([^/]*)");
  //static const std::regex valid_path_names("^" + file_name + "$");
  //static const std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");
  getFilesMatchingRelativePattern(f, result);

  //std::copy_if(all_possible_file_names.begin(), all_possible_file_names.end(),
  //             std::back_inserter(result), [](const std::string &s) {
  //               return std::regex_match(s, valid_path_names) &&
  //                      std::regex_match(s, valid_org_name);
  //             });

  if (result.empty()) {
    std::cout << " error: " << f << " does not match any files" << std::endl;
    exit(1);
  }
  return result;
} // end Loader::expandFiles

std::pair<long, long> Loader::generatePopulation(const std::string &file_name) {

  auto org_file_data = getAttributeMap(file_name);
  auto file_contents_pair =
      std::make_pair(long(all_organisms.size()), long(org_file_data.size()));

  static const std::regex valid_org_name(R"((.*)_organisms(_\d+)?.csv$)");
  std::smatch match_org;
  std::regex_match(file_name, match_org, valid_org_name);
  //auto data_file_name = std::regex_replace(file_name, valid_org_name,
  //                                         match_org[1].str() + "_data" +
  //                                             match_org[2].str() + ".csv");

  std::map<long, std::map<std::string, std::string>> data_file_data;
  std::string data_file_name(dataVersionOfFilename(file_name));
  if (fileExists(data_file_name)) data_file_data = getAttributeMap(data_file_name);
  //if (std::find(all_possible_file_names.begin(), all_possible_file_names.end(),
  //              data_file_name) != all_possible_file_names.end()) {
  //  data_file_data = getAttributeMap(data_file_name);
  //}

  // Note - no checking for overlapping columns
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
                cout << " warning: org " << org.orig_ID << " from file " <<
         file_name
                 << " does not have a corresponding entry in " << file_name <<
         endl
                 << " Was this file generated by MABE? " << endl;
      */
    }
    org.attributes.insert(make_pair("ID", std::to_string(org.orig_ID)));
    all_organisms.push_back(org);
  }

  return file_contents_pair;
} // end Loader::generatePopulation

// reads organisms or data file. return key of ID to map of attributes to values
// attributes do NOT include ID
std::map<long, std::map<std::string, std::string>>
Loader::getAttributeMap(const std::string &file_name) {

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

void Loader::printOrganism(long i) {

  if (i != -1)
    std::cout << "\tID: " << all_organisms.at(i).orig_ID << " from file "
              << all_organisms.at(i).from_file << std::endl;
  else
    std::cout << "\trandom default organism" << std::endl;
} // end Loader::printOrganism
