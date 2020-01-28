cmake_minimum_required(VERSION 3.15.3)
option(BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE ${BUILD_TYPE})

set(EXE mabe)
project(${EXE})

## set path to cmake modules for finding mkl, etc.
set(CMAKE_MODULE_PATH ${CMAKE_MODULEPATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# OS-specific alterations
if (MSYS OR MINGW)
  # msys2 environment struggles with compiler detection
  message(STATUS "detected platform: msys/mingw on windows")
  set(CMAKE_C_COMPILER gcc)
  set(CMAKE_CXX_COMPILER g++)
endif()
if (MSVC OR WIN32)
  message(STATUS "detected platform: visual studio on windows")
endif()
if (UNIX AND NOT APPLE)
  message(STATUS "detected platform: linux")
endif()
if (APPLE)
  message(STATUS "detected platform: apple")
endif()

# read buildOptions
# produce modules.h, cmake_auto_injection.txt
message(STATUS "building modules.h...")
find_package(Python3 COMPONENTS Interpreter)
if (NOT Python3_FOUND)
  message(FATAL_ERROR " Python3 required for pythonTools/mbuild.py functionality")
endif()
execute_process(
  COMMAND ${Python3_EXECUTABLE} pythonTools/mbuild.py -nc
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_executable(${EXE} main.cpp)

# include cmake_auto_injection.txt (mabe build config)
include(cmake_auto_injection.txt)

## set standard to c++17
set(CMAKE_CXX_STANDARD 17)
target_compile_features(${EXE} PRIVATE cxx_std_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(REMOVE cmake_auto_injection.txt)
