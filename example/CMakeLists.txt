# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.17)


set(__zisc_dir__ ${CMAKE_SOURCE_DIR}/source/zisc)


function(getExampleWarningFlags example_warning_flags)
  set(warning_flags "")

  # Suppress warnings
  if(ZISC_SUPPRESS_EXCESSIVE_WARNING)
    if(Z_CLANG)
      list(APPEND warning_flags -Wno-float-equal
                                -Wno-sign-conversion 
                                )
    elseif(Z_GCC)
      list(APPEND warning_flags -Wno-sign-conversion 
                                )
    endif()
  endif()

  set(${example_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(getExampleWarningFlags)


# Add an example
function(addExample target save_prerequisites)
  set(example_source_files ${ARGN})
  add_executable(${target} ${example_source_files})
  source_group(${PROJECT_NAME} FILES ${example_source_files})

  #
  set_target_properties(${target} PROPERTIES CXX_STANDARD 20
                                             CXX_STANDARD_REQUIRED ON)

  include(${__zisc_dir__}/cmake/compiler.cmake)
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  getExampleWarningFlags(example_warning_flags)
  target_compile_options(${target} PRIVATE ${cxx_compile_warning_flags}
                                           ${example_warning_flags})
  target_link_libraries(${target} PRIVATE Zisc)
  Zisc_enableIpo(${target})

  #
  Zisc_setStaticAnalyzer(${target})
  Zisc_createLinkToTarget(${target} ${PROJECT_BINARY_DIR})

  # Save the dependency list
  if(save_prerequisites)
    Zisc_savePrerequisites(${target} "" "" "")
  endif()
endfunction(addExample)


#
set(project_description "Examples of Zisc")
project(Examples VERSION 0.0.1 DESCRIPTION "${project_description}" LANGUAGES  CXX)

# Initialize platform info
include(${__zisc_dir__}/cmake/general.cmake)
include(${__zisc_dir__}/cmake/platform.cmake)
Zisc_getPlatformFlags(platform_definitions)
Zisc_setVariablesOnCMake(${platform_definitions})

# Check dependencies
Zisc_checkTarget(Zisc)

# Build examples
addExample(AlgorithmExample ON
           ${PROJECT_SOURCE_DIR}/algorithm_example.cpp)
addExample(CmjEngineExample OFF
           ${PROJECT_SOURCE_DIR}/correlated_multi_jittered_engine_example.cpp)
addExample(CompensatedSummationExample OFF
           ${PROJECT_SOURCE_DIR}/compensated_summation_example.cpp)
addExample(CsvExample OFF
           ${PROJECT_SOURCE_DIR}/csv_example.cpp)
addExample(CumulativeDistributionFunctionExample OFF
           ${PROJECT_SOURCE_DIR}/cumulative_distribution_function_example.cpp)
addExample(ErrorExample OFF
           ${PROJECT_SOURCE_DIR}/error_example.cpp)
addExample(LinearAlgebraExample OFF
           ${PROJECT_SOURCE_DIR}/linear_algebra_example.cpp) 
addExample(LockFreeBoundedQueueExample OFF
           ${PROJECT_SOURCE_DIR}/lock_free_bounded_queue_example.cpp)
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
