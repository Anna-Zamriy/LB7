cmake_minimum_required(VERSION 3.21)

add_library(NumericalDiff STATIC
    src/NumericalDiff.cpp
    src/DifferentiationTask.cpp
)

target_include_directories(NumericalDiff PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(NumericalDiff PUBLIC cxx_std_20)