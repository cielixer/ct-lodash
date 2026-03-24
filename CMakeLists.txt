cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Out-of-source build guard
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not permitted. "
    "Please create a separate build directory and run CMake from there:\n"
    "  cmake -S . -B build -G Ninja\n"
    "  cmake --build build\n"
  )
endif()

project(ctl
  VERSION 0.1.0
  DESCRIPTION "Lodash for C++ structs: header-only C++23 functional utilities"
  LANGUAGES CXX
)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Find Boost dependencies
find_package(Boost REQUIRED COMPONENTS headers)

# Enable testing only when this is the top-level project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(BUILD_TESTING "Enable testing" ON)
  if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
  endif()
endif()

# Header-only INTERFACE library
add_library(ctl INTERFACE)
add_library(ctl::ctl ALIAS ctl)

# Enforce C++23
target_compile_features(ctl INTERFACE cxx_std_23)

# Public headers
target_include_directories(ctl INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Link Boost
target_link_libraries(ctl INTERFACE Boost::headers)

# Examples
add_subdirectory(examples)

# Installation rules
include(GNUInstallDirs)

# Install INTERFACE target with export
install(
  TARGETS ctl
  EXPORT ctlTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(
  DIRECTORY include/ctl
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.hpp"
)

# Install export set
install(
  EXPORT ctlTargets
  FILE ctlTargets.cmake
  NAMESPACE ctl::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctl
)

# Export targets to build tree for consumers (must come after install(EXPORT))
export(EXPORT ctlTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/ctlTargets.cmake"
       NAMESPACE ctl::)

# Generate package config files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ctlConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctlConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ctlConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctl
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ctlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ctlConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctl
)
