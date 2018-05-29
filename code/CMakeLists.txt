cmake_minimum_required (VERSION 3.0.0)

project (search CXX)

set (CXX11_FOUND "1")

set (CMAKE_PREFIX_PATH "seqan/util/cmake")
set (SEQAN_INCLUDE_PATH "seqan/include")

find_package (SeqAn REQUIRED)

include_directories (${SEQAN_INCLUDE_DIRS})
add_definitions (${SEQAN_DEFINITIONS})

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SEQAN_CXX_FLAGS} -std=c++14 -DSEQAN_DISABLE_VERSION_CHECK=YES")
set (CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} ${SEQAN_CXX_FLAGS} -std=c++14 -DSEQAN_DISABLE_VERSION_CHECK=YES")

add_executable (benchmark_create_index benchmark_create_index.cpp)
target_link_libraries (benchmark_create_index ${SEQAN_LIBRARIES})

add_executable (randomizeN randomizeN.cpp)
target_link_libraries (randomizeN ${SEQAN_LIBRARIES})

add_executable (benchmark benchmark.cpp)
target_link_libraries (benchmark ${SEQAN_LIBRARIES})

