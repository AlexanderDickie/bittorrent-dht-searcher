cmake_minimum_required(VERSION 3.22)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled main.cpp network.cpp network.h searcher.cpp searcher.h parse_message.cpp parse_message.h utils.cpp utils.h)
