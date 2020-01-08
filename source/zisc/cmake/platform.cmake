# file: platform.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


# Check development environment
function(getPlatformFlags platform_definitions)
  set(definitions "")

  # Detect Platform
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(system_definition Z_WINDOWS)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(system_definition Z_LINUX)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(system_definition Z_MAC)
  else()
    set(unsupported_platform ON)
  endif()
  list(APPEND definitions ${system_definition})
  if(unsupported_platform)
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
  endif()

  # Detect C++ compiler
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(compiler_definition Z_GCC)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(compiler_definition Z_CLANG)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(compiler_definition Z_CLANG Z_APPLE_CLANG)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(compiler_definition Z_MSVC)
  else()
    set(unsupported_compiler ON)
  endif()
  list(APPEND definitions ${compiler_definition})
  if(unsupported_compiler)
    message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
  #  if(CMAKE_CXX_COMPILER_VERSION)
  #    list(APPEND definitions "Z_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}")
  #  endif()

  # Detect environment
  if(CMAKE_GENERATOR MATCHES ".*Makefiles")
    set(generator_definition Z_MAKEFILE)
  elseif(CMAKE_GENERATOR MATCHES "Visual Studio.*")
    set(generator_definition Z_VISUAL_STUDIO)
  elseif(CMAKE_GENERATOR MATCHES "Xcode")
    set(generator_definition "Z_XCODE")
  else()
    set(unsupported_generator ON)
  endif()
  list(APPEND definitions ${generator_definition})
  if(unsupported_generator)
    message(WARNING "Unsupported generator: ${CMAKE_GENERATOR}")
  endif()


  # Detect build type
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_mode_definition Z_DEBUG_MODE)
  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(build_mode_definition Z_DEBUG_MODE Z_RELEASE_MODE)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_mode_definition Z_RELEASE_MODE)
  else()
    set(unsupported_build_type ON)
  endif()
  list(APPEND definitions ${build_mode_definition})
  if(unsupported_build_type)
    message(WARNING "Unsupported build type: ${CMAKE_BUILD_TYPE}")
  endif()


  # Output variable
  set(${platform_definitions} ${definitions} PARENT_SCOPE)
endfunction(getPlatformFlags)
