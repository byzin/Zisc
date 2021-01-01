# file:CMakeLists.txt
# author: Sho Ikeda
# 
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.17)


function(Zisc_initZiscOptions)
  # Atomic
  set(option_description "Use OS specified atomic wait-notification.")
  Zisc_setBooleanOption(ZISC_ATOMIC_OS_SPECIFIED_WAIT ON ${option_description})

  set(option_description "Enable zisc assertion.")
  if(Z_DEBUG_MODE)
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION ON ${option_description})
  else()
    Zisc_setBooleanOption(ZISC_ENABLE_ASSERTION OFF ${option_description})
  endif()

  set(option_description "L1 cache line size")
  Zisc_setStringOption(ZISC_L1_CACHE_LINE_SIZE "128" ${option_description})
endfunction(Zisc_initZiscOptions)


function(Zisc_getZiscFlags zisc_compile_flags zisc_linker_flags zisc_definitions)
  # Options
  if(${ZISC_ATOMIC_OS_SPECIFIED_WAIT})
    list(APPEND definitions ZISC_ATOMIC_OS_SPECIFIED_WAIT=1)
  endif()

  if(${ZISC_ENABLE_ASSERTION})
    list(APPEND definitions ZISC_ASSERTION=1)
  endif()


  # Output variables
  set(${zisc_definitions} ${definitions} PARENT_SCOPE)
endfunction(Zisc_getZiscFlags)


#
set(project_description "Zisc provides commonly used C++ classes and functions.")
project(Zisc VERSION 0.0.62 DESCRIPTION "${project_description}" LANGUAGES CXX)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(${PROJECT_SOURCE_DIR}/cmake/general.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)
Zisc_getPlatformFlags(platform_definitions)
Zisc_setVariablesOnCMake(${platform_definitions})
Zisc_initZiscOptions()

# Make zisc configuration header file
set(zisc_config_dir ${PROJECT_BINARY_DIR}/include/zisc)
file(MAKE_DIRECTORY ${zisc_config_dir})
configure_file(${PROJECT_SOURCE_DIR}/zisc/zisc_config-inl.hpp.in
               ${zisc_config_dir}/zisc_config-inl.hpp
               @ONLY)

# Create Zisc library
file(GLOB_RECURSE zisc_source_files ${PROJECT_SOURCE_DIR}/zisc/*.cpp
                                    ${PROJECT_SOURCE_DIR}/zisc/*.hpp)
list(APPEND zisc_source_files ${zisc_config_dir}/zisc_config-inl.hpp)
add_library(${PROJECT_NAME} STATIC ${zisc_source_files})
source_group(${PROJECT_NAME} FILES ${zisc_source_files})

# Find dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Compiler flags
include(${PROJECT_SOURCE_DIR}/cmake/compiler.cmake)
Zisc_initCompilerOptions()
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20
                                                 CXX_STANDARD_REQUIRED ON)

Zisc_getCxxFeatureList(cxx_feature_list)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20 ${cxx_feature_list})

Zisc_getCxxCompilerFlags(cxx_compile_flags cxx_linker_flags cxx_definitions)
Zisc_getCxxWarningFlags(cxx_compile_warning_flags)
Zisc_getZiscFlags(zisc_compile_flags zisc_linker_flags zisc_definitions)
target_compile_options(${PROJECT_NAME} PUBLIC ${cxx_compile_flags}
                                              ${zisc_compile_flags}
                                       PRIVATE ${cxx_compile_warning_flags})
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
if(Z_WINDOWS)
  target_link_libraries(${PROJECT_NAME} PRIVATE Synchronization.lib)
endif()
target_link_options(${PROJECT_NAME} PUBLIC ${cxx_linker_flags}
                                           ${zisc_linker_flags})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${platform_definitions}
                                                  ${cxx_definitions}
                                                  ${zisc_definitions})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}
                                                  ${PROJECT_BINARY_DIR}/include)
Zisc_enableIpo(${PROJECT_NAME})

#
Zisc_setStaticAnalyzer(${PROJECT_NAME})
