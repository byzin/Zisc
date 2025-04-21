# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.30)


cmake_path(SET __zisc_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../source/zisc")


#
function(setTestCompileFlags target scope)
  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")

  Zisc_checkTarget(${target})
endfunction(setTestCompileFlags)


#
function(setTestWarningFlags target scope)
  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")

  Zisc_checkTarget(${target})

  # Set properties
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)

  # Set MSVC warnings
  set(msvc_flags #/wd4244 # 'argument': conversion from '_Ty' to 'const int', possible loss of data
                 #/wd4267 # conversion from 'size_t' to '_To', possible loss of data.
                 #/wd4324 # structure was padded due to alignment specifier¬
                 #/wd4334 # '<<': result of 32-bit shift implicitly converted to 64 bits
                 #/wd6031 # Return value ignored
                 #/wd6297 # Arithmetic overflow.  Results might not be an expected value.
                 #/wd6326 # Potential comparison of a constant with another constant.
                 )

  # Set GCC warnings
  set(gcc_flags)

  # Set Clang warnings
  set(clang_flags)

  # Actually set the warnings
  target_compile_options(${target} ${scope}
    $<${has_msvc}:${msvc_flags}>
    $<${has_gcc}:${msvc_gcc}>
    $<${has_clang}:${msvc_clang}>
  )
endfunction(setTestWarningFlags)


#function(getMakeTestRunScriptCode target bin_dir lib_dir script_name output_dir code)
#  # Set environment variables
#  set(env_vars "")
#  cmake_path(NATIVE_PATH lib_dir native_lib_dir)
#  if(Z_SYSTEM_LINUX)
#    list(APPEND env_vars "LD_LIBRARY_PATH=\\\"\\\${Z_SCRIPT_DIR}/${native_lib_dir}\\\${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}\\\"")
#  elseif(Z_SYSTEM_DARWIN)
#    list(APPEND env_vars "DYLD_LIBRARY_PATH=\\\"\\\${Z_SCRIPT_DIR}/${native_lib_dir}:\\\${DYLD_LIBRARY_PATH:-}\\\"")
#  elseif(Z_SYSTEM_WINDOWS)
#  endif()
#
#  #
#  cmake_path(SET source_dir NORMALIZE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../source")
#  include("${source_dir}/zisc/cmake/packaging.cmake")
#  Zisc_getMakeSimpleRunScriptCode(${target} "${bin_dir}" "${lib_dir}" "${env_vars}" "${script_name}" "${output_dir}" install_code)
#
#
#  # Output variables
#  set(${code} ${install_code} PARENT_SCOPE)
#endfunction(getMakeTestRunScriptCode)


#
macro(setTestProject)
  set(project_description "Zisc unit test.")
  project(UnitTest VERSION 0.0.1 DESCRIPTION ${project_description} LANGUAGES CXX)


  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")
  include("${__zisc_dir__}/cmake/compiler.cmake")
  include("${__zisc_dir__}/cmake/packaging.cmake")

  # Check dependencies
  Zisc_checkTarget(Zisc::Zisc)

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

  # Check source files
  file(GLOB unittest_source_files ${PROJECT_SOURCE_DIR}/unittest/*[.hpp|.cpp])
  source_group(TREE "${PROJECT_SOURCE_DIR}/unittest" PREFIX ${PROJECT_NAME} FILES ${unittest_source_files})


  # Create the binary
  set(target unittest)
  add_executable(${target} ${unittest_source_files})
  target_link_libraries(${target} PRIVATE Zisc::Zisc
                                          GTest::gtest)
  Zisc_setSanitizerFlags(${target} PRIVATE)
  Zisc_setCxxWarningFlags(${target} PRIVATE)
  setTestCompileFlags(${target} PRIVATE)
  setTestWarningFlags(${target} PRIVATE)
  target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/math_reference")
  Zisc_enableIpo(${target})
  Zisc_createLinkToTarget(${target} "${PROJECT_BINARY_DIR}")

  # Set static analyzer
  set(clang_tidy_exclusion_checks
    #                                  bugprone-easily-swappable-parameters
    #                                  bugprone-implicit-widening-of-multiplication-result
    #                                  bugprone-narrowing-conversions
    #                                  cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays
    #                                  cppcoreguidelines-avoid-magic-numbers
    #                                  cppcoreguidelines-avoid-non-const-global-variables
    #                                  cppcoreguidelines-macro-usage
    #                                  cppcoreguidelines-narrowing-conversions
    #                                  cppcoreguidelines-owning-memory
    #                                  cppcoreguidelines-rvalue-reference-param-not-moved
    #                                  cppcoreguidelines-pro-bounds-array-to-pointer-decay
    #                                  cppcoreguidelines-pro-bounds-constant-array-index
    #                                  cppcoreguidelines-special-member-functions
    #                                  google-readability-braces-around-statements
    #                                  google-runtime-int
    #                                  hicpp-avoid-c-arrays
    #                                  hicpp-braces-around-statements
    #                                  hicpp-no-array-decay
    #                                  hicpp-signed-bitwise
    #                                  hicpp-special-member-functions
    #                                  hicpp-uppercase-literal-suffix
    #                                  misc-const-correctness
    #                                  misc-include-cleaner
    #                                  misc-non-private-member-variables-in-classes
    #                                  misc-redundant-expression
    #                                  modernize-avoid-c-arrays
    #                                  modernize-use-trailing-return-type
    #                                  performance-avoid-endl
    #                                  readability-avoid-const-params-in-decls
    #                                  readability-braces-around-statements
    #                                  readability-function-cognitive-complexity
    #                                  readability-identifier-length
    #                                  readability-isolate-declaration
    #                                  readability-magic-numbers
    #                                  readability-uppercase-literal-suffix
                                  )
  Zisc_setStaticAnalyzer(${target}
                         CLANG_TIDY_HEADER_PATHS ${PROJECT_SOURCE_DIR}/unittest/
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})

  # Installation settings
  Zisc_saveDependencyListWhenInstallation(${target} "${PROJECT_BINARY_DIR}" COMPONENT development)
  install(TARGETS ${target}
          RUNTIME_DEPENDENCY_SET unittest-deps
          COMPONENT development)
  Zisc_installDependencySet(unittest-deps
                            DESTINATION "lib"
                            COMPONENT development)
  Zisc_installRunningTargetScript(${target}
                                  ${CMAKE_INSTALL_BINDIR}
                                  run-unittest
                                  COMPONENT development)
  install(DIRECTORY "${dst_resource_dir}"
          DESTINATION "test"
          COMPONENT development)

  # Add tests for CMake
  add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
                                WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
  set(test_env_list "")
  list(APPEND test_env_list GTEST_COLOR=1)
  set_tests_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "${test_env_list}")

  Zisc_printTargetCompileProperties(${target} "  + ")
endmacro(setTestProject)


##
setTestProject()
