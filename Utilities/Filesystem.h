// author : joryschossau
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

#include <algorithm> // for split
#include <iterator> // for split
#include <string>
#include <sstream>
#include <vector>

// given a path or filename, return T or F if it exists already
bool fileExists(const std::string & /*filename*/);

// given a path or filename, return T if directory, F if file
bool isDirectory(const std::string & /*dirname*/);

// given a dir path, and a bash-style wildcard pattern, get all files that match the pattern, saving into given vector of strings (RECURSIVE)
// curPath: string.  Ex: "./"
// depthIntoFilterPathParts: uint.  Ex: 3  (this is which subpattern of the vector `filterPathParts` we're currently matching for)
// filterPathParts: vector<string>.  Ex: "C0*_/rep1??/*.csv" should be passed as a vector {"C0*_","rep1??","*.csv"}
// collectedFiles: vector<string>.  Ex: {"C01_final/rep101/LOD_data.csv","C02_final/rep102/LOD_organisms.csv"} (the output of this function)
void followPathAndCollectFiles(std::string & /*curPath*/,
                               unsigned int /*depthIntoFilterPathParts*/,
                               std::vector<std::string> & /*filterPathParts*/,
                               std::vector<std::string> & /*collectedFiles*/);

// given a bash-style pattern, fill given vector of strings with all filenames that match the pattern
// pattern: string.  Ex: "C0*_/rep1??/*.csv" will internally be converted to a vector with strings regex-converted {"C0.*_","rep1.?.?",".*\.csv"}
// files: vector<string>.  Ex: {"C01_final/rep101/LOD_data.csv","C02_final/rep102/LOD_organisms.csv"} (the output of this function)
// Notes: backslashes and slashes do funny things in a regex, so we convert those to '@' then split everything after regexing by '@'.
// So... this might blow up if there are '@' in the filenames or patternes given
void getFilesMatchingRelativePattern(const std::string & /*pattern*/,
                                     std::vector<std::string> & /*files*/);

// works like python's str.split() fn
template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
