cmake_minimum_required(VERSION 4.0)
project(KVDatabase)

set(CMAKE_CXX_STANDARD 20)

add_executable(KVDatabase main.cpp
        avl_tree.cpp
        avl_tree.h
)
