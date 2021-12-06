cmake_minimum_required(VERSION 3.17)
project(R8PD VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(wininet SHARED main.cpp)