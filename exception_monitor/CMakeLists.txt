cmake_minimum_required(VERSION 3.23)
project(jvmti)

set(CMAKE_CXX_STANDARD 14)

add_library(jvmti SHARED exception_monitor.cpp)
include_directories("/usr/local/Cellar/openjdk/19/include/")