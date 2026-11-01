
cmake_minimum_required(VERSION 3.21)

project(Galactic)

if (NOT DEFINED GEODE_SDK)
    message(FATAL_ERROR "GEODE_SDK is missing")
endif()

add_subdirectory(
    ${GEODE_SDK}
    ${CMAKE_CURRENT_BINARY_DIR}/geode
)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
