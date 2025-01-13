# file: general.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2024 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


include_guard()


# Set boolean value option
macro(Zisc_setBooleanOption variable value doc_string)
  set(${variable} ${value} CACHE BOOL "${doc_string}")
endmacro(Zisc_setBooleanOption)


# Set string value option
macro(Zisc_setStringOption variable value doc_string)
  set(${variable} ${value} CACHE STRING "${doc_string}")
endmacro(Zisc_setStringOption)


#
macro(Zisc_setInternalValue variable value)
  set("${variable}" ${value} CACHE INTERNAL "" FORCE)
endmacro(Zisc_setInternalValue)


# Prohibit building on all directories except debug and release directory.
function(Zisc_restrictBuildDirectory build_dir)
  if(NOT PROJECT_BINARY_DIR MATCHES "${build_dir}(/*)?")
    message(FATAL_ERROR "\"${PROJECT_BINARY_DIR}\" directory is not allowed to build.\n"
                        "Please build on \"${build_dir}\".")
  endif()
endfunction(Zisc_restrictBuildDirectory)


#
function(Zisc_checkSubmodule submodule_path)
  if(NOT EXISTS "${submodule_path}")
    cmake_path(GET submodule_path FILENAME name)
    message(FATAL_ERROR "Submodule '${name}' not found. Please initialize the submodule.")
  endif()
endfunction(Zisc_checkSubmodule)


#
function(Zisc_checkTarget target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Target '${target}' not found. Please initialize the target.")
  endif()
endfunction(Zisc_checkTarget)


# Build GoogleTest libraries
function(Zisc_addGoogleTest source_dir binary_dir)
  Zisc_checkSubmodule("${source_dir}")
  Zisc_setInternalValue(BUILD_GMOCK OFF)
  Zisc_setInternalValue(INSTALL_GTEST OFF)
  Zisc_setInternalValue(GTEST_HAS_ABSL OFF)
  Zisc_setInternalValue(gtest_force_shared_crt ON) # Prevent overriding the parent project's compiler/linker settings on Windows
  Zisc_setInternalValue(gtest_build_tests OFF)
  Zisc_setInternalValue(gtest_build_samples OFF)
  Zisc_setInternalValue(gtest_disable_pthreads ON)
  Zisc_setInternalValue(gtest_hide_internal_symbols ON)
  add_subdirectory("${source_dir}" "${binary_dir}" EXCLUDE_FROM_ALL)
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.cmake")
  set_target_properties(gtest
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${binary_dir}/bin"
      LIBRARY_OUTPUT_DIRECTORY "${binary_dir}/lib"
      ARCHIVE_OUTPUT_DIRECTORY "${binary_dir}/lib"
      PDB_OUTPUT_DIRECTORY "${binary_dir}/bin")
  set_target_properties(gtest_main
      PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${binary_dir}/bin"
      LIBRARY_OUTPUT_DIRECTORY "${binary_dir}/lib"
      ARCHIVE_OUTPUT_DIRECTORY "${binary_dir}/lib"
      PDB_OUTPUT_DIRECTORY "${binary_dir}/bin")
  # Set warning
  Zisc_getPlatformFlags(platform_definitions)
  Zisc_setVariablesOnCMake(${platform_definitions})
  if(Z_CLANG)
    target_compile_options(gtest PRIVATE -Wno-implicit-int-float-conversion)
    target_compile_options(gtest_main PRIVATE -Wno-implicit-int-float-conversion)
  endif()
endfunction(Zisc_addGoogleTest)


#
function(Zisc_printSystemInfo message_prefix)
  set(info_key_list
      "OS_NAME" "OS name"
      "OS_VERSION" "OS version"
      "OS_PLATFORM" "OS platform"
      "PROCESSOR_NAME" "Processor name"
      "PROCESSOR_DESCRIPTION" "Processor description"
      "NUMBER_OF_PHYSICAL_CORES" "Number of physical cores"
      "NUMBER_OF_LOGICAL_CORES" "Number of logical cores"
      "TOTAL_PHYSICAL_MEMORY" "Toal physical memory (MB)"
      "AVAILABLE_PHYSICAL_MEMORY" "Free physical memory (MB)"
      "TOTAL_VIRTUAL_MEMORY" "Total virtual memory (MB)"
      "AVAILABLE_VIRTUAL_MEMORY" "Free virtual memory (MB)"
      "HOSTNAME" "Host name"
      "FQDN" "Fully qualified domain name"
      )
  set(is_query_key TRUE)
  foreach(key_item IN LISTS info_key_list)
    if(is_query_key)
      cmake_host_system_information(RESULT system_info QUERY ${key_item})
      set(is_query_key FALSE)
    else()
      message(STATUS "${message_prefix}${key_item}: ${system_info}")
      set(is_query_key TRUE)
    endif()
  endforeach(key_item)

  message(STATUS "${message_prefix}Build type          : ${CMAKE_BUILD_TYPE}")
  message(STATUS "${message_prefix}Generator           : ${CMAKE_GENERATOR}")
  message(STATUS "${message_prefix}C++ Compiler ID     : ${CMAKE_CXX_COMPILER_ID}")
  message(STATUS "${message_prefix}C++ Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
  message(STATUS "${message_prefix}Target architecture : ${CMAKE_SYSTEM_PROCESSOR}")
endfunction(Zisc_printSystemInfo)


# Print target compilation related properties for debug at build time
function(Zisc_printTargetCompileProperties target message_prefix)
  Zisc_checkTarget(${target})

  set(echo ${CMAKE_COMMAND} -E echo) # Set echo command to print properties
  set(prefix "${message_prefix}")
  set(prop TARGET_PROPERTY:${target})
  add_custom_command(TARGET ${target}
                     PRE_BUILD
                     COMMENT "'${target}' compilation related properties:"
                     COMMAND ${echo} "${prefix}Build type     : $<CONFIG>"
                     COMMAND ${echo} "${prefix}Platform       : $<PLATFORM_ID>"
                     COMMAND ${echo} "${prefix}C   compiler   : $<$<BOOL:$<C_COMPILER_ID>>:$<C_COMPILER_ID>_$<C_COMPILER_VERSION>_(std=$<TARGET_PROPERTY:${target},C_STANDARD>)>"
                     COMMAND ${echo} "${prefix}C++ compiler   : $<$<BOOL:$<CXX_COMPILER_ID>>:$<CXX_COMPILER_ID>;$<CXX_COMPILER_VERSION>;(std=$<TARGET_PROPERTY:${target},CXX_STANDARD>)>"
                     COMMAND ${echo} "${prefix}Compile options: $<${prop},COMPILE_OPTIONS>"
                     COMMAND ${echo} "${prefix}Compile defs   : $<${prop},COMPILE_DEFINITIONS>"
                     COMMAND ${echo} "${prefix}Link options   : $<${prop},LINK_OPTIONS>"
                     COMMAND_EXPAND_LISTS
                     VERBATIM
                     )
endfunction(Zisc_printTargetCompileProperties)


function(Zisc_setVariablesOnCMake)
  foreach(value IN LISTS ARGV)
    set(${value} ON PARENT_SCOPE)
  endforeach(value)
endfunction(Zisc_setVariablesOnCMake)
