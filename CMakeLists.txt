cmake_minimum_required(VERSION 2.6)

project(PropagationContraintes)

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11")

file(
	GLOB_RECURSE
	source_files
	src/*
)

add_executable(
    main
    ${source_files}
)
