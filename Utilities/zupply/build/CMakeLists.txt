CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
project(zupply)
IF (WIN32)
    # FOR windows
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
    SET(CMAKE_EXE_LINKER_FLAGS "-lpthread")
ELSE()
    SET(CMAKE_CXX_FLAGS "-std=c++11")
ENDIF()

add_executable(unittest
				../unittest/unittest.cpp
				../src/zupply.hpp
				../src/zupply.cpp)

add_executable(quickstart
				../src/quickstart.cpp
				../src/zupply.hpp
				../src/zupply.cpp)