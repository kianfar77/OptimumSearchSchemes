cmake_minimum_required (VERSION 3.0.0)

project (search CXX)

set (CXX11_FOUND "1")

if (NOT DEFINED ${CMAKE_BUILD_TYPE})
    set (CMAKE_BUILD_TYPE Release)
    message("*** CMAKE_BUILD_TYPE was not passed to CMake, therefore it was set to Release. ***")
endif()

set (CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/seqan/util/cmake")
set (SEQAN_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/seqan/include")

find_package (SeqAn REQUIRED)
find_package (OpenMP)

include_directories (${SEQAN_INCLUDE_DIRS})
add_definitions (${SEQAN_DEFINITIONS})

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SEQAN_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -std=c++14 -DSEQAN_DISABLE_VERSION_CHECK=YES")
set (CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} ${SEQAN_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -std=c++14 -DSEQAN_DISABLE_VERSION_CHECK=YES")

include (ExternalProject)
set (GBENCHMARK_CMAKE_ARGS "-DBENCHMARK_ENABLE_TESTING=false;-DCMAKE_BUILD_TYPE=Release;-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}")
ExternalProject_Add (
    gbenchmark_project
    PREFIX gbenchmark_project
    GIT_REPOSITORY "https://github.com/google/benchmark.git"
    GIT_TAG "v1.4.1"
    CMAKE_ARGS ${GBENCHMARK_CMAKE_ARGS}
    UPDATE_DISCONNECTED yes
)

add_executable (build_index build_index.cpp)
target_link_libraries (build_index ${SEQAN_LIBRARIES})

add_executable (count_edges count_edges.cpp)
target_link_libraries (count_edges ${SEQAN_LIBRARIES})

add_executable (benchmark benchmark.cpp)
add_dependencies(benchmark gbenchmark_project)
set_source_files_properties(benchmark.cpp PROPERTIES COMPILE_FLAGS "-I${CMAKE_CURRENT_BINARY_DIR}/include")
target_link_libraries (benchmark "${SEQAN_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib -lbenchmark")

add_executable (randomizeN randomizeN.cpp)
target_link_libraries (randomizeN ${SEQAN_LIBRARIES})

add_executable (detailed_benchmark detailed_benchmark.cpp)
target_link_libraries (detailed_benchmark ${SEQAN_LIBRARIES})
