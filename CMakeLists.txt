# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.22)


project(ZiscProject VERSION 0.0.72 LANGUAGES CXX)

cmake_path(SET zisc_path "${PROJECT_SOURCE_DIR}/source/zisc")
if(PROJECT_IS_TOP_LEVEL)
  ## CMake configurations
  set(CMAKE_CONFIGURATION_TYPES "Debug" "RelWithDebInfo" "Release")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
  endif()
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  include(InstallRequiredSystemLibraries)
  ## Print system environment information
  include("${zisc_path}/cmake/general.cmake")
  message(STATUS "System information:")
  Zisc_printSystemInfo("  + ")
  ## Project options
  include("${PROJECT_SOURCE_DIR}/cmake/option.cmake")
  initProjectOptions()
  Zisc_restrictBuildDirectory("${PROJECT_SOURCE_DIR}/build")
endif()

# Build Zisc
add_subdirectory("${zisc_path}" "${PROJECT_BINARY_DIR}/Zisc")

if(PROJECT_IS_TOP_LEVEL OR ZISC_BUILD_EXAMPLES OR ZISC_BUILD_TESTS)
  include("${PROJECT_SOURCE_DIR}/cmake/project.cmake")
  setZiscAlias()
endif()

# Build examples
if(ZISC_BUILD_EXAMPLES)
  add_subdirectory("${PROJECT_SOURCE_DIR}/example" "${PROJECT_BINARY_DIR}/Examples")
endif()

# Build unit test
if(ZISC_BUILD_TESTS)
  addGoogleTest("${PROJECT_BINARY_DIR}/GoogleTest")
  enable_testing()
  add_subdirectory("${PROJECT_SOURCE_DIR}/test" "${PROJECT_BINARY_DIR}/UnitTest")
endif()

if(PROJECT_IS_TOP_LEVEL)
  # Build documents
  add_subdirectory("${PROJECT_SOURCE_DIR}/document" "${PROJECT_BINARY_DIR}/Document")

  # Packaging
  include("${PROJECT_SOURCE_DIR}/cmake/packaging.cmake")
  packageProject()
endif()
