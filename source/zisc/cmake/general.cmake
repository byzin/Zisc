# file: general.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


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
    message(FATAL_ERROR "\"${PROJECT_BINARY_DIR}\" dir is not allowed to build.\n"
                        "Please build on \"${build_dir}\".")
  endif()
endfunction(Zisc_restrictBuildDirectory)


#
function(Zisc_checkSubmodule submodule_path)
  if(NOT EXISTS "${submodule_path}")
    cmake_path(GET submodule_path FILENAME name)
    message(FATAL_ERROR "Submodule '${name}' not found. Please init the submodule.")
  endif()
endfunction(Zisc_checkSubmodule)


#
function(Zisc_checkTarget target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Target '${target}' not found. Please init the target.")
  endif()
endfunction(Zisc_checkTarget)


# Build GoogleTest libraries
function(Zisc_addGoogleTest source_dir binary_dir)
  message(STATUS "Add GoogleTest library.")
  Zisc_checkSubmodule("${source_dir}")
  set(BUILD_GMOCK OFF CACHE INTERNAL "" FORCE)
  set(INSTALL_GTEST OFF CACHE INTERNAL "" FORCE)
  set(gtest_force_shared_crt ON CACHE INTERNAL "" FORCE) # Prevent overriding the parent project's compiler/linker settings on Windows
  set(gtest_build_tests OFF CACHE INTERNAL "" FORCE)
  set(gtest_build_samples OFF CACHE INTERNAL "" FORCE)
  set(gtest_disable_pthreads ON CACHE INTERNAL "" FORCE)
  set(gtest_hide_internal_symbols ON CACHE INTERNAL "" FORCE)
  add_subdirectory("${source_dir}" "${binary_dir}" EXCLUDE_FROM_ALL)
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

  message(STATUS "${message_prefix}Build mode: ${CMAKE_BUILD_TYPE}")
  message(STATUS "${message_prefix}Generator: ${CMAKE_GENERATOR}")
  message(STATUS "${message_prefix}Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
  message(STATUS "${message_prefix}Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
  message(STATUS "${message_prefix}Target architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endfunction(Zisc_printSystemInfo)


function(Zisc_setVariablesOnCMake)
  foreach(value IN LISTS ARGV)
    set(${value} ON PARENT_SCOPE)
  endforeach(value)
endfunction(Zisc_setVariablesOnCMake)
