cmake_minimum_required(VERSION 3.10)

project(GlobalRegisterAllocator
    VERSION 1.0
    DESCRIPTION "Global register allocation using graph coloring heuristics"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    src/main.cpp
    src/GraphColoring.cpp
    src/Menu.cpp
    src/Output.cpp
    src/Parser.cpp
)

set(HEADERS
    include/Graph.h
    include/GraphColoring.h
    include/DataStructures.h
    include/Parser.h
    include/Output.h
    include/Menu.h
)

add_executable(myProg ${SOURCES} ${HEADERS})

target_include_directories(myProg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
