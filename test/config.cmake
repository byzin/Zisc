# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2016 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

set(__test_root__ ${CMAKE_CURRENT_LIST_DIR})


function(getTestWarningOption test_warning_flags)
  set(warning_flags "")
  if(Z_IS_CLANG)
    list(APPEND warning_flags -Wno-sign-conversion
                              -Wno-float-equal
                              -Wno-used-but-marked-unused
                              -Wno-covered-switch-default
                              )
  elseif(Z_IS_GCC)
    list(APPEND warning_flags -Wno-sign-conversion
                              -Wno-strict-overflow
                               )
  endif()
  set(${test_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getTestWarningOption)


function(buildUnitTest)
  # Load google test
  include(${__test_root__}/gtest/config.cmake)
  loadGoogleTest(SHARED 14 gtest_include_dir gtest_library)

  ## Build a unit test
  if(NOT EXISTS ${PROJECT_BINARY_DIR}/resources)
    execute_process(COMMAND ln -sf ${__test_root__}/resources
                                   ${PROJECT_BINARY_DIR}/resources)
  endif()
  #
  file(GLOB unit_test_source_files  ${__test_root__}/unit_test/*.cpp)
  add_executable(UnitTest ${unit_test_source_files} ${zisc_header_files})
  set_target_properties(UnitTest PROPERTIES CXX_STANDARD 14
                                            CXX_STANDARD_REQUIRED ON)
  getCxxWarningOption(cxx_warning_flags)
  getTestWarningOption(test_warning_flags)
  checkCompilerHasCxx14Features(UnitTest)
  target_compile_options(UnitTest PRIVATE ${cxx_compile_flags}
                                          ${zisc_compile_flags}
                                          ${cxx_warning_flags}
                                          ${test_warning_flags})
  target_include_directories(UnitTest PRIVATE ${zisc_include_dirs})
  target_include_directories(UnitTest SYSTEM PRIVATE ${gtest_include_dir})
  find_package(Threads REQUIRED)
  target_link_libraries(UnitTest PRIVATE ${CMAKE_THREAD_LIBS_INIT}
                                         ${cxx_linker_flags}
                                         ${zisc_linker_flags}
                                         ${gtest_library})
  target_compile_definitions(UnitTest PRIVATE ${environment_definitions}
                                              ${cxx_definitions}
                                              ${zisc_definitions})
endfunction(buildUnitTest)
