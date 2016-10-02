cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0
project(metadiff-core)

if( CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR )
    # Standalone build
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set(CMAKE_CXX_FLAGS "-Wall -Werror=return-type -Wno-unknown-pragmas -Wno-sign-compare")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wwrite-strings -Wno-unused")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    endif()
endif()
add_subdirectory(external/symbolic_integers)

include_directories(external/spdlog/include)
include_directories(external/symbolic_integers/include)
include_directories(include)

# dl is needed for dynamic loading of the shared library generated
link_libraries(dl)
## curl is needed for downloading of files, used in utils
#link_libraries(curl)
link_libraries(symints)

set(METADIFF_SOURCES
        ${PROJECT_SOURCE_DIR}/src/utils.cpp
        ${PROJECT_SOURCE_DIR}/src/print.cpp)
# Add gmock only if it has not been added by a parental project
add_executable(try src/test.cpp ${METADIFF_SOURCES})

# Build tests
#add_subdirectory(tests)

# Static registry example
#add_executable(example_static_registry examples/example.cpp ${SYMBOLIC_INTEGERS_SOURCES})

# Dynamic registry example
#add_executable(example_dynamic_registry examples/example.cpp ${SYMBOLIC_INTEGERS_SOURCES})
#target_compile_definitions(example_dynamic_registry PRIVATE METADIFF_SYMBOLIC_INTEGERS_DYNAMIC_REGISTRY)
