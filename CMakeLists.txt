PROJECT(FBIMiner CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -O2 -Wno-sign-compare -Wextra")
FILE(GLOB_RECURSE DataSrcs data/*.cpp)
FILE(GLOB_RECURSE AlgSrcs algorithms/*.cpp)
FILE(GLOB_RECURSE UtilSrcs util/*.cpp)
ADD_EXECUTABLE(FBIMiner main.cpp ${DataSrcs} ${AlgSrcs} ${UtilSrcs})