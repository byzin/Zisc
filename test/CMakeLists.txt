# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.17)


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
      list(APPEND warning_flags -Wno-float-equal
                                -Wno-global-constructors
                                -Wno-sign-conversion
                                -Wno-used-but-marked-unused
                                )
      if(Z_VISUAL_STUDIO)
        list(APPEND warning_flags -Wno-language-extension-token
                                  -Wno-shift-sign-overflow
                                  -Wno-sign-compare
                                  -Wno-undef
                                  )
      endif()
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
Zisc_getPlatformFlags(platform_definitions)
Zisc_setVariablesOnCMake(${platform_definitions})

# Check dependencies
Zisc_checkTarget(Zisc)
Zisc_checkTarget(gtest_main)

# Create a symlink to the test resources
file(COPY ${PROJECT_SOURCE_DIR}/resources DESTINATION ${PROJECT_BINARY_DIR})
file(GLOB math_reference_files "${PROJECT_SOURCE_DIR}/math_reference/*.tar.xz")
foreach(reference_path IN LISTS math_reference_files)
  set(src_file_path ${reference_path})
  get_filename_component(reference_file "${reference_path}" NAME_WLE)
  get_filename_component(reference_file "${reference_file}" NAME_WLE)
  set(dst_file_path ${PROJECT_BINARY_DIR}/resources/${reference_file})
  if(${src_file_path} IS_NEWER_THAN ${dst_file_path})
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar Jxf ${src_file_path}
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/resources)
    file(TOUCH_NOCREATE ${dst_file_path})
  endif()
endforeach(reference_path)

# Create a unit test
file(GLOB unittest_source_files ${PROJECT_SOURCE_DIR}/unittest/*.cpp
                                ${PROJECT_SOURCE_DIR}/unittest/*.hpp)
add_executable(${PROJECT_NAME} ${unittest_source_files})
source_group(${PROJECT_NAME} FILES ${unittest_source_files})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 
                                                 CXX_STANDARD_REQUIRED ON)

include(${__zisc_dir__}/cmake/compiler.cmake)
Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
getTestWarningFlags(test_warning_flags)
target_compile_options(${PROJECT_NAME} PRIVATE ${test_compile_flags}
                                               ${cxx_compile_warning_flags}
                                               ${test_warning_flags})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/math_reference)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main
                                              Zisc
                                              ${test_linker_flags})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${test_definitions})
Zisc_enableIpo(${PROJECT_NAME})

#
Zisc_setStaticAnalyzer(${PROJECT_NAME})
Zisc_createLinkToTarget(${PROJECT_NAME} ${PROJECT_BINARY_DIR})

# Save the dependency list
Zisc_savePrerequisites(${PROJECT_NAME} "" "" "")
