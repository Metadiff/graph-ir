cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0

# Project name and versions
project(GraphIR)
set(GRAPH_IR_MAJOR_VERSION 0)
set(GRAPH_IR_INTEGERS_VERSION 0)
set(GRAPH_IR_INTEGERS_VERSION 1)
set(GRAPH_IR__VERSION ${GRAPH_IR_MAJOR_VERSION}.${GRAPH_IR_MINOR_VERSION}.${GRAPH_IR_PATCH_VERSION})

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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
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
# Add boost
add_definitions(-DBOOST_SYSTEM_SOURCE=1)
add_definitions(-DBOOST_FILESYSTEM_SOURCE=1)
add_definitions(-DBOOST_SYSTEM_NO_DEPRECATED=1)
include_directories(external/system/include external/filesystem/include)
# Finally include this project directories
include_directories(include)

set(BOOST_SOURCES
        ${PROJECT_SOURCE_DIR}/external/system/src/error_code.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/codecvt_error_category.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/operations.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/path.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/path_traits.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/portability.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/unique_path.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/utf8_codecvt_facet.cpp
        ${PROJECT_SOURCE_DIR}/external/filesystem/src/windows_file_codecvt.cpp)

# All of the api source files
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
# All of the graph IR sources
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

# Add All extra sources
list(APPEND GRAPH_IR_SOURCES ${API_SOURCES})
list(APPEND GRAPH_IR_SOURCES ${BOOST_SOURCES})

# Append them to all of the source fiels
list(APPEND ALL_SOURCES ${GRAPH_IR_SOURCES})

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Build under anohter project umbrella
    set (ALL_SRORCES ${ALL_SRORCES} PARENT_SCOPE)
endif()

# Build library
if(NOT DEFINED GRAPH_IR_SHARED)
    set(GRAPH_IR_SHARED 1)
endif()
if(GRAPH_IR_SHARED)
    add_library(graph_ir SHARED ${ALL_SOURCES})
else()
    add_library(graph_ir STATIC ${ALL_SOURCES})
endif()

# Build Mock Backend
if(NOT DEFINED BUILD_MOCK_BACKEND)
    set(BUILD_MOCK_BACKEND 1)
endif()
if(BUILD_MOCK_BACKEND)
    add_subdirectory(mock_backend)
    # Build test example
    add_executable(try examples/src/test.cpp)
    target_include_directories(try PUBLIC mock_backend/include)
    # Link libraries in correct order
    target_link_libraries(try graph_ir mock_backend dl)
endif()


