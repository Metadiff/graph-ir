cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0
project(metadiff-core)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Standalone build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
    if(NOT DEFINED BUILD_EXAMPLES)
        set(BUILD_EXAMPLES 1)
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set(CMAKE_CXX_FLAGS "-Wall -Werror=return-type -Wno-unknown-pragmas -Wno-sign-compare")
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
        ${PROJECT_SOURCE_DIR}/src/visual/cytoscape.cpp
        )

# Add gmock only if it has not been added by a parental project
add_executable(try src/test.cpp ${METADIFF_SOURCES})
add_dependencies(try symints_s)

# Build tests
#add_subdirectory(tests)

# Static registry example
#add_executable(example_static_registry examples/example.cpp ${SYMBOLIC_INTEGERS_SOURCES})

# Dynamic registry example
#add_executable(example_dynamic_registry examples/example.cpp ${SYMBOLIC_INTEGERS_SOURCES})
#target_compile_definitions(example_dynamic_registry PRIVATE METADIFF_SYMBOLIC_INTEGERS_DYNAMIC_REGISTRY)
