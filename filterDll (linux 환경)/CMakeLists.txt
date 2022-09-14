cmake_minimum_required (VERSION 3.0)
project (filterDll)

set (CMAKE_VERBOSE_MAKEFILE on)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories(
    .
    ${EIGEN3_INCLUDE_DIR}
)

## .so 颇老 积己
add_library (filterDll SHARED
    FilterDll.cpp
    FilterLowPass.cpp
)

## .a 颇老 积己
#add_library (filterDll
#    FilterDll.cpp
#    FilterLowPass.cpp
#)

target_link_libraries (filterDll Eigen3::Eigen)

add_executable(filterLibTest FilterLibTest.cpp)
target_link_libraries (filterLibTest filterDll)