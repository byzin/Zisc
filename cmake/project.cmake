# file: project.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


# Zisc
function(setZiscAlias)
  if(TARGET Zisc::Zisc)
    return()
  endif()

  cmake_path(SET zisc_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source/zisc")
  include("${zisc_path}/cmake/general.cmake")
  include("${zisc_path}/cmake/platform.cmake")
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})

  include("${zisc_path}/cmake/compiler.cmake")
  Zisc_getArchitectureName(TRUE architecture)
  Zisc_getArchitectureTargetName("Zisc" ${architecture} zisc_name)
  Zisc_checkTarget(${zisc_name})
  add_library(Zisc::Zisc ALIAS ${zisc_name})
endfunction(setZiscAlias)


# GoogleTest
function(addGoogleTest binary_dir)
  if(TARGET GTest::gtest)
    return()
  else()
    message(STATUS "Add GoogleTest subdirectory.")
  endif()


  # Add googletest
  cmake_path(SET dependencies_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../test/dependencies")
  cmake_path(SET googletest_path "${dependencies_dir}/googletest")
  cmake_path(SET zisc_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source/zisc")
  include("${zisc_path}/cmake/general.cmake")
  Zisc_checkSubmodule(${googletest_path})
  Zisc_addGoogleTest(${googletest_path} ${binary_dir})
  Zisc_checkTarget(gtest)
  #
  include("${zisc_path}/cmake/compiler.cmake")
  Zisc_populateTargetOptions(Zisc::Zisc gtest)
  Zisc_checkTarget(GTest::gtest)
endfunction(addGoogleTest)
