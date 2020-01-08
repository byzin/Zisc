# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.16)

set(__zisc_dir__ ${CMAKE_SOURCE_DIR}/source/zisc)


set(project_description "Zisc documents")
project(Document VERSION 0.0.1)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  # Check dependencies
  if(NOT TARGET Zisc)
    message(FATAL_ERROR "Zisc not found.")
  endif()

  include(${__zisc_dir__}/cmake/document.cmake)

  # Doxygen custom properties
  # Project
  set(DOXYGEN_PROJECT_NAME Zisc)
  set(DOXYGEN_PROJECT_NUMBER 0.0.50)
  set(DOXYGEN_PROJECT_BRIEF "Zisc provides commonly used C++ classes and functions.")

  # HTML
  set(html_theme_path ${PROJECT_SOURCE_DIR}/doxygen_dark_theme)
  if(EXISTS ${html_theme_path})
    set(DOXYGEN_HTML_HEADER ${html_theme_path}/html_header.html)
    set(DOXYGEN_HTML_FOOTER ${html_theme_path}/html_footer.html)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${html_theme_path}/custom_dark_theme.css)
  endif()

  # Example
  set(DOXYGEN_EXAMPLE_PATH ${CMAKE_SOURCE_DIR}/example)
  set(DOXYGEN_EXAMPLE_RECURSIVE YES)

  # Compile flags
  get_target_property(zisc_definitions Zisc COMPILE_DEFINITIONS)
  if(zisc_definitions)
    set(DOXYGEN_PREDEFINED ${zisc_definitions})
  endif()

  get_target_property(zisc_source_files Zisc SOURCES)
  addDoxygenDoc(document ${PROJECT_BINARY_DIR}
    SOURCE_FILES_OR_DIRS ${CMAKE_SOURCE_DIR}/README.md
                         ${CMAKE_SOURCE_DIR}/LICENSE.txt
                         ${__zisc_dir__}/document.dox
                         ${zisc_source_files})
endif()
