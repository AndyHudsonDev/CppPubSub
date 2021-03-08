cmake_minimum_required(VERSION 3.19)
project(pubsub)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror -std=c++20")

FIND_PACKAGE( Boost 1.75 REQUIRED )
FIND_PACKAGE (Threads)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )


set(SOURCE_FILES main.cpp pubsub.hpp)
add_executable(pubsub ${SOURCE_FILES})
target_link_libraries (pubsub ${CMAKE_THREAD_LIBS_INIT} -fsanitize=thread)

