cmake_minimum_required(VERSION 2.8) # Could be even lower. Works with 3.0

# Project name and versions
project(MockBackend)
set(MockBackend_MAJOR_VERSION 0)
set(MockBackend_INTEGERS_VERSION 0)
set(MockBackend_INTEGERS_VERSION 1)
set(MockBackend__VERSION ${MockBackend_MAJOR_VERSION}.${MockBackend_MINOR_VERSION}.${MockBackend_PATCH_VERSION})

# Include directory
include_directories(include)

# Source files
set(MOCK_BACKEND_SOURCES
        ${PROJECT_SOURCE_DIR}/src/backend.cpp
        ${PROJECT_SOURCE_DIR}/src/source_gen.cpp
        ${PROJECT_SOURCE_DIR}/src/evaluator.cpp
        )

# Build library
if(DEFINED GRAPH_IR_SHARED)
    set(MOCK_BACKEND_SHARED GRAPH_IR_SHARED)
else()
    set(MOCK_BACKEND_SHARED 1)
endif()
if(MOCK_BACKEND_SHARED)
    add_library(mock_backend SHARED ${MOCK_BACKEND_SOURCES})
else()
    add_library(mock_backend STATIC ${MOCK_BACKEND_SOURCES})
endif()
