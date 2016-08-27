# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2016 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

set(__gtest_path__ ${CMAKE_CURRENT_LIST_DIR})


## Load GoogleTest library
# Defined variables
#   gtest_include_dir
#   gtest_library
function(LoadGoogleTest library_type cxx_version gtest_include_dir gtest_library)
  # Version
  set(version_major 1)
  set(version_minor 7)
  set(version_patch 0)
  set(version "${version_major}.${version_minor}.${version_patch}")
  message(STATUS "## GoogleTest version: ${version}")
  
  # Set source code
  set(gtest_files ${__gtest_path__}/gtest-all.cc ${__gtest_path__}/gtest/gtest.h)
  source_group(GoogleTest FILES ${gtest_files})

  # Make library
  add_library(GoogleTest ${library_type} ${gtest_files})
  # Set properties
  set_target_properties(GoogleTest PROPERTIES CXX_STANDARD ${cxx_version}
                                              CXX_STANDARD_REQUIRED ON)
  target_compile_options(GoogleTest PRIVATE ${cxx_compile_flags})

  target_link_libraries(GoogleTest ${gtest_dependencies} ${cxx_linker_flags})
  target_compile_definitions(GoogleTest PRIVATE ${environment_definitions}
                                                ${cxx_definitions})
  # Set variable3s
  set(gtest_version_major ${version_major} PARENT_SCOPE)
  set(gtest_version_minor ${version_minor} PARENT_SCOPE)
  set(gtest_version_patch ${version_patch} PARENT_SCOPE)
  set(gtest_version ${version} PARENT_SCOPE)
  set(${gtest_include_dir} ${__gtest_path__} PARENT_SCOPE)
  set(${gtest_library} GoogleTest PARENT_SCOPE)
endfunction(LoadGoogleTest)
