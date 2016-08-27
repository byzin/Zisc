# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2016 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.4)


## The project information
project(Zisc LANGUAGES CXX)
include(${PROJECT_SOURCE_DIR}/cmake/project.cmake)
setZiscProperties()
message(STATUS "## Zisc version: ${PROJECT_VERSION}")

# Debug info
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Build configuration
include(${PROJECT_SOURCE_DIR}/cmake/general.cmake)
restrictBuildDirectory(${PROJECT_SOURCE_DIR}/build)
detectEnvironment(environment_definitions)
# Command options
include(${PROJECT_SOURCE_DIR}/cmake/option.cmake)
setCommandOption()
#
include(${PROJECT_SOURCE_DIR}/cmake/compiler.cmake)
setCompilerOption()
getCompilerOption(cxx_compile_flags cxx_linker_flags cxx_definitions)

# Load zisc
include(${PROJECT_SOURCE_DIR}/source/zisc/config.cmake)
loadZisc(zisc_header_files zisc_include_dirs zisc_compile_flags zisc_linker_flags zisc_definitions)

# Build example
include(${PROJECT_SOURCE_DIR}/example/config.cmake)
buildExample()

# Build unit test
include(${PROJECT_SOURCE_DIR}/test/config.cmake)
buildUnitTest()
