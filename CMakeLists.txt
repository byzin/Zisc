# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.14)


function(checkSubmodule submodule_path)
  if(NOT EXISTS ${submodule_path})
    get_filename_component(submodule_name "${submodule_path}" NAME)
    message(FATAL_ERROR "Submodule '${submodule_name}' not found, please init submodules.")
  endif()
endfunction()

## The project information
project(Zisc LANGUAGES C CXX)
include(${PROJECT_SOURCE_DIR}/cmake/project.cmake)
setZiscProperties()
message(STATUS "Zisc version: ${PROJECT_VERSION}")

# Build configuration
set(CMAKE_CONFIGURATION_TYPES "Debug" "RelWithDebInfo" "Release")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build configuration
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)
restrictBuildDirectory(${PROJECT_SOURCE_DIR}/build)
initPlatform(platform_definitions)
# Command options
include(${PROJECT_SOURCE_DIR}/cmake/option.cmake)
initCommandOptions()
#
include(${PROJECT_SOURCE_DIR}/cmake/compiler.cmake)
initCompilerOptions()
getCxxCompilerFlags(cxx_compile_flags cxx_linker_flags cxx_definitions)

# Load zisc
include(${PROJECT_SOURCE_DIR}/source/zisc/config.cmake)
loadZisc(zisc_header_files
         zisc_include_dirs
         zisc_compile_flags
         zisc_linker_flags
         zisc_definitions)
# Thread
find_package(Threads REQUIRED)

# Build example
include(${PROJECT_SOURCE_DIR}/example/config.cmake)
buildExample()

# Build unit test
if(ZISC_BUILD_TESTS)
  include(${PROJECT_SOURCE_DIR}/test/config.cmake)
  buildUnitTest()
endif()

# Document configuration
find_package(Doxygen)
if(DOXYGEN_FOUND)
  include(${PROJECT_SOURCE_DIR}/cmake/document.cmake)
  set(html_theme_path ${PROJECT_SOURCE_DIR}/document/doxygen_dark_theme)
  if(EXISTS ${html_theme_path})
    list(APPEND html_theme_params HTML_HEADER ${html_theme_path}/html_header.html)
    list(APPEND html_theme_params HTML_FOOTER ${html_theme_path}/html_footer.html)
    list(APPEND html_theme_params HTML_EXTRA_STYLESHEET
                                  ${html_theme_path}/custom_dark_theme.css)
  endif()
  addDoxygenDoc(documents ${PROJECT_BINARY_DIR}/documents
    SOURCE_FILEDIRS ${PROJECT_SOURCE_DIR}/README.md
                    ${PROJECT_SOURCE_DIR}/LICENSE.txt
                    ${PROJECT_SOURCE_DIR}/source/zisc/document.dox
                    ${zisc_include_dirs}
    EXAMPLE_FILEDIRS ${PROJECT_SOURCE_DIR}/example
    ${html_theme_params})
endif()
