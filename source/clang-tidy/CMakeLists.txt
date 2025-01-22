# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.30)


# Constant values
cmake_path(SET __zisc_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../zisc")


macro(Zisc_setZiscClangTidyProject)
  set(project_description "The clang-tidy project for Zisc header analyzation.")
  project(ZiscClangTidy VERSION ${ZiscProject_VERSION} DESCRIPTION "${project_description}" LANGUAGES CXX)

  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")
  include("${__zisc_dir__}/cmake/compiler.cmake")

  # Check dependencies
  Zisc_checkTarget(Zisc::Zisc)

  # Create a source code that is analyzed
  cmake_path(SET zisc_analyzer_dir "${PROJECT_BINARY_DIR}/cpp")
  file(MAKE_DIRECTORY "${zisc_analyzer_dir}")
  cmake_path(SET zisc_source_file ${zisc_analyzer_dir}/clang-tidy.cpp)
  configure_file("${PROJECT_SOURCE_DIR}/clang-tidy.cpp.in"
                 "${zisc_source_file}"
                 @ONLY)
  source_group(${PROJECT_NAME} FILES "${zisc_source_file}")

  # Set compilation properties for the analyzation
  set(target zisc-clang-tidy)
  add_executable(${target} ${zisc_source_file})
  target_link_libraries(${target} PRIVATE Zisc::Zisc)
  Zisc_populateTargetCompilationProperties(Zisc::Zisc ${target})
  Zisc_setCxxWarningFlags(${target} PRIVATE)

  # Set static analyzer
  set(clang_tidy_exclusion_checks
                                 )
  Zisc_setStaticAnalyzer(${target}
                         CLANG_TIDY_HEADER_PATHS ${__zisc_dir__}/core/zisc
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})
endmacro(Zisc_setZiscClangTidyProject)


##
Zisc_setZiscClangTidyProject()
