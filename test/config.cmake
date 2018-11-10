# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2018 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

set(__test_root__ ${CMAKE_CURRENT_LIST_DIR})

function(getTestCompileOption test_compile_flags)
  set(compile_flags "")
  # Output variable
  set(${test_compile_flags} ${compile_flags} PARENT_SCOPE)
endfunction(getTestCompileOption)

function(getTestWarningOption test_warning_flags)
  set(warning_flags "")

  # Suppress warnings
  if(ZISC_SUPPRESS_EXCESSIVE_WARNING)
    if(Z_CLANG)
      list(APPEND warning_flags -Wno-covered-switch-default
                                -Wno-exit-time-destructors
                                -Wno-float-equal
                                -Wno-global-constructors
                                -Wno-sign-conversion
                                -Wno-used-but-marked-unused
                                -Wno-weak-vtables
                                -Wno-zero-as-null-pointer-constant
                                )
    elseif(Z_GCC)
      list(APPEND warning_flags -Wno-sign-conversion
                                -Wno-strict-overflow
                                -Wno-unused-but-set-variable
                                 )
    endif()
  endif()

  # Output variable
  set(${test_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getTestWarningOption)


function(buildUnitTest)
  # Load google test
  include(${PROJECT_SOURCE_DIR}/cmake/googletest.cmake)
  set(gtest_project_root ${__test_root__}/googletest)
  checkSubmodule(${gtest_project_root})
  buildGoogleTest(${gtest_project_root} gtest_include_dir gtest_libraries)

  ## Build a unit test
  file(COPY ${__test_root__}/resources DESTINATION ${PROJECT_BINARY_DIR})
  file(GLOB unittest_source_files  ${__test_root__}/unittest/*.cpp
                                   ${__test_root__}/unittest/*.hpp)
  add_executable(UnitTest ${unittest_source_files} ${zisc_header_files})
  source_group(UnitTest FILES ${unittest_source_files})
  set_target_properties(UnitTest PROPERTIES CXX_STANDARD 17
                                            CXX_STANDARD_REQUIRED ON)
  getCxxWarningOption(cxx_warning_flags)
  getTestCompileOption(test_compile_flags)
  getTestWarningOption(test_warning_flags)
  target_compile_options(UnitTest PRIVATE ${cxx_compile_flags}
                                          ${zisc_compile_flags}
                                          ${cxx_warning_flags}
                                          ${test_compile_flags}
                                          ${test_warning_flags})
  target_include_directories(UnitTest PRIVATE ${zisc_include_dirs})
  target_include_directories(UnitTest SYSTEM PRIVATE ${gtest_include_dir})
  find_package(Threads REQUIRED)
  target_link_libraries(UnitTest PRIVATE Threads::Threads
                                         ${cxx_linker_flags}
                                         ${zisc_linker_flags}
                                         ${gtest_libraries})
  target_compile_definitions(UnitTest PRIVATE ${environment_definitions}
                                              ${cxx_definitions}
                                              ${zisc_definitions})
  setStaticAnalyzer(UnitTest)
endfunction(buildUnitTest)
