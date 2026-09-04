add_compile_options(-w)

set(LIBDEFLATE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(libdeflate)
