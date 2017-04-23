cmake_minimum_required(VERSION 3.1)
project(post_stretch)

set (CMAKE_CXX_STANDARD 11)

add_definitions (-DBOOST_ALL_DYN_LINK)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)


