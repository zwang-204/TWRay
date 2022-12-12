cmake_minimum_required (VERSION 3.10) 

project (RayTracing)

SET(CMAKE_CXX_STANDARD 11) 
SET(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")

add_executable(RT main.cc)