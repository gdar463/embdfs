cmake_minimum_required(VERSION 3.21)

project(embdfs LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG ON)
else()
    set(DEBUG OFF)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT DEFINED EMBDFS_PATH)
    message(FATAL_ERROR "EMBDFS_PATH is not defined.")
endif()

add_subdirectory(third_party)

include(ExternalProject)

set(EMBDFS_GENERATOR "${CMAKE_CURRENT_BINARY_DIR}/generator/embdfs_generator")
set(EMBDFS_GENERATED_RESOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/generator/embdfs_generated.cpp")
ExternalProject_Add(generator
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generator"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generator"

    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DEMBDFS_PATH=${EMBDFS_PATH}
        -DEMBDFS_GENERATED_RESOURCES=${EMBDFS_GENERATED_RESOURCES}

    BUILD_COMMAND
        ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}

    INSTALL_COMMAND ""

    BUILD_BYPRODUCTS
        "${EMBDFS_GENERATOR}")

add_subdirectory(lib)
