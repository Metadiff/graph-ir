cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0
project(metadiff-core)

SET(CORE_SHARED 1)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Standalone build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
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
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/symints")
endif()

# Add symbolic_integers
add_subdirectory(external/symbolic_integers)
include_directories(external/symbolic_integers/include)
# Add spdlog
include_directories(external/spdlog/include)
# Add my include directories
include_directories(include)
# Link with symints, we use static libs
link_libraries(symints_s)
# Link with dl, for dynamic loading of the shared library generated
link_libraries(dl)
## curl is needed for downloading of files, used in utils
#link_libraries(curl)

set(METADIFF_SOURCES
        ${PROJECT_SOURCE_DIR}/src/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/definitions.cpp
        ${PROJECT_SOURCE_DIR}/src/props.cpp
        ${PROJECT_SOURCE_DIR}/src/helpers.cpp
        ${PROJECT_SOURCE_DIR}/src/node.cpp
        ${PROJECT_SOURCE_DIR}/src/graph.cpp
        ${PROJECT_SOURCE_DIR}/src/print.cpp
        ${PROJECT_SOURCE_DIR}/src/exceptions.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/abstract.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/input.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/constant.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/special.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/shape.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/logical.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/arithmetic.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/reduction.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/elementwise.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/linalg.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/optimized.cpp
        ${PROJECT_SOURCE_DIR}/src/operators/random.cpp
        ${PROJECT_SOURCE_DIR}/src/export/cytoscape.cpp
        )

# Build libs (static by default)
if(CORE_SHARED)
    # Create shared libs
    add_library(md_core SHARED ${METADIFF_SOURCES})
else()
    # Create static library
    add_library(md_core STATIC ${METADIFF_SOURCES})
endif()

# Build test example
add_executable(try src/test.cpp)
target_link_libraries(try md_core)

