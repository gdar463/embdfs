project(
    embdfs-lib
    VERSION 0.1
    DESCRIPTION "Embed files in c++"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

###
### Compiler Flags
###
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unknown-pragmas)
endif()

if(NOT DEFINED USE_ASAN)
    set(USE_ASAN OFF)
endif()

###
### Generator
###
set(EMBDFS_FILES "")
foreach(path ${EMBDFS_PATH})
    file(GLOB_RECURSE EMBDFS_FILES_PART "${path}/*")
    list(APPEND EMBDFS_FILES ${EMBDFS_FILES_PART})
endforeach()

add_custom_command(OUTPUT ${EMBDFS_GENERATED_RESOURCES}
    COMMAND ${EMBDFS_GENERATOR}
    DEPENDS generator ${EMBDFS_FILES}
    VERBATIM)

add_custom_target(generate_resources DEPENDS ${EMBDFS_GENERATED_RESOURCES})

###
### Project
###
file(GLOB_RECURSE EMBDFS_HEADERS CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE EMBDFS_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp")

add_library(embdfs-lib STATIC
    ${EMBDFS_GENERATED_RESOURCES}
    ${EMBDFS_HEADERS}
    ${EMBDFS_SOURCES})

add_dependencies(embdfs-lib generate_resources)

target_include_directories(embdfs-lib PRIVATE
    include)

target_link_libraries(embdfs-lib PRIVATE
    libdeflate_static)

target_compile_features(embdfs-lib PUBLIC cxx_std_23)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${EMBDFS_HEADERS})
source_group(
    TREE "${PROJECT_SOURCE_DIR}/src"
    PREFIX "Source Files"
    FILES ${EMBDFS_SOURCES})

if(DEBUG)
    target_compile_definitions(embdfs-lib PRIVATE DEBUG)
endif()

if(UNIX AND DEBUG AND USE_ASAN)
    target_compile_options(embdfs-lib PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )

    target_link_options(embdfs-lib PRIVATE
        -fsanitize=address,undefined
    )
endif()
