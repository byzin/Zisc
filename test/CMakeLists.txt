# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.16)


set(__zisc_dir__ ${CMAKE_SOURCE_DIR}/source/zisc)


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


#
set(project_description "Zisc unit test.")
project(UnitTest VERSION 0.0.1 DESCRIPTION ${project_description} LANGUAGES CXX)


# Initialize platform info
include(${__zisc_dir__}/cmake/general.cmake)
include(${__zisc_dir__}/cmake/platform.cmake)
getPlatformFlags(platform_definitions)
setVariablesOnCMake(${platform_definitions})

# Check dependencies
checkTarget(Zisc)
checkTarget(gtest_main)

# Copy resources
file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${PROJECT_BINARY_DIR})

# Create a unit test
file(GLOB unittest_source_files ${PROJECT_SOURCE_DIR}/unittest/*.cpp
                                ${PROJECT_SOURCE_DIR}/unittest/*.hpp)
source_group(${PROJECT_NAME} FILES ${unittest_source_files})
add_executable(${PROJECT_NAME} ${unittest_source_files})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR})

include(${__zisc_dir__}/cmake/compiler.cmake)
getCxxWarningFlags(cxx_compile_warning_flags)
getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
getTestWarningFlags(test_warning_flags)
target_compile_options(${PROJECT_NAME} PRIVATE ${test_compile_flags}
                                               ${cxx_compile_warning_flags}
                                               ${test_warning_flags})
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main
                                              Zisc
                                              ${test_linker_flags})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${test_definitions})

#
setStaticAnalyzer(${PROJECT_NAME})
