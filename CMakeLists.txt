cmake_minimum_required(VERSION 3.5.2)
project(poincare)

# The version number.
set (poincare_VERSION_MAJOR 0)
set (poincare_VERSION_MINOR 1)

include_directories(src)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE " -pthread -std=c++11 -funroll-loops -Ofast")
set(CMAKE_CXX_FLAGS_DEBUG " -pthread -std=c++11 -g -O0 -fno-inline -Wfatal-errors")

set(HEADER_FILES
    src/args.h
    src/digraph.h
    src/sampler.h
    src/poincare.h
    src/model.h
    src/real.h
    src/vector.h)

set(SOURCE_FILES
    src/args.cc
    src/digraph.cc
    src/sampler.cc
    src/poincare.cc
    src/main.cc
    src/model.cc
    src/vector.cc)

# Compile static library from source files
add_library(poincare-static STATIC ${SOURCE_FILES} ${HEADER_FILES})
set_target_properties(poincare-static PROPERTIES OUTPUT_NAME poincare)

# Make executable
add_executable(poincare-bin src/main.cc)
target_link_libraries(poincare-bin pthread poincare-static)
set_target_properties(poincare-bin PROPERTIES PUBLIC_HEADER "${HEADER_FILES}" OUTPUT_NAME poincare)
