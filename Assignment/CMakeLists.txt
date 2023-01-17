cmake_minimum_required(VERSION 3.20)
project(Assignment)

set(CMAKE_CXX_STANDARD 11)

add_executable(Assignment main.cpp Competitor.cpp ThreadMap.cpp Results.cpp Results.h FinishAgent.cpp FinishAgent.h)
