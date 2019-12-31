# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

set(__test_root__ ${CMAKE_CURRENT_LIST_DIR})

function(getTestCompileFlags test_compile_flags test_linker_flags test_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  if(Z_MSVC)
    list(APPEND compile_flags /bigobj)
  endif()

  # Output variable
  set(${test_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${test_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${test_definitions} ${definitions} PARENT_SCOPE)
endfunction(getTestCompileFlags)

function(getTestWarningFlags test_warning_flags)
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
endfunction(getTestWarningFlags)


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
  set_target_properties(UnitTest PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}
      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}
      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})
  getCxxWarningFlags(cxx_warning_flags)
  getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
  getTestWarningFlags(test_warning_flags)
  target_compile_options(UnitTest PRIVATE ${cxx_compile_flags}
                                          ${zisc_compile_flags}
                                          ${test_compile_flags}
                                          ${cxx_warning_flags}
                                          ${test_warning_flags})
  target_include_directories(UnitTest PRIVATE ${zisc_include_dirs})
  target_include_directories(UnitTest SYSTEM PRIVATE ${gtest_include_dir})
  target_link_libraries(UnitTest PRIVATE Threads::Threads
                                         ${cxx_linker_flags}
                                         ${gtest_libraries}
                                         ${zisc_linker_flags}
                                         ${test_linker_flags})
  target_compile_definitions(UnitTest PRIVATE ${platform_definitions}
                                              ${cxx_definitions}
                                              ${zisc_definitions}
                                              ${test_definitions})
  setStaticAnalyzer(UnitTest)
endfunction(buildUnitTest)
