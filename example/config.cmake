# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

set(__example_root__ ${CMAKE_CURRENT_LIST_DIR})


function(getExampleWarningFlags example_warning_flags)
  set(warning_flags "")

  # Suppress warnings
  if(ZISC_SUPPRESS_EXCESSIVE_WARNING)
    if(Z_CLANG)
      list(APPEND warning_flags -Wno-exit-time-destructors
                                -Wno-float-equal
                                -Wno-sign-conversion 
                                -Wno-weak-vtables
                                )
    elseif(Z_GCC)
      list(APPEND warning_flags -Wno-sign-conversion 
                                -Wno-strict-overflow
                                )
    endif()
  endif()

  set(${example_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getExampleWarningFlags)


function(setExample example_name)
  # Build a example
  set(example_source_files ${ARGN})
  add_executable(${example_name} ${example_source_files})
  source_group(Examples FILES ${example_source_files})
  #
  set_target_properties(${example_name} PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Examples
      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Examples
      RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/Examples
      RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/Examples)
  getCxxWarningFlags(cxx_warning_flags)
  getExampleWarningFlags(example_warning_flags)
  target_compile_options(${example_name} PRIVATE ${cxx_compile_flags} 
                                                 ${zisc_compile_flags}
                                                 ${cxx_warning_flags}
                                                 ${example_warning_flags})
  target_include_directories(${example_name} PRIVATE ${zisc_include_dirs})
  target_link_libraries(${example_name} PRIVATE Threads::Threads
                                                ${cxx_linker_flags}
                                                ${zisc_linker_flags})
  target_compile_definitions(${example_name} PRIVATE ${platform_definitions}
                                                     ${cxx_definitions}
                                                     ${zisc_definitions})
  setStaticAnalyzer(${example_name})
endfunction(setExample)


function(buildExample)
  # Build examples
  setExample(AlgorithmExample 
             ${__example_root__}/algorithm_example.cpp)
  setExample(CmjEngineExample
             ${__example_root__}/correlated_multi_jittered_engine_example.cpp)
  setExample(CompensatedSummationExample
             ${__example_root__}/compensated_summation_example.cpp)
  setExample(CsvExample
             ${__example_root__}/csv_example.cpp)
  setExample(CumulativeDistributionFunctionExample
             ${__example_root__}/cumulative_distribution_function_example.cpp)
  setExample(ErrorExample
             ${__example_root__}/error_example.cpp)
  setExample(LinearAlgebraExample
             ${__example_root__}/linear_algebra_example.cpp) 
  setExample(LockFreeBoundedQueueExample
             ${__example_root__}/lock_free_bounded_queue_example.cpp)
  setExample(PseudoRandomNumberEngineExample
             ${__example_root__}/pseudo_random_number_engine_example.cpp)
  setExample(StopwatchExample
             ${__example_root__}/stopwatch_example.cpp)
  setExample(StringExample
             ${__example_root__}/string_example.cpp)
  setExample(ThreadManagerExample
             ${__example_root__}/thread_manager_example.cpp)
  setExample(UnitExample
             ${__example_root__}/unit_example.cpp)
endfunction(buildExample)
