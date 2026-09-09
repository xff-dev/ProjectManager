cmake_minimum_required(VERSION 3.12)
project(project-manager)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(inipp GIT_REPOSITORY https://github.com/mcmtroffaes/inipp.git GIT_TAG 1.0.13)
FetchContent_MakeAvailable(inipp)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.4.0)
FetchContent_MakeAvailable(Catch2)


file(GLOB_RECURSE LIB_SOURCES src/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/main.cpp") 

add_library(pm-logic STATIC ${LIB_SOURCES})
target_link_libraries(pm-logic PUBLIC inipp::inipp)

add_executable(pm src/main.cpp)
target_link_libraries(pm PRIVATE  pm-logic)


file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE pm-logic Catch2::Catch2WithMain)


install(TARGETS pm DESTINATION bin)


