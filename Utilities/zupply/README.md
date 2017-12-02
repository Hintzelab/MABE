# Zupply - A light-weight portable C++ 11 library for Researches and Demos

![Build Status](https://travis-ci.org/zhreshold/zupply.svg?branch=master)
[![License](https://img.shields.io/:license-MIT-blue.svg)](./LICENSE)

## Introduction
**Zupply** is a light-weight, cross-platform, easy to use C++11 library packed with fundamental functions/classes best
for reaserches/small projects/demos.

#### Why Zupply
- Tired of repetitive coding on reading/writing files? Logging messages? Loading configurations?
- Feel desperate transferring code to another platform because you hard-coded in platform specific ways?
- Reluctant to use Boost because it's too heavy-weight?
- Hate setting up environments on a clean computer without any develop library which is required to be linked by many programs?
- Just want to build a small demo, why bother libraries such as OpenCV just for reading/writing images?

- ##### If you agree at least two of them, zupply will be the right tool.

#### Features
- Zero dependency, only C++ 11 standard
- Designed to be easily included in projects, no need to link
- Pure and clean, everything encapsulated in namespace zz, almost no pollution if you don't expose the namespace ([explain](https://github.com/ZhreShold/zupply/wiki/Why-not-single-file-and-header-only%3F))
- Targeting Linux/Windows/Mac OS X/Partial Unix based OSes, meanwhile providing unified experience coding on each platform

#### What's included
- CLI [Argument-parser](https://github.com/ZhreShold/zupply/wiki/Argument-Parser)
- INI/CFG [Configuration-file-parser](https://github.com/ZhreShold/zupply/wiki/Configuration-file-parser)
- Easy to use [Timer and DateTime](https://github.com/ZhreShold/zupply/wiki/Date-and-Timer) classes to measure time and datetime
- Fast sync/async [Logger](https://github.com/ZhreShold/zupply/wiki/Logger) with rich information and highly configurable
- Cross-platform functions to handle [filesystems](https://github.com/ZhreShold/zupply/wiki/Filesystem). (create directory, check file existence, iterate through folder etc...)
- Various [formatting](https://github.com/ZhreShold/zupply/wiki/Format) functions to (trim/split/replace/...) strings
- Thread safe data structures for specific purposes
- [ProgBar](https://github.com/ZhreShold/zupply/wiki/Progress-Bar) class that is designed for displaying progress bar inside consoles
- [Image and ImageHdr](https://github.com/ZhreShold/zupply/wiki/Handle-image) class to do basic operations with image, for example, read(JPG, PNG, BMP, TGA, PSD, GIF, HDR, PIC)/write(JPG, PNG, TGA, BMP), crop, resize, pixel-wise access.
- A lot more

#### What's under construction
- Serializer/Deserializer: for dump/read objects to/from string directly, binary should also be supported


## Usage
#### zupply is designed to be as easy to integrate as possible, thus you can:
- Copy **zupply.hpp** and **zupply.cpp** into your project
- Start writing code
```c++
#include "zupply.hpp"
using namespace zz; // optional using namespace zz for ease

// write your own code
int main(int argc, char** argv)
{
    auto logger = log::get_logger("default");
    logger->info("Welcome to zupply!");
    return 0;
}
```
- Build and run

##### Note: you will need a compiler which support C++11 features, the following compilers/libraries are tested

#### You can start from scratch with quickstart.cpp in src folder
- vc++12(Visual Studio 2013) or newer
```
# create visual studio project require cmake
cd build & create_visual_studio_2013_project.bat
```
- g++ 4.8.1 or newer(link pthread because gcc requires it!!)
```
# with cmake
cd build
cmake .
make
# or manual build without cmake
cd build
g++ -std=c++11 -pthread ../unittest/unittest.cpp ../src/zupply.cpp -lpthread -o unittest
```
- Clang++ 3.3 or newer
```
# using cmake is identical to gcc
# or manually build with clang++
cd build
clang++ -std=c++11 ../unittest/unittest.cpp ../src/zupply.cpp -o unittest
```

## Documentation
Full [documentation](http://zhreshold.github.io/zupply/) supplied.

## Tutorials
For tutorials, please check Zupply [Wiki](https://github.com/ZhreShold/zupply/wiki)!

## License
Zupply uses very permissive [MIT](https://opensource.org/licenses/MIT) license.

## Credits
- [UTF8-CPP](http://utfcpp.sourceforge.net/)
- [STB-Image](https://github.com/nothings/stb)
