# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.30)


# Constant values
cmake_path(SET __zisc_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../source/zisc")


#
function(setExampleWarningFlags target scope)
  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")

  Zisc_checkTarget(${target})

  # Set properties
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)

  # Set MSVC warnings
  set(msvc_flags #/wd4244 # 'argument': conversion from '_Ty' to 'const int', possible loss of data¬
                 # /wd4267 # conversion from 'size_t' to '_To', possible loss of data.
                 # /wd4702 # unreachable code
                 # /wd6326 # Potential comparison of a constant with another constant.
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
endfunction(setExampleWarningFlags)


# Add an example
function(addExample target save_dependency_list)
  # Include dependencies
  include("${__zisc_dir__}/cmake/general.cmake")
  include("${__zisc_dir__}/cmake/compiler.cmake")

  # Check dependencies
  Zisc_checkTarget(Zisc::Zisc)

  # Check source files
  set(example_source_files ${ARGN})
  source_group(${PROJECT_NAME}/${target} FILES ${example_source_files})

  # Create the binary
  add_executable(${target} ${example_source_files})
  target_link_libraries(${target} PRIVATE Zisc::Zisc)
  Zisc_setSanitizerFlags(${target} PRIVATE)
  Zisc_setCxxWarningFlags(${target} PRIVATE)
  setExampleWarningFlags(${target} PRIVATE)
  Zisc_enableIpo(${target})
  Zisc_createLinkToTarget(${target} "${PROJECT_BINARY_DIR}")

  # Set static analyzer
  set(clang_tidy_exclusion_checks 
    #                                  bugprone-exception-escape
    #                                  bugprone-implicit-widening-of-multiplication-result
    #    #bugprone-narrowing-conversions
    #                                  cppcoreguidelines-avoid-magic-numbers
    #                                  cppcoreguidelines-pro-bounds-pointer-arithmetic
    #                                  google-readability-braces-around-statements
    #                                  hicpp-braces-around-statements
    #                                  hicpp-uppercase-literal-suffix
    #    #misc-non-private-member-variables-in-classes
    #    #misc-redundant-expression
    #                                  misc-include-cleaner
    #    #modernize-avoid-c-arrays
    #    #modernize-use-auto
    #                                  modernize-use-trailing-return-type
    #                                  performance-avoid-endl
    #                                  readability-avoid-const-params-in-decls
    #                                  readability-braces-around-statements
    #                                  readability-function-cognitive-complexity
    #                                  readability-identifier-length
    #                                  readability-isolate-declaration
    #                                  readability-magic-numbers
    #                                  #readability-static-accessed-through-instance
    #                                  readability-uppercase-literal-suffix
                                  )
  Zisc_setStaticAnalyzer(${target}
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})

#  # Installation settings
#  include("${__zisc_dir__}/cmake/packaging.cmake")
#  ## Save the dependency list
#  if(save_dependency_list)
#    Zisc_getSaveDependencyCode(${target} "${PROJECT_BINARY_DIR}" "" "" "" dependency_code)
#    install(CODE ${dependency_code}
#            COMPONENT example)
#  endif()
#  install(TARGETS ${target}
#          DESTINATION "${CMAKE_INSTALL_BIDIR}"
#          COMPONENT example)
#
#  install(CODE [[
#    message("## Installl!!!")
#    message("## target path: $<TARGET_FILE:UnitExample>")
#    file(GET_RUNTIME_DEPENDENCIES
#      RESOLVED_DEPENDENCIES_VAR resolved_dependencies
#      UNRESOLVED_DEPENDENCIES_VAR unresolved_dependencies
#      EXECUTABLES $<TARGET_FILE:UnitExample>)
#    message("## resolved dependencies: ${resolved_dependencies}")
#    message("## unresolved dependencies: ${unresolved_dependencies}")
#  ]]
#  COMPONENT example)
endfunction(addExample)


#
macro(setExampleProject)
  set(project_description "Examples of Zisc.")
  project(Examples VERSION 0.0.1 DESCRIPTION "${project_description}" LANGUAGES  CXX)

  # TODO. Use lower case name for binary name: e.g. example-algorithm
  # TODO. Also rename source code using lower case name: e.g. example_algorithm.cpp
  # TODO. Can we number the examples? e.g. example_0001_algorithm.cpp
  # Build examples
  addExample(AlgorithmExample ON
             ${PROJECT_SOURCE_DIR}/algorithm_example.cpp)
  addExample(CmjEngineExample OFF
             ${PROJECT_SOURCE_DIR}/correlated_multi_jittered_engine_example.cpp)
  addExample(CompensatedSummationExample OFF
             ${PROJECT_SOURCE_DIR}/compensated_summation_example.cpp)
  addExample(CsvExample OFF
             ${PROJECT_SOURCE_DIR}/csv_example.cpp)
  addExample(ErrorExample OFF
             ${PROJECT_SOURCE_DIR}/error_example.cpp)
  addExample(ConcurrentBoundedQueueExample OFF
             ${PROJECT_SOURCE_DIR}/concurrent_bounded_queue_example.cpp)
  addExample(PseudoRandomNumberEngineExample OFF
             ${PROJECT_SOURCE_DIR}/pseudo_random_number_engine_example.cpp)
  addExample(StopwatchExample OFF
             ${PROJECT_SOURCE_DIR}/stopwatch_example.cpp)
  addExample(StringExample OFF
             ${PROJECT_SOURCE_DIR}/string_example.cpp)
  addExample(ThreadManagerExample OFF
             ${PROJECT_SOURCE_DIR}/thread_manager_example.cpp)
  addExample(UnitExample OFF
             ${PROJECT_SOURCE_DIR}/unit_example.cpp)
endmacro(setExampleProject)


##
setExampleProject()
