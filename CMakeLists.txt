cmake_minimum_required(VERSION 3.15)
project(mpm)

set(CMAKE_CXX_STANDARD 17)

add_executable(mpm main.cpp Net.cpp Net.h)
find_package(cJSON CONFIG REQUIRED)
target_link_libraries(mpm PRIVATE cjson)
find_package(CURL CONFIG REQUIRED)
target_link_libraries(mpm PRIVATE CURL::libcurl)