cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0

# Project name and versions
project(GraphIR)
set(GRAPH_IR_MAJOR_VERSION 0)
set(GRAPH_IR_INTEGERS_VERSION 0)
set(GRAPH_IR_INTEGERS_VERSION 1)
set(GRAPH_IR__VERSION
        ${GRAPH_IR_MAJOR_VERSION}.${GRAPH_IR_MINOR_VERSION}.${GRAPH_IR_PATCH_VERSION})

# *** DEBUG ***
set(GRAPH_IR_SHARED 1)
# *** DEBUG ***

# Build flags
if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Standalone build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
    if(NOT DEFINED BUILD_EXAMPLES)
        set(BUILD_EXAMPLES 1)
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set(CMAKE_CXX_FLAGS "-fPIC -Wall -Werror=return-type -Wno-unknown-pragmas -Wno-sign-compare")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wwrite-strings -Wno-unused")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    endif()
else()
    # Build under anohter project umbrella
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gir")
endif()

# Add symbolic_integers
add_subdirectory(external/symbolic-integers)
include_directories(external/symbolic-integers/include)
# Add spdlog
include_directories(external/spdlog/include)
# Add rapidjson
include_directories(external/rapidjson/include)
# Add my include directories
include_directories(include)
# Link with symints, we use static libs
#link_libraries(symints_s)
# Link with dl, for dynamic loading of the shared library generated
#link_libraries(dl)
## curl is needed for downloading of files, used in utils
#link_libraries(curl)

# All of the project source files
set(API_SOURCES
        ${PROJECT_SOURCE_DIR}/src/api/input.cpp
        ${PROJECT_SOURCE_DIR}/src/api/constant.cpp
        ${PROJECT_SOURCE_DIR}/src/api/special.cpp
        ${PROJECT_SOURCE_DIR}/src/api/shape.cpp
        ${PROJECT_SOURCE_DIR}/src/api/logical.cpp
        ${PROJECT_SOURCE_DIR}/src/api/arithmetic.cpp
        ${PROJECT_SOURCE_DIR}/src/api/reduction.cpp
        ${PROJECT_SOURCE_DIR}/src/api/elementwise.cpp
        ${PROJECT_SOURCE_DIR}/src/api/linalg.cpp
        ${PROJECT_SOURCE_DIR}/src/api/random.cpp
        ${PROJECT_SOURCE_DIR}/src/api/debug.cpp
        ${PROJECT_SOURCE_DIR}/src/api/optimized.cpp
        ${PROJECT_SOURCE_DIR}/src/api/grad.cpp
        )

set(GRAPH_IR_SOURCES
        ${PROJECT_SOURCE_DIR}/src/props.cpp
        ${PROJECT_SOURCE_DIR}/src/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/node.cpp
        ${PROJECT_SOURCE_DIR}/src/graph.cpp
        ${PROJECT_SOURCE_DIR}/src/print.cpp
        ${PROJECT_SOURCE_DIR}/src/abstract_operator.cpp
        ${PROJECT_SOURCE_DIR}/src/export/cytoscape.cpp
        ${PROJECT_SOURCE_DIR}/src/export/json.cpp
        )

list(APPEND GRAPH_IR_SOURCES ${API_SOURCES})

# Append them to all of the source fiels
list(APPEND ALL_SOURCES ${GRAPH_IR_SOURCES})

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Build under anohter project umbrella
    set (ALL_SRORCES ${ALL_SRORCES} PARENT_SCOPE)
endif()

# Build libs (static by default)
if(GRAPH_IR_SHARED)
    # Create shared libs
    add_library(graph_ir SHARED ${ALL_SOURCES})
else()
    # Create static library
    add_library(graph_ir STATIC ${ALL_SOURCES})
endif()

# Build test example
add_executable(try examples/src/test.cpp)
target_link_libraries(try graph_ir)

