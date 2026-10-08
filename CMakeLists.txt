cmake_minimum_required(VERSION 3.10)
project(CompilerOptimizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(optimizer
    src/main.cpp
    src/IR.cpp
    src/ConstantFolding.cpp
    src/ConstantPropagation.cpp
    src/DeadCodeElimination.cpp
    src/Optimizer.cpp
)

enable_testing()
add_test(NAME all_cases
         COMMAND optimizer ${CMAKE_SOURCE_DIR}/tests/test_cases.txt --quiet)
