# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
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


function(getMakeTestRunScriptCode target bin_dir lib_dir script_name output_dir code)
  # Set environment variables
  set(env_vars "")
  cmake_path(NATIVE_PATH lib_dir native_lib_dir)
  if(Z_LINUX)
    list(APPEND env_vars "LD_LIBRARY_PATH=\\\"\\\${Z_SCRIPT_DIR}/${native_lib_dir}\\\${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}\\\"")
  elseif(Z_MAC)
    list(APPEND env_vars "DYLD_LIBRARY_PATH=\\\"\\\${Z_SCRIPT_DIR}/${native_lib_dir}:\\\${DYLD_LIBRARY_PATH:-}\\\"")
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
      list(APPEND warning_flags /wd4267 # conversion from 'size_t' to '_To', possible loss of data.
                                /wd6031 # Return value ignored
                                /wd6326 # Potential comparison of a constant with another constant.
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

  # Create a symlink to the test resources
  cmake_path(SET dst_resource_dir "${PROJECT_BINARY_DIR}/resources")
  file(COPY "${PROJECT_SOURCE_DIR}/resources" DESTINATION "${PROJECT_BINARY_DIR}")
  file(GLOB math_reference_files "${PROJECT_SOURCE_DIR}/math_reference/reference/*.7z")
  foreach(reference_path IN LISTS math_reference_files)
    cmake_path(SET src_file_path NORMALIZE "${reference_path}")
    cmake_path(GET src_file_path STEM LAST_ONLY reference_name)
    cmake_path(SET dst_file_path "${dst_resource_dir}/${reference_name}")
    if("${src_file_path}" IS_NEWER_THAN "${dst_file_path}")
      file(ARCHIVE_EXTRACT INPUT "${src_file_path}"
                           DESTINATION "${dst_resource_dir}")
      file(TOUCH_NOCREATE "${dst_file_path}")
    endif()
  endforeach(reference_path)

  # Create a unit test
  file(GLOB unittest_source_files ${PROJECT_SOURCE_DIR}/unittest/*[.hpp|.cpp])
  source_group(${PROJECT_NAME} FILES ${unittest_source_files})


  include("${__zisc_dir__}/cmake/compiler.cmake")
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
  getTestWarningFlags(test_warning_flags)
  add_executable(${PROJECT_NAME} ${unittest_source_files})
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 
                                                   CXX_STANDARD_REQUIRED ON)
  target_compile_options(${PROJECT_NAME} PRIVATE ${test_compile_flags}
                                                 ${cxx_compile_warning_flags}
                                                 ${compile_sanitizer_flags}
                                                 ${test_warning_flags})
  target_link_libraries(${PROJECT_NAME} PRIVATE Zisc::Zisc
                                                GTest::gtest
                                                ${test_linker_flags})
  target_link_options(${PROJECT_NAME} PRIVATE ${linker_sanitizer_flags})
  target_compile_definitions(${PROJECT_NAME} PRIVATE ${test_definitions})
  target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/math_reference")
  Zisc_enableIpo(${PROJECT_NAME})

  #
  set(clang_tidy_exclusion_checks
                                   # bugprone-implicit-widening-of-multiplication-result
  #                                bugprone-narrowing-conversions
  #                                bugprone-use-after-move
                                  misc-include-cleaner
  #                                misc-non-private-member-variables-in-classes
  #                                misc-redundant-expression
  #                                modernize-avoid-c-arrays
  #                                modernize-use-auto
  #                                performance-move-const-arg
                                  readability-function-cognitive-complexity
  #                                readability-function-size
                                  readability-identifier-length
  #                                readability-isolate-declaration
  #                                readability-magic-numbers
  #                                readability-named-parameter
  #                                readability-static-accessed-through-instance
  #                                readability-uppercase-literal-suffix
                                  )

  Zisc_setStaticAnalyzer(${PROJECT_NAME}
                         CLANG_TIDY_HEADER_PATHS ${PROJECT_SOURCE_DIR}/unittest/
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
  ## Dependencies
  Zisc_getDeployingNonSystemDependencyCode(${PROJECT_NAME} "${CMAKE_INSTALL_LIBDIR}" "" "" "" deploy_code)
  install(CODE ${deploy_code}
          COMPONENT test)
  ## Runner script
  getMakeTestRunScriptCode(${PROJECT_NAME} "../${CMAKE_INSTALL_BINDIR}" "../${CMAKE_INSTALL_LIBDIR}" "runUnitTest" "${PROJECT_NAME}" run_script_code)
  install(CODE ${run_script_code}
          COMPONENT test)
  ## Resources
  install(DIRECTORY "${dst_resource_dir}"
          DESTINATION "${PROJECT_NAME}"
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
