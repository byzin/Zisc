# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.22)


cmake_path(SET __zisc_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../source/zisc")


function(getTestCompileFlags test_compile_flags test_linker_flags test_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

  # Output variable
  set(${test_compile_flags} ${compile_flags} PARENT_SCOPE)
  set(${test_linker_flags} ${linker_flags} PARENT_SCOPE)
  set(${test_definitions} ${definitions} PARENT_SCOPE)
endfunction(getTestCompileFlags)


function(addGoogleTest googletest_path)
  cmake_path(SET source_dir NORMALIZE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source")
  include("${source_dir}/zisc/cmake/general.cmake")
  include("${source_dir}/zisc/cmake/compiler.cmake")

  Zisc_addGoogleTest("${googletest_path}" "${PROJECT_BINARY_DIR}/GoogleTest")
  Zisc_populateTargetOptions(Zisc gtest)
  Zisc_populateTargetOptions(Zisc gtest_main)
endfunction(addGoogleTest)


function(getMakeTestRunScriptCode target bin_dir lib_dir script_name output_dir code)
  # Set environment variables
  set(env_vars "")
  cmake_path(NATIVE_PATH lib_dir native_lib_dir)
  if(Z_LINUX)
    list(APPEND env_vars "LD_LIBRARY_PATH=\\\"${native_lib_dir}\\\${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}\\\"")
  elseif(Z_MAC)
    list(APPEND env_vars "DYLD_LIBRARY_PATH=\\\"${native_lib_dir}:\\\${DYLD_LIBRARY_PATH:-}\\\"")
  elseif(Z_WINDOWS)
  endif()

  #
  cmake_path(SET source_dir NORMALIZE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source")
  include("${source_dir}/zisc/cmake/packaging.cmake")
  Zisc_getMakeSimpleRunScriptCode(${target} "${bin_dir}" "${lib_dir}" "${env_vars}" "${script_name}" "${output_dir}" install_code)


  # Output variables
  set(${code} ${install_code} PARENT_SCOPE)
endfunction(getMakeTestRunScriptCode)


function(getTestWarningFlags test_warning_flags)
  set(warning_flags "")

  # Suppress warnings
  if(ZISC_SUPPRESS_EXCESSIVE_WARNING)
    if(Z_CLANG)
      # list(APPEND warning_flags
      #                          )
      if(Z_VISUAL_STUDIO)
        # list(APPEND warning_flags
        #                          )
      endif()
    elseif(Z_GCC)
      # list(APPEND warning_flags
      #                          )
    elseif(Z_MSVC)
      list(APPEND warning_flags /wd6326 # Potential comparison of a constant with another constant.
                                )
    endif()
  endif()

  # Output variable
  set(${test_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getTestWarningFlags)


#
macro(setTestProject)
  set(project_description "Zisc unit test.")
  project(UnitTest VERSION 0.0.1 DESCRIPTION ${project_description} LANGUAGES CXX)


  # Initialize platform info
  include("${__zisc_dir__}/cmake/general.cmake")
  include("${__zisc_dir__}/cmake/platform.cmake")
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})

  # Check dependencies
  Zisc_checkTarget(Zisc)
  addGoogleTest("${PROJECT_SOURCE_DIR}/dependencies/googletest")
  Zisc_checkTarget(gtest_main)

  # Create a symlink to the test resources
  file(COPY "${PROJECT_SOURCE_DIR}/resources" DESTINATION "${PROJECT_BINARY_DIR}")
  file(GLOB math_reference_files "${PROJECT_SOURCE_DIR}/math_reference/*.tar.xz")
  foreach(reference_path IN LISTS math_reference_files)
    cmake_path(SET src_file_path NORMALIZE "${reference_path}")
    get_filename_component(reference_file "${reference_path}" NAME_WLE)
    get_filename_component(reference_file "${reference_file}" NAME_WLE)
    cmake_path(SET dst_file_path "${PROJECT_BINARY_DIR}/resources/${reference_file}")
    if("${src_file_path}" IS_NEWER_THAN "${dst_file_path}")
      execute_process(COMMAND ${CMAKE_COMMAND} -E tar Jxf "${src_file_path}"
                      WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/resources")
      file(TOUCH_NOCREATE "${dst_file_path}")
    endif()
  endforeach(reference_path)

  # Create a unit test
  file(GLOB unittest_source_files ${PROJECT_SOURCE_DIR}/unittest/*[.hpp|.cpp])
  add_executable(${PROJECT_NAME} ${unittest_source_files})
  source_group(${PROJECT_NAME} FILES ${unittest_source_files})

  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 
                                                   CXX_STANDARD_REQUIRED ON)

  include("${__zisc_dir__}/cmake/compiler.cmake")
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
  getTestWarningFlags(test_warning_flags)
  target_compile_options(${PROJECT_NAME} PRIVATE ${test_compile_flags}
                                                 ${cxx_compile_warning_flags}
                                                 ${test_warning_flags})
  target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main
                                                Zisc
                                                ${test_linker_flags})
  target_compile_definitions(${PROJECT_NAME} PRIVATE ${test_definitions})
  target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/math_reference")
  Zisc_enableIpo(${PROJECT_NAME})

  #
  set(clang_tidy_exclusion_checks bugprone-implicit-widening-of-multiplication-result
                                  bugprone-narrowing-conversions
                                  misc-non-private-member-variables-in-classes
                                  misc-redundant-expression
                                  modernize-avoid-c-arrays
                                  modernize-use-auto
                                  performance-move-const-arg
                                  readability-function-cognitive-complexity
                                  readability-isolate-declaration
                                  readability-magic-numbers
                                  readability-named-parameter
                                  readability-static-accessed-through-instance
                                  readability-uppercase-literal-suffix)
  Zisc_setStaticAnalyzer(${PROJECT_NAME}
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})
  Zisc_createLinkToTarget(${PROJECT_NAME} ${PROJECT_BINARY_DIR})

  # Installation settings
  include("${__zisc_dir__}/cmake/packaging.cmake")
  ## Save the dependency list
  Zisc_getSaveDependencyCode(${PROJECT_NAME} "${PROJECT_BINARY_DIR}" "" "" "" dependency_code)
  install(CODE ${dependency_code} COMPONENT test)
  ## Install actual binary
  install(TARGETS ${PROJECT_NAME}
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT test)
  Zisc_getDeployingNonSystemDependencyCode(${PROJECT_NAME} "${CMAKE_INSTALL_LIBDIR}" "" "" "" deploy_code)
  install(CODE ${deploy_code}
          COMPONENT test)
        getMakeTestRunScriptCode(${PROJECT_NAME} "../${CMAKE_INSTALL_BINDIR}" "../${CMAKE_INSTALL_LIBDIR}" "runUnitTest" "script" run_script_code)
  install(CODE ${run_script_code}
          COMPONENT test)

  # Add tests for CMake
  add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
                                WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
  set(test_env_list "")
  list(APPEND test_env_list GTEST_COLOR=1)
  set_tests_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "${test_env_list}")
endmacro(setTestProject)


##
setTestProject()
