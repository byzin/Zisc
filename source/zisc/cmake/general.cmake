# file: general.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2025 Sho Ikeda
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
  if(NOT EXISTS "${submodule_path}/.git")
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


# Create a link of the given target into the output directory
function(Zisc_createLinkToTarget target output_dir)
  #
  Zisc_checkTarget(${target})
  get_target_property(binary_dir ${target} BINARY_DIR)
  set(link_target ${target}_link)

  # Create a linking script
  set(script [[
    cmake_path(GET z_target_path FILENAME file_name)
    cmake_path(APPEND link_path "${z_output_dir}" "${file_name}")
    cmake_path(COMPARE "${link_path}" NOT_EQUAL "${z_target_path}" result)
    if(result)
      message(STATUS "Create a link of '${file_name}'.")
      file(CREATE_LINK "${z_target_path}" "${link_path}" RESULT result COPY_ON_ERROR)
      if(result)
        message(FATAL_ERROR "${result}")
      endif()
    endif()
  ]])
  cmake_path(SET script_dir "${binary_dir}/Script")
  file(MAKE_DIRECTORY "${script_dir}")
  cmake_path(SET script_file "${script_dir}/${link_target}.cmake")
  file(WRITE "${script_file}" ${script})

  # Register the linking script to the given target
  add_custom_target(
      ${link_target} ALL
      ${CMAKE_COMMAND} -D z_target_path=$<TARGET_FILE:${target}> -D z_output_dir=${output_dir} -P "${script_file}"
      DEPENDS ${target}
      WORKING_DIRECTORY "${binary_dir}"
      COMMENT "Create a link to the target '${target}' into '${output_dir}'"
      SOURCE "${script_file}")
endfunction(Zisc_createLinkToTarget)


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
  Zisc_checkTarget(gtest)
  Zisc_checkTarget(gtest_main)
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
  # Set workaround of warnings
  block()
    set(has_clang $<OR:$<C_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:AppleClang>>)
    target_compile_options(gtest PRIVATE $<${has_clang}:-Wno-implicit-int-float-conversion>)
    target_compile_options(gtest_main PRIVATE $<${has_clang}:-Wno-implicit-int-float-conversion>)
  endblock()
endfunction(Zisc_addGoogleTest)


#
function(Zisc_printSystemInfo message_prefix)
  # Include dependencies
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/platform.cmake")

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
  message(STATUS "${message_prefix}C++ Compiler variant: ${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}")
  message(STATUS "${message_prefix}Target architecture : ${CMAKE_SYSTEM_PROCESSOR}")
  Zisc_getFeatureLevelList(feature_level_list)
  message(STATUS "${message_prefix}Arch feature levels : ${feature_level_list}")
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
                     COMMAND ${echo} "${prefix}C++ compiler   : $<$<BOOL:$<CXX_COMPILER_ID>>:$<CXX_COMPILER_ID>;$<CXX_COMPILER_VERSION>>"
                     COMMAND ${echo} "${prefix}Compile options: $<${prop},COMPILE_OPTIONS>"
                     COMMAND ${echo} "${prefix}Compile defs   : $<${prop},COMPILE_DEFINITIONS>"
                     COMMAND ${echo} "${prefix}Link options   : $<${prop},LINK_OPTIONS>"
                     VERBATIM
                     COMMAND_EXPAND_LISTS
                     )
endfunction(Zisc_printTargetCompileProperties)


function(Zisc_setVariablesOnCMake)
  foreach(value IN LISTS ARGV)
    set(${value} ON PARENT_SCOPE)
  endforeach(value)
endfunction(Zisc_setVariablesOnCMake)
