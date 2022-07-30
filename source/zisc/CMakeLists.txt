# file:CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#

cmake_minimum_required(VERSION 3.22)


function(Zisc_initZiscOptions)
  # Atomic
  set(option_description "Use OS specified atomic wait-notification.")
  Zisc_setBooleanOption(ZISC_ATOMIC_OS_SPECIFIED_WAIT ON "${option_description}")

  set(option_description "Enable zisc assertion.")
  if(Z_DEBUG_MODE)
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION ON "${option_description}")
  else()
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION OFF "${option_description}")
  endif()

  set(option_description "L1 cache line size")
  Zisc_setStringOption(ZISC_L1_CACHE_LINE_SIZE "128" "${option_description}")
endfunction(Zisc_initZiscOptions)


function(Zisc_getZiscFlags zisc_compile_flags zisc_linker_flags zisc_definitions)
  # Options
  if(${ZISC_ATOMIC_OS_SPECIFIED_WAIT})
    list(APPEND definitions ZISC_ATOMIC_OS_SPECIFIED_WAIT=1)
  endif()

  if(${ZISC_ENABLE_ASSERTION})
    list(APPEND definitions ZISC_ASSERTION=1)
  endif()

  if(Z_WINDOWS)
    list(APPEND definitions _ENABLE_EXTENDED_ALIGNED_STORAGE=1)
  endif()

  # Output variables
  set(${zisc_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getZiscFlags)


function(Zisc_getZiscWarningFlags zisc_warning_flags)
  set(warning_flags "")

  if(Z_CLANG)
    # list(APPEND warning_flags
    #                          )
  elseif(Z_GCC)
    # list(APPEND warning_flags
    #                          )
  endif()

  # Output variables
  set(${zisc_warning_flags} ${warning_flags} PARENT_SCOPE)
endfunction(Zisc_getZiscWarningFlags)


#
macro(Zisc_setZiscProject)
  set(project_description "Zisc provides commonly used C++ classes and functions.")
  project(Zisc VERSION 0.0.71 DESCRIPTION "${project_description}" LANGUAGES CXX)
  message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

  include("${PROJECT_SOURCE_DIR}/cmake/general.cmake")
  include("${PROJECT_SOURCE_DIR}/cmake/platform.cmake")
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})
  Zisc_initZiscOptions()

  cmake_path(SET zisc_core_dir "${PROJECT_SOURCE_DIR}/core")

  # Make zisc configuration header file
  cmake_path(SET zisc_config_dir "${PROJECT_BINARY_DIR}/include/zisc")
  file(MAKE_DIRECTORY "${zisc_config_dir}")
  configure_file("${zisc_core_dir}/zisc/zisc_config-inl.hpp.in"
                 "${zisc_config_dir}/zisc_config-inl.hpp"
                 @ONLY)

  # Create Zisc library
  file(GLOB_RECURSE zisc_source_files ${zisc_core_dir}/zisc/*[.hpp|.cpp])
  list(APPEND zisc_source_files "${zisc_config_dir}/zisc_config-inl.hpp")
  add_library(${PROJECT_NAME} STATIC "${zisc_source_files}")
  source_group(${PROJECT_NAME} FILES "${zisc_source_files}")

  # Find dependencies
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # Compiler flags
  include("${PROJECT_SOURCE_DIR}/cmake/compiler.cmake")
  Zisc_initCompilerOptions()
  set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20
                                                   CXX_STANDARD_REQUIRED ON)

  Zisc_getCxxCompilerFlags(cxx_compile_flags cxx_linker_flags cxx_definitions)
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  Zisc_getZiscFlags(zisc_compile_flags zisc_linker_flags zisc_definitions)
  Zisc_getZiscWarningFlags(zisc_warning_flags)
  target_compile_options(${PROJECT_NAME} PUBLIC ${cxx_compile_flags}
                                                ${zisc_compile_flags}
                                         PRIVATE ${cxx_compile_warning_flags}
                                                 ${zisc_warning_flags})
  target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
  if(Z_WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE Synchronization.lib)
  endif()
  target_link_options(${PROJECT_NAME} PUBLIC ${cxx_linker_flags}
                                             ${zisc_linker_flags})
  target_compile_definitions(${PROJECT_NAME} PUBLIC ${platform_definitions}
                                                    ${cxx_definitions}
                                                    ${zisc_definitions}
                                             PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_include_directories(${PROJECT_NAME} PUBLIC "${zisc_core_dir}"
                                                    "${PROJECT_BINARY_DIR}/include")
  Zisc_enableIpo(${PROJECT_NAME})

  #
  set(clang_tidy_exclusion_checks readability-identifier-length
                                  )
  Zisc_setStaticAnalyzer(${PROJECT_NAME}
                         CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})
endmacro(Zisc_setZiscProject)


##
Zisc_setZiscProject()
