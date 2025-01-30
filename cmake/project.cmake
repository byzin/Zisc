# file: project.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


include_guard()


# Get the representative feature level used in the projects
function(getRepresentativeFeatureLevel output)
  # Include dependencies
  cmake_path(SET zisc_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source/zisc")
  include("${zisc_path}/cmake/general.cmake")
  include("${zisc_path}/cmake/platform.cmake")

  # Used options
  set(has_hardware_features ${Z_ENABLE_HARDWARE_FEATURES})

  set(representative_feature_level)
  if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
    set(representative_feature_level "Amd64V1")
    if(has_hardware_features)
      set(representative_feature_level "Amd64V3")
    endif()
  else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()

  # Check if the result is valid
  Zisc_checkIfFeatureLevelIsSupported(${representative_feature_level})

  # Actually set to the output
  set(${output} ${representative_feature_level} PARENT_SCOPE)
endfunction(getRepresentativeFeatureLevel)


# Define an alias of the representative zisc library
function(setZiscAlias)
  if(TARGET Zisc::Zisc)
    return()
  endif()

  # Include dependencies
  cmake_path(SET zisc_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source/zisc")
  include("${zisc_path}/cmake/general.cmake")

  # Find the representative zisc library
  getRepresentativeFeatureLevel(feature_level)
  set(build_prop_alias Zisc::ZiscBuildProperties${feature_level})
  Zisc_checkTarget(${build_prop_alias})
  get_target_property(build_prop_target ${build_prop_alias} ALIASED_TARGET)
  set(zisc_alias Zisc::Zisc${feature_level})
  Zisc_checkTarget(${zisc_alias})
  get_target_property(zisc_target ${zisc_alias} ALIASED_TARGET)

  # Actually set the found zisc library as representative zisc
  add_library(Zisc::ZiscBuildProperties ALIAS ${build_prop_target})
  add_library(Zisc::Zisc ALIAS ${zisc_target})
endfunction(setZiscAlias)


# GoogleTest
function(addGoogleTest binary_dir)
  if(TARGET GTest::gtest)
    return()
  else()
    message(STATUS "Add GoogleTest subdirectory.")
  endif()

  # Include dependencies
  cmake_path(SET zisc_path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source/zisc")
  include("${zisc_path}/cmake/general.cmake")
  include("${zisc_path}/cmake/compiler.cmake")

  # Check dependencies
  Zisc_checkTarget(Zisc::ZiscBuildProperties)

  # Add googletest
  cmake_path(SET dependencies_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../test/dependencies")
  cmake_path(SET googletest_path "${dependencies_dir}/googletest")
  Zisc_addGoogleTest(${googletest_path} ${binary_dir})
  Zisc_checkTarget(GTest::gtest)
  # Set the compiler flags required for Zisc to GoogleTest
  get_target_property(gtest_target GTest::gtest ALIASED_TARGET)
  target_link_libraries(${gtest_target} PRIVATE Zisc::ZiscBuildProperties)
  Zisc_populateTargetCompilationProperties(Zisc::ZiscBuildProperties ${gtest_target})
endfunction(addGoogleTest)
