// author : joryschossau

#include "Filesystem.h"
#include <vector>
#include <regex>
#include <string>

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
    //static const std::regex dirSepPattern(std::string(1,dirSep));
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
