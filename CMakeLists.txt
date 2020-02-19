cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Options are: Debug or Release (case sensitive)")

set(EXE mabe)
project(${EXE})

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
  OUTPUT_QUIET
  )

add_executable(${EXE} main.cpp)

## set standard to c++17
set(CMAKE_CXX_STANDARD 17)
target_compile_features(${EXE} PRIVATE cxx_std_17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Useful directory-crawling macro
## example:
## SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})
## FOREACH(subdir ${SUBDIRS})
##   ADD_SUBDIRECTORY(${subdir})
## ENDFOREACH()
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# main already included as entrypoint
target_sources(${EXE} PRIVATE Global.cpp)

include(Analyze/CMakeLists.txt)
include(Archivist/CMakeLists.txt)
include(Brain/CMakeLists.txt)
include(Genome/CMakeLists.txt)
include(Group/CMakeLists.txt)
include(Optimizer/CMakeLists.txt)
include(Organism/CMakeLists.txt)
include(Utilities/CMakeLists.txt)
include(World/CMakeLists.txt)

include_directories(.)
