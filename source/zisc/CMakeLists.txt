# file:CMakeLists.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
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
  project(Zisc VERSION ${ZiscProject_VERSION} DESCRIPTION "${project_description}" LANGUAGES CXX)
  message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

  include("${PROJECT_SOURCE_DIR}/cmake/general.cmake")
  include("${PROJECT_SOURCE_DIR}/cmake/platform.cmake")
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})
  Zisc_initZiscOptions()

  # Find dependencies
  if(NOT TARGET Threads::Threads)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
  endif()

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
  source_group(${PROJECT_NAME} FILES "${zisc_source_files}")

  # Compiler flags
  include("${PROJECT_SOURCE_DIR}/cmake/compiler.cmake")
  Zisc_initCompilerOptions()
  Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
  Zisc_getSanitizerFlags(compile_sanitizer_flags linker_sanitizer_flags)
  Zisc_getZiscFlags(zisc_compile_flags zisc_linker_flags zisc_definitions)
  Zisc_getZiscWarningFlags(zisc_warning_flags)
  # Get architecture
  Zisc_getArchitectureName(FALSE arch_name_list)
  foreach(architecture IN LISTS arch_name_list)
    Zisc_getArchitectureTargetName(${PROJECT_NAME} ${architecture} library_name)
    add_library(${library_name} STATIC "${zisc_source_files}")
    Zisc_getCxxCompilerFlags(${architecture} cxx_compile_flags cxx_linker_flags cxx_definitions)
    list(APPEND cxx_definitions ZISC_ARCHITECTURE_NAME=\"${architecture}\")
    set_target_properties(${library_name} PROPERTIES CXX_STANDARD 20
                                                     CXX_STANDARD_REQUIRED ON
                                                     POSITION_INDEPENDENT_CODE ON)
    target_compile_options(${library_name} PUBLIC ${cxx_compile_flags}
                                                  ${zisc_compile_flags}
                                           PRIVATE ${cxx_compile_warning_flags}
                                                   ${compile_sanitizer_flags}
                                                   ${zisc_warning_flags})
    target_link_libraries(${library_name} PUBLIC Threads::Threads)
    if(Z_WINDOWS)
      target_link_libraries(${library_name} PRIVATE Synchronization.lib)
    endif()
    target_link_options(${library_name} PUBLIC ${cxx_linker_flags}
                                               ${zisc_linker_flags}
                                        PRIVATE ${linker_sanitizer_flags})
    target_compile_definitions(${library_name} PUBLIC ${platform_definitions}
                                                      ${cxx_definitions}
                                                      ${zisc_definitions}
                                               PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_include_directories(${library_name} PUBLIC "${zisc_core_dir}"
                                                      "${PROJECT_BINARY_DIR}/include")
    Zisc_enableIpo(${library_name})

    # Set analyzer
    set(clang_tidy_exclusion_checks 
                                    cppcoreguidelines-avoid-do-while
                                    cppcoreguidelines-macro-usage
                                    cppcoreguidelines-pro-type-vararg
                                    google-runtime-int
                                    hicpp-vararg
                                    misc-include-cleaner
                                    performance-avoid-endl
                                    readability-identifier-length
                                    )
    Zisc_setStaticAnalyzer(${library_name}
                           CLANG_TIDY_HEADER_PATHS ${PROJECT_SOURCE_DIR}/core/zisc/
                           CLANG_TIDY_EXCLUSION_CHECKS ${clang_tidy_exclusion_checks})
  endforeach(architecture)
endmacro(Zisc_setZiscProject)


##
Zisc_setZiscProject()
