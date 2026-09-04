cmake_minimum_required(VERSION 3.21)

project(embdfs-generator
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_subdirectory(../third_party "${CMAKE_CURRENT_BINARY_DIR}/third_party")

add_executable(embdfs_generator
    src/generator.cpp)

target_link_libraries(embdfs_generator PRIVATE libdeflate_static)
target_compile_definitions(embdfs_generator PRIVATE
    EMBDFS_PATH="${EMBDFS_PATH}"
    EMBDFS_GENERATED_RESOURCES="${EMBDFS_GENERATED_RESOURCES}")

if(WIN32)
    target_compile_definitions(embdfs_generator PRIVATE WIN32)
endif()
