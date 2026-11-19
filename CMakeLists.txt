cmake_minimum_required(VERSION 3.21)
project(NumericalStaticProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Статична бібліотека та основна демонстраційна програма
add_subdirectory(NumericalDiff)
add_subdirectory(App)

# 2. Демонстрація антипатернів
add_executable(AntipatternDemo
    antipattern/main.cpp
)

# 3. Модульні тести Google Test
find_package(GTest CONFIG REQUIRED)
enable_testing()

add_executable(DifferentiationTests
    tests/DifferentiationTests.cpp
)

target_link_libraries(DifferentiationTests
    PRIVATE
        NumericalDiff
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(DifferentiationTests)