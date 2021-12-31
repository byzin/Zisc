# file: CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.22)


set(__zisc_dir__ ${CMAKE_CURRENT_LIST_DIR}/../../source/zisc)


function(getTestCompileFlags test_compile_flags test_linker_flags test_definitions)
  set(compile_flags "")
  set(linker_flags "")
  set(definitions "")

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
      list(APPEND warning_flags -Wno-double-promotion
                                -Wno-float-equal
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
macro(setMathReferenceProject)
  set(project_description "Output math references.")
  project(MathReference VERSION 0.0.1 DESCRIPTION ${project_description} LANGUAGES CXX)

  # Find GMP and MPFR
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
  find_package(GMP)
  find_package(MPFR)
  if((NOT GMP_FOUND) OR (NOT MPFR_FOUND))
    return()
  endif()

  # Initialize platform info
  include(${__zisc_dir__}/cmake/general.cmake)
  include(${__zisc_dir__}/cmake/platform.cmake)
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})

  # Check dependencies
  Zisc_checkTarget(Zisc)

  # Create a unit test
  file(GLOB math_source_files ${PROJECT_SOURCE_DIR}/*.cpp
                              ${PROJECT_SOURCE_DIR}/*.hpp)
  add_executable(${PROJECT_NAME} ${math_source_files})
  source_group(${PROJECT_NAME} FILES ${math_source_files})

  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 
                                                   CXX_STANDARD_REQUIRED ON)

  include(${__zisc_dir__}/cmake/compiler.cmake)
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  getTestCompileFlags(test_compile_flags test_linker_flags test_definitions)
  getTestWarningFlags(test_warning_flags)
  target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${GMP_INCLUDES}
                                                            ${MPFR_INCLUDES}
                                                            ${PROJECT_SOURCE_DIR}/dependencies/mpreal)
  target_compile_options(${PROJECT_NAME} PRIVATE ${test_compile_flags}
                                                 ${cxx_compile_warning_flags}
                                                 ${test_warning_flags})
  target_link_libraries(${PROJECT_NAME} PRIVATE ${GMP_LIBRARIES}
                                                ${MPFR_LIBRARIES}
                                                Zisc
                                                ${test_linker_flags})
  target_compile_definitions(${PROJECT_NAME} PRIVATE ${test_definitions})
  Zisc_enableIpo(${PROJECT_NAME})

  #
  Zisc_setStaticAnalyzer(${PROJECT_NAME})
  Zisc_createLinkToTarget(${PROJECT_NAME} ${PROJECT_BINARY_DIR})

  # installation settings
  ## Save dependency list
  Zisc_getSaveDependencyCode(${PROJECT_NAME} ${PROJECT_BINARY_DIR} "" "" "" dependency_code)
  install(CODE ${dependency_code} COMPONENT test)
  ## Install actual binary
  install(TARGETS ${PROJECT_NAME} DESTINATION bin COMPONENT test)
endmacro(setMathReferenceProject)


##
setMathReferenceProject()
