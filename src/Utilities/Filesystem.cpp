// author : joryschossau

#include "Filesystem.h"
#include <vector>
#include <regex>
#include <string>

// given a path or filename, return T or F if it exists already
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

// given a path or filename, return T if directory, F if file
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

// given a dir path, and a bash-style wildcard pattern, get all files that match the pattern, saving into given vector of strings (RECURSIVE)
// curPath: string.  Ex: "./"
// depthIntoFilterPathParts: uint.  Ex: 3  (this is which subpattern of the vector `filterPathParts` we're currently matching for)
// filterPathParts: vector<string>.  Ex: "C0*_/rep1??/*.csv" should be passed as a vector {"C0*_","rep1??","*.csv"}
// collectedFiles: vector<string>.  Ex: {"C01_final/rep101/LOD_data.csv","C02_final/rep102/LOD_organisms.csv"} (the output of this function)
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
        std::string filePath(curPath+"/"+fileinfo->d_name);
        if (curPath == "./") filePath = fileinfo->d_name;
        stat(filePath.c_str(),&statbuf);
        lstat(filePath.c_str(),&lstatbuf);
        const std::regex pattern(filterPathParts[depthIntoFilterPathParts]);
        if (S_ISDIR(statbuf.st_mode)) {
            if ( std::regex_match(fileinfo->d_name, pattern) ) {
                std::string newPath(curPath+"/"+fileinfo->d_name);
                if (curPath == "./") newPath = fileinfo->d_name;
                followPathAndCollectFiles(newPath, depthIntoFilterPathParts+1, filterPathParts, collectedFiles);
            }
        } else if (S_ISREG(statbuf.st_mode)) {
            if ( std::regex_match(fileinfo->d_name, pattern) ) {
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
            std::string filePath(curPath+"\\"+data.cFileName);
            if (curPath == ".") filePath = data.cFileName;
            DWORD ftyp = GetFileAttributesA(filePath.c_str());
            const std::regex pattern(filterPathParts[depthIntoFilterPathParts]);
            if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {
                if ( std::regex_match(data.cFileName, pattern) ) {
                    std::string newPath(curPath+"\\"+data.cFileName);
                    if (curPath == ".") newPath = data.cFileName;
                    followPathAndCollectFiles(newPath, depthIntoFilterPathParts+1, filterPathParts, collectedFiles);
                }
            } else { // regular file
                if ( std::regex_match(data.cFileName, pattern) ) {
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


// given a bash-style pattern, fill given vector of strings with all filenames that match the pattern
// pattern: string.  Ex: "C0*_/rep1??/*.csv" will internally be converted to a vector with strings regex-converted {"C0.*_","rep1.?.?",".*\.csv"}
// files: vector<string>.  Ex: {"C01_final/rep101/LOD_data.csv","C02_final/rep102/LOD_organisms.csv"} (the output of this function)
// Notes: backslashes and slashes do funny things in a regex, so we convert those to '@' then split everything after regexing by '@'.
// So... this might blow up if there are '@' in the filenames or patternes given
void getFilesMatchingRelativePattern(const std::string& pattern, std::vector<std::string>& files) {
#if defined(OS_UNIX)
    std::string pathToStart("./");
    char dirSep='/';
    static const std::regex dirSepPattern("/");
#elif defined(OS_WINDOWS)
    std::string pathToStart(".");
    char dirSep='\\';
    static const std::regex dirSepPattern("\\\\");
#endif
    std::string file_name_pattern(pattern); // we will escape all regex-sensitive symbols appropriately and convert other appropriately (?->.?, *->.*)
    static const std::regex period(R"(\.)");
    static const std::regex wildcard_one_char(R"(\?)");
    static const std::regex wildcard_0_or_more_chars(R"(\*)");
    file_name_pattern = std::regex_replace(file_name_pattern, dirSepPattern, R"(@)");
    file_name_pattern = std::regex_replace(file_name_pattern, period, R"(\.)");
    file_name_pattern = std::regex_replace(file_name_pattern, wildcard_one_char, R"(.?)");
    file_name_pattern = std::regex_replace(file_name_pattern, wildcard_0_or_more_chars, R"(.*)");
    std::vector<std::string> filterPathParts; // filterPath split by dir sep
    split(file_name_pattern, filterPathParts, '@'); // split filterPath into its parts
    followPathAndCollectFiles(pathToStart, 0, filterPathParts, files);
}
