# file: platform.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


include_guard()


# Set platform related compile flags to the given target
# scope: 'PUBLIC', 'PRIVATE' or 'INTERFACE'
function(Zisc_setPlatformFlags target scope)
  # Include dependencies
  include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake)

  Zisc_checkTarget(${target})

  # Set system definition
  set(supported_system_list "Windows" "Linux" "Darwin")
  if(NOT CMAKE_SYSTEM_NAME IN_LIST supported_system_list)
    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
  endif()
  target_compile_definitions(${target} ${scope}
                             $<$<PLATFORM_ID:Windows>:Z_SYSTEM_WINDOWS>
                             $<$<PLATFORM_ID:Linux>:Z_SYSTEM_LINUX>
                             $<$<PLATFORM_ID:Darwin>:Z_SYSTEM_DARWIN>
                             )

  # Set architecture definition
  if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
    target_compile_definitions(${target} ${scope} Z_ARCHITECTURE_AMD64)
  else()
    message(WARNING "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
  endif()

  # Set compiler definitions
  get_property(language_list GLOBAL PROPERTY ENABLED_LANGUAGES)
  set(supported_compiler_list "AppleClang" "Clang" "GNU" "MSVC")
  if(("C" IN_LIST language_list) AND
     (NOT CMAKE_C_COMPILER_ID IN_LIST supported_compiler_list))
    message(WARNING "Unsupported C   compiler: ${CMAKE_C_COMPILER_ID}")
  endif()
  if(("CXX" IN_LIST language_list) AND
     (NOT CMAKE_CXX_COMPILER_ID IN_LIST supported_compiler_list))
    message(WARNING "Unsupported C++ compiler: ${CMAKE_C_COMPILER_ID}")
  endif()
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>>)
  set(has_apple_clang $<OR:$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)
  target_compile_definitions(${target} ${scope}
                             $<${has_gcc}:Z_COMPILER_GNU>
                             $<${has_clang}:Z_COMPILER_CLANG>
                             $<${has_apple_clang}:Z_COMPILER_APPLE_CLANG>
                             $<${has_msvc}:Z_COMPILER_MSVC>
                             )

  # Set generator definition
  if(CMAKE_GENERATOR MATCHES ".*Makefiles")
    target_compile_definitions(${target} ${scope} Z_GENERATOR_MAKEFILE)
  elseif(CMAKE_GENERATOR MATCHES "Visual Studio.*")
    target_compile_definitions(${target} ${scope} Z_GENERATOR_VISUAL_STUDIO)
  elseif(CMAKE_GENERATOR MATCHES "Xcode")
    target_compile_definitions(${target} ${scope} Z_GENERATOR_XCODE)
  elseif(CMAKE_GENERATOR MATCHES "Ninja.*")
    target_compile_definitions(${target} ${scope} Z_GENERATOR_NINJA)
  else()
    message(WARNING "Unsupported generator: ${CMAKE_GENERATOR}")
  endif()

  # Set build type definitions
  set(supported_build_type_list "Debug" "RelWithDebInfo" "Release")
  if(NOT CMAKE_BUILD_TYPE IN_LIST supported_build_type_list)
    message(WARNING "Unsupported build type: ${CMAKE_BUILD_TYPE}")
  endif()
  target_compile_definitions(${target} ${scope} 
                             $<$<CONFIG:Debug>:Z_CONFIG_DEBUG>
                             $<$<CONFIG:RelWithDebInfo>:Z_CONFIG_DEBUG;Z_CONFIG_RELEASE>
                             $<$<CONFIG:Release>:Z_CONFIG_RELEASE>
                             )
endfunction(Zisc_setPlatformFlags)


## Check development environment
#function(Zisc_getPlatformFlags platform_definitions)
#  set(definitions "")
#
#  # Detect Platform
#  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    set(system_definition Z_WINDOWS)
#  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#    set(system_definition Z_LINUX)
#  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(system_definition Z_MAC)
#  else()
#    set(unsupported_platform ON)
#  endif()
#  list(APPEND definitions ${system_definition})
#  if(unsupported_platform)
#    message(WARNING "Unsupported system: ${CMAKE_SYSTEM_NAME}")
#  endif()
#
#  # Detect target architecture
#  if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
#    set(architecture_definition Z_AMD64)
#  else()
#    set(unsupported_architecture ON)
#  endif()
#  list(APPEND definitions ${architecture_definition})
#  if(unsupported_architecture)
#    message(WARNING "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
#  endif()
#
#  # Detect C++ compiler
#  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#    set(compiler_definition Z_GCC)
#  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#    set(compiler_definition Z_CLANG)
#  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
#    set(compiler_definition Z_CLANG Z_APPLE_CLANG)
#  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    set(compiler_definition Z_MSVC)
#  else()
#    set(unsupported_compiler ON)
#  endif()
#  list(APPEND definitions ${compiler_definition})
#  if(unsupported_compiler)
#    message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
#  endif()
#  #  if(CMAKE_CXX_COMPILER_VERSION)
#  #    list(APPEND definitions "Z_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}")
#  #  endif()
#
#  # Detect environment
#  if(CMAKE_GENERATOR MATCHES ".*Makefiles")
#    set(generator_definition Z_MAKEFILE)
#  elseif(CMAKE_GENERATOR MATCHES "Visual Studio.*")
#    set(generator_definition Z_VISUAL_STUDIO)
#  elseif(CMAKE_GENERATOR MATCHES "Xcode")
#    set(generator_definition "Z_XCODE")
#  else()
#    set(unsupported_generator ON)
#  endif()
#  list(APPEND definitions ${generator_definition})
#  if(unsupported_generator)
#    message(WARNING "Unsupported generator: ${CMAKE_GENERATOR}")
#  endif()
#
#  # Detect build type
#  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    set(build_mode_definition Z_DEBUG_MODE)
#  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#    set(build_mode_definition Z_DEBUG_MODE Z_RELEASE_MODE)
#  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    set(build_mode_definition Z_RELEASE_MODE)
#  else()
#    set(unsupported_build_type ON)
#  endif()
#  list(APPEND definitions ${build_mode_definition})
#  if(unsupported_build_type)
#    message(WARNING "Unsupported build type: ${CMAKE_BUILD_TYPE}")
#  endif()
#
#
#  # Output variable
#  set(${platform_definitions} ${definitions} PARENT_SCOPE)
#endfunction(Zisc_getPlatformFlags)


# Get the list of supported feature levels
function(Zisc_getFeatureLevelList level_list)
  # Used options
  set(has_hardware_feature ${Z_ENABLE_HARDWARE_FEATURES})

  # Get the list
  set(feature_level_list)
  if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") OR (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64"))
    list(APPEND feature_level_list "Amd64V1")
    if(has_hardware_feature)
      list(APPEND feature_level_list "Amd64V2" "Amd64V3" "Amd64V4")
    endif()
  endif()

  # Output
  set(${level_list} ${feature_level_list} PARENT_SCOPE)
endfunction(Zisc_getFeatureLevelList)


# Check if the specified feature level is supported on this machine
function(Zisc_checkIfFeatureLevelIsSupported feature_level)
  Zisc_getFeatureLevelList(supported_level_list)
  if(NOT feature_level IN_LIST supported_level_list)
    message(FATAL_ERROR "'${feature_level}' is not supported.")
  endif()
endfunction(Zisc_checkIfFeatureLevelIsSupported)
