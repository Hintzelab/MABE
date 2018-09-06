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
