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
  set(has_msvc $<OR:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(has_gcc $<OR:$<C_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:GNU>>)
  set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>>)
  set(has_apple_clang $<OR:$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)
  target_compile_definitions(${target} ${scope}
                             $<${has_msvc}:Z_COMPILER_MSVC>
                             $<${has_gcc}:Z_COMPILER_GNU>
                             $<${has_clang}:Z_COMPILER_CLANG>
                             $<${has_apple_clang}:Z_COMPILER_APPLE_CLANG>
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
