cmake_minimum_required(VERSION 3.16)
project(dadiff VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC
	compressor.cpp
	compressor.h
	config.h
	main.cpp
)
set(PCH pch.h)

add_executable(dadiff ${SRC} ${PCH})
target_precompile_headers(dadiff PRIVATE ${PCH})

if(CMAKE_BUILD_TYPE AND (${CMAKE_BUILD_TYPE} STREQUAL "Release"))
	if(MSVC)
		set_property(TARGET dadiff PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -static -static-libgcc -static-libstdc++")
	endif()
endif()
