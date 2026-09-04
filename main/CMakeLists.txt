project(embdfs-main LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(embdfs-main src/main.cpp)
target_link_libraries(embdfs-main PRIVATE embdfs-lib)
