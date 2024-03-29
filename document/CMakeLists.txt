# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.22)


cmake_path(SET __zisc_root_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/..")
cmake_path(SET __zisc_dir__ "${__zisc_root_dir__}/source/zisc")


macro(setDocumentProject)
  set(project_description "Zisc documents")
  project(Document VERSION ${ZiscProject_VERSION})
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    include("${__zisc_dir__}/cmake/document.cmake")

    # Doxygen custom properties
    # Project
    set(DOXYGEN_PROJECT_NAME Zisc)
    set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})
    set(DOXYGEN_PROJECT_BRIEF "Zisc provides commonly used C++ classes and functions.")

    # HTML
    cmake_path(SET html_theme_path "${PROJECT_SOURCE_DIR}/doxygen_dark_theme")
    if(EXISTS "${html_theme_path}")
      set(DOXYGEN_HTML_HEADER "${html_theme_path}/html_header.html")
      set(DOXYGEN_HTML_FOOTER "${html_theme_path}/html_footer.html")
      set(DOXYGEN_HTML_EXTRA_STYLESHEET "${html_theme_path}/custom_dark_theme.css")
    endif()

    # Example
    set(DOXYGEN_EXAMPLE_PATH "${__zisc_root_dir__}/example")
    set(DOXYGEN_EXAMPLE_RECURSIVE YES)

    # Compile flags
    get_target_property(zisc_definitions Zisc::Zisc COMPILE_DEFINITIONS)
    if(zisc_definitions)
      set(DOXYGEN_PREDEFINED ${zisc_definitions})
    endif()

    get_target_property(zisc_source_files Zisc::Zisc SOURCES)
    Zisc_addDoxygenDoc(document ${PROJECT_BINARY_DIR}
      SOURCE_FILES_OR_DIRS "${__zisc_root_dir__}/README.md"
                           "${__zisc_root_dir__}/LICENSE.md"
                           "${__zisc_dir__}/document.dox"
                           ${zisc_source_files})
  endif()
endmacro(setDocumentProject)


##
setDocumentProject()
