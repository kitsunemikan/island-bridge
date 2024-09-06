cmake_minimum_required(VERSION 3.20)
project(island-bridge)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(range-v3 REQUIRED)
find_package(rang REQUIRED)
find_package(cxxopts REQUIRED)

add_library(alg STATIC
    src/alg/alg.hpp
    src/alg/no_fancy.cpp
    src/alg/with_ranges.cpp
)
target_compile_features(alg PRIVATE cxx_std_17)
target_include_directories(alg PUBLIC src/)
target_link_libraries(alg PUBLIC range-v3::range-v3)

add_executable(test
    vendor/boost/ut.hpp
    src/alg.test.cpp
)
target_compile_features(test PUBLIC cxx_std_20)
target_include_directories(test PRIVATE src/ vendor/)
target_link_libraries(test PRIVATE alg)

# MSVC can't work with modules well
if (MSVC)
    target_compile_definitions(test PRIVATE BOOST_UT_DISABLE_MODULE)
endif()

add_executable(benchmark
    src/benchmark.main.cpp
)
target_link_libraries(benchmark PRIVATE alg rang::rang cxxopts::cxxopts)
