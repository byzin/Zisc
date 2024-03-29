# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.22)


cmake_path(SET __zisc_dir__ NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/../source/zisc")


function(getExampleWarningFlags example_warning_flags)
  set(warning_flags "")

  # Suppress warnings
  if(ZISC_SUPPRESS_EXCESSIVE_WARNING)
    if(Z_CLANG)
      # list(APPEND warning_flags 
      #                           )
    elseif(Z_GCC)
      # list(APPEND warning_flags 
      #                           )
    elseif(Z_MSVC)
      list(APPEND warning_flags /wd4244 # 'argument': conversion from '_Ty' to 'const int', possible loss of data¬
                                /wd4267 # conversion from 'size_t' to '_To', possible loss of data.
                                /wd4702 # unreachable code
                                /wd6326 # Potential comparison of a constant with another constant.
                                )
    endif()
  endif()

  set(${example_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getExampleWarningFlags)


# Add an example
function(addExample target save_dependency_list)
  set(example_source_files ${ARGN})
  source_group(${PROJECT_NAME}/${target} FILES ${example_source_files})

  include("${__zisc_dir__}/cmake/compiler.cmake")
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  getExampleWarningFlags(example_warning_flags)

  #
  add_executable(${target} ${example_source_files})
  set_target_properties(${target} PROPERTIES CXX_STANDARD 20
                                             CXX_STANDARD_REQUIRED ON)
  target_compile_options(${target} PRIVATE ${cxx_compile_warning_flags}
                                           ${compile_sanitizer_flags}
                                           ${example_warning_flags})
  target_link_libraries(${target} PRIVATE Zisc::Zisc)
  target_link_options(${target} PRIVATE ${linker_sanitizer_flags})
  Zisc_enableIpo(${target})

  #
  set(clang_tidy_exclusion_checks 
                                  bugprone-exception-escape
                                  bugprone-implicit-widening-of-multiplication-result
    #bugprone-narrowing-conversions
                                  cppcoreguidelines-avoid-magic-numbers
                                  cppcoreguidelines-pro-bounds-pointer-arithmetic
                                  google-readability-braces-around-statements
                                  hicpp-braces-around-statements
                                  hicpp-uppercase-literal-suffix
    #misc-non-private-member-variables-in-classes
    #misc-redundant-expression
                                  misc-include-cleaner
    #modernize-avoid-c-arrays
    #modernize-use-auto
                                  modernize-use-trailing-return-type
                                  performance-avoid-endl
                                  readability-avoid-const-params-in-decls
                                  readability-braces-around-statements
                                  readability-function-cognitive-complexity
                                  readability-identifier-length
                                  readability-isolate-declaration
                                  readability-magic-numbers
                                  #readability-static-accessed-through-instance
                                  readability-uppercase-literal-suffix
                                  )
  Zisc_setStaticAnalyzer(${target}
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})
  Zisc_createLinkToTarget(${target} "${PROJECT_BINARY_DIR}")

  # Installation settings
  include("${__zisc_dir__}/cmake/packaging.cmake")
  ## Save the dependency list
  if(save_dependency_list)
    Zisc_getSaveDependencyCode(${target} "${PROJECT_BINARY_DIR}" "" "" "" dependency_code)
    install(CODE ${dependency_code}
            COMPONENT example)
  endif()
  install(TARGETS ${target}
          DESTINATION "${CMAKE_INSTALL_BIDIR}"
          COMPONENT example)
endfunction(addExample)


#
macro(setExampleProject)
  set(project_description "Examples of Zisc")
  project(Examples VERSION 0.0.1 DESCRIPTION "${project_description}" LANGUAGES  CXX)

  # Initialize platform info
  include("${__zisc_dir__}/cmake/general.cmake")
  include("${__zisc_dir__}/cmake/platform.cmake")
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})

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
