# file: config.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2018 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

set(__example_root__ ${CMAKE_CURRENT_LIST_DIR})


function(getExampleWarningOption example_warning_flags)
  set(warning_flags "")
  if(Z_CLANG)
    list(APPEND warning_flags -Wno-sign-conversion 
                              -Wno-float-equal
                              )
  elseif(Z_GCC)
    list(APPEND warning_flags -Wno-sign-conversion 
                              -Wno-strict-overflow
                              )
  endif()
  set(${example_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getExampleWarningOption)


function(setExample example_name)
  # Build a example
  add_executable(${example_name} ${ARGN})
  #
  set_target_properties(${example_name} PROPERTIES CXX_STANDARD 17
                                                   CXX_STANDARD_REQUIRED ON
                                                   RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Examples)
  getCxxWarningOption(cxx_warning_flags)
  getExampleWarningOption(example_warning_flags)
  target_compile_options(${example_name} PRIVATE ${cxx_compile_flags} 
                                                 ${zisc_compile_flags}
                                                 ${cxx_warning_flags}
                                                 ${example_warning_flags})
  target_include_directories(${example_name} PRIVATE ${zisc_include_dirs})
  find_package(Threads REQUIRED)
  target_link_libraries(${example_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT}
                                                ${cxx_linker_flags}
                                                ${zisc_linker_flags})
  target_compile_definitions(${example_name} PRIVATE ${environment_definitions}
                                                     ${cxx_definitions}
                                                     ${zisc_definitions})
endfunction(setExample)


function(buildExample)
  # Build examples
  setExample(AlgorithmExample 
             ${__example_root__}/algorithm_example.cpp)
  setExample(CumulativeDistributionFunctionExample
             ${__example_root__}/cumulative_distribution_function_example.cpp)
  setExample(CompensatedSummationExample
             ${__example_root__}/compensated_summation_example.cpp)
  setExample(CsvExample
             ${__example_root__}/csv_example.cpp)
  setExample(PseudoRandomNumberEngineExample
             ${__example_root__}/pseudo_random_number_engine_example.cpp)
  setExample(StopwatchExample
             ${__example_root__}/stopwatch_example.cpp)
  setExample(LinearAlgebraExample
             ${__example_root__}/linear_algebra_example.cpp) 
  setExample(UnitExample
             ${__example_root__}/unit_example.cpp)
  setExample(ThreadPoolExample
             ${__example_root__}/thread_pool_example.cpp)
  setExample(StringExample
             ${__example_root__}/string_example.cpp)
  setExample(CmjEngineExample
             ${__example_root__}/correlated_multi_jittered_engine_example.cpp)
  setExample(ErrorExample
             ${__example_root__}/error_example.cpp)
endfunction(buildExample)
