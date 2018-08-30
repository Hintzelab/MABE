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

bool isDirectory(const std::string&);
bool fileExists(const std::string& filename);
bool isDirectory(const std::string& dirname);
template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}
