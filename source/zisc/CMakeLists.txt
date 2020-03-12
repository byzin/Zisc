# file:CMakeLists.txt
# author: Sho Ikeda
# 
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

cmake_minimum_required(VERSION 3.16)


function(initZiscOptions)
  # Algorithm
  set(option_description "Use STL binary tree algorithm.")
  setBooleanOption(ZISC_ALGORITHM_STL_BINARY_TREE OFF ${option_description})

  # Atomic
  set(option_description "Use OS specified atomic wait-notification.")
  setBooleanOption(ZISC_ATOMIC_OS_SPECIFIED_WAIT ON ${option_description})

  # Math
  set(option_description "Use STL math funcs instead of the Zisc funcs.")
  setBooleanOption(ZISC_MATH_STL OFF ${option_description})

  set(option_description "Use STL 'exp' instead of the Zisc func.")
  setBooleanOption(ZISC_MATH_STL_EXP ON ${option_description})

  set(option_description "Use STL 'log', 'log2' funcs instead of the Zisc funcs.")
  setBooleanOption(ZISC_MATH_STL_LOG ON ${option_description})

  set(option_description "Use STL 'pow' func instead of the Zisc func")
  setBooleanOption(ZISC_MATH_STL_POW ON ${option_description})

  set(option_description "Use STL 'sqrt' func instead of the Zisc func")
  setBooleanOption(ZISC_MATH_STL_SQRT ON ${option_description})

  set(option_description "Use STL 'cbrt' func instead of the Zisc func")
  setBooleanOption(ZISC_MATH_STL_CBRT ON ${option_description})

  set(option_description "Use STL 'sin', 'cos' and 'tan' funcs instead of the Zisc funcs")
  setBooleanOption(ZISC_MATH_STL_TRIGONOMETRIC ON ${option_description})

  set(option_description "Use STL 'asin', 'acos' and 'atan' funcs instead of the Zisc funcs")
  setBooleanOption(ZISC_MATH_STL_INV_TRIGONOMETRIC ON ${option_description})

  set(option_description "Enable zisc assertion.")
  if(Z_DEBUG_MODE)
    setBooleanOption(ZISC_ENABLE_ASSERTION ON ${option_description})
  else()
    setBooleanOption(ZISC_ENABLE_ASSERTION OFF ${option_description})
  endif()
endfunction(initZiscOptions)


function(getZiscFlags zisc_compile_flags zisc_linker_flags zisc_definitions)
  # Options
  if(${ZISC_ALGORITHM_STL_BINARY_TREE})
    list(APPEND definitions ZISC_ALGORITHM_STL_BINARY_TREE=1)
  endif()
  if(${ZISC_ATOMIC_OS_SPECIFIED_WAIT})
    list(APPEND definitions ZISC_ATOMIC_OS_SPECIFIED_WAIT=1)
  endif()
  if(${ZISC_MATH_STL})
    list(APPEND definitions ZISC_MATH_STL=1)
  endif()
  if(${ZISC_MATH_STL_EXP})
    list(APPEND definitions ZISC_MATH_STL_EXP=1)
  endif()
  if(${ZISC_MATH_STL_LOG})
    list(APPEND definitions ZISC_MATH_STL_LOG=1)
  endif()
  if(${ZISC_MATH_STL_POW})
    list(APPEND definitions ZISC_MATH_STL_POW=1)
  endif()
  if(${ZISC_MATH_STL_SQRT})
    list(APPEND definitions ZISC_MATH_STL_SQRT=1)
  endif()
  if(${ZISC_MATH_STL_CBRT})
    list(APPEND definitions ZISC_MATH_STL_CBRT=1)
  endif()
  if(${ZISC_MATH_STL_TRIGONOMETRIC})
    list(APPEND definitions ZISC_MATH_STL_TRIGONOMETRIC=1)
  endif()
  if(${ZISC_MATH_STL_INV_TRIGONOMETRIC})
    list(APPEND definitions ZISC_MATH_STL_INV_TRIGONOMETRIC=1)
  endif()
  if(${ZISC_ENABLE_ASSERTION})
    list(APPEND definitions ZISC_ASSERTION=1)
  endif()


  # Output variables
  set(${zisc_definitions} ${definitions} PARENT_SCOPE)
endfunction(getZiscFlags)


#
set(project_description "Zisc provides commonly used C++ classes and functions.")
project(Zisc VERSION 0.0.57 DESCRIPTION "${project_description}" LANGUAGES CXX)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include(${PROJECT_SOURCE_DIR}/cmake/general.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)
getPlatformFlags(platform_definitions)
setVariablesOnCMake(${platform_definitions})
initZiscOptions()

# Make zisc configuration header file
set(zisc_config_dir ${PROJECT_BINARY_DIR}/include/zisc)
file(MAKE_DIRECTORY ${zisc_config_dir})
configure_file(${PROJECT_SOURCE_DIR}/zisc/zisc_config-inl.hpp.in
               ${zisc_config_dir}/zisc_config-inl.hpp
               @ONLY)

# Create Zisc library
file(GLOB zisc_source_files ${PROJECT_SOURCE_DIR}/zisc/*.cpp
                            ${PROJECT_SOURCE_DIR}/zisc/*.hpp)
list(APPEND zisc_source_files ${zisc_config_dir}/zisc_config-inl.hpp)
source_group(${PROJECT_NAME} FILES ${zisc_source_files})
add_library(${PROJECT_NAME} STATIC ${zisc_source_files})

# Find dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Compiler flags
include(${PROJECT_SOURCE_DIR}/cmake/compiler.cmake)
initCompilerOptions()
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17
                                                 CXX_STANDARD_REQUIRED ON)

getCxxFeatureList(cxx_feature_list)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17 ${cxx_feature_list})

getCxxCompilerFlags(cxx_compile_flags cxx_linker_flags cxx_definitions)
getCxxWarningFlags(cxx_compile_warning_flags)
getZiscFlags(zisc_compile_flags zisc_linker_flags zisc_definitions)
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

#
setStaticAnalyzer(${PROJECT_NAME})
