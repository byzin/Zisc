# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.17)


project(ZiscProject VERSION 0.0.1 LANGUAGES CXX)

## CMake configurations
set(CMAKE_CONFIGURATION_TYPES "Debug" "RelWithDebInfo" "Release")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(InstallRequiredSystemLibraries)

set(zisc_path ${PROJECT_SOURCE_DIR}/source/zisc)
include(${zisc_path}/cmake/general.cmake)
message(STATUS "System information:")
Zisc_printSystemInfo("  + ")
## Project options
include(${PROJECT_SOURCE_DIR}/cmake/option.cmake)
initProjectOptions()
Zisc_restrictBuildDirectory(${PROJECT_SOURCE_DIR}/build)

# Build Zisc
add_subdirectory(${zisc_path} ${PROJECT_BINARY_DIR}/Zisc)

# Build examples
add_subdirectory(${PROJECT_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/Examples)

# Build unit test
if(ZISC_BUILD_TESTS)
  include(${PROJECT_SOURCE_DIR}/cmake/googletest.cmake)
  addZiscGoogleTest()
  add_subdirectory(${PROJECT_SOURCE_DIR}/test ${PROJECT_BINARY_DIR}/UnitTest)
  add_subdirectory(${PROJECT_SOURCE_DIR}/test/math_reference ${PROJECT_BINARY_DIR}/MathReference)
endif()

# Build documents
add_subdirectory(${PROJECT_SOURCE_DIR}/document ${PROJECT_BINARY_DIR}/Document)
