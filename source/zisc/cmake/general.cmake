# file: general.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

set(__general_path__ ${CMAKE_CURRENT_LIST_FILE})


# Set boolean value option
macro(setBooleanOption variable value doc_string)
  set(${variable} ${value} CACHE BOOL ${doc_string})
endmacro(setBooleanOption)


# Set string value option
macro(setStringOption variable value doc_string)
  set(${variable} ${value} CACHE STRING ${doc_string})
endmacro(setStringOption)


# Prohibit building on all directories except debug and release directory.
function(restrictBuildDirectory build_dir)
  if(NOT PROJECT_BINARY_DIR MATCHES "${build_dir}(/*)?")
    message(FATAL_ERROR "\"${PROJECT_BINARY_DIR}\" dir is not allowed to build.\n"
                        "Please build on \"${build_dir}\".")
  endif()
endfunction(restrictBuildDirectory)


#
function(checkSubmodule submodule_path)
  if(NOT EXISTS ${submodule_path})
    get_filename_component(submodule_name "${submodule_path}" NAME)
    message(FATAL_ERROR
            "Submodule '${submodule_name}' not found. Please init the submodule.")
  endif()
endfunction(checkSubmodule)


#
function(checkTarget target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR
            "Target '${target}' not found. Please init the target.")
  endif()
endfunction(checkTarget)


# Build GoogleTest libraries
function(addGoogleTest source_dir binary_dir)
  message(STATUS "Add GoogleTest library.")
  checkSubmodule(${source_dir})
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Prevent overriding the parent project's compiler/linker settings on Windows
  set(gtest_build_tests OFF CACHE BOOL "" FORCE)
  set(gtest_build_samples OFF CACHE BOOL "" FORCE)
  add_subdirectory(${source_dir} ${binary_dir})
endfunction(addGoogleTest)


#
function(printSystemInfo message_prefix)
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
endfunction(printSystemInfo)


function(setVariablesOnCMake)
  foreach(value IN LISTS ARGV)
    set(${value} ON PARENT_SCOPE)
  endforeach(value)
endfunction(setVariablesOnCMake)


function(getPrerequisites target_path exe_path dirs rpaths dependency_list)
  include(GetPrerequisites)
  # Check the given target path
  is_file_executable("${target_path}" is_executable)
  if(${is_executable})
    # Get dependency list
    get_prerequisites("${target_path}" deps 0 1 "${exe_path}" "${dirs}" "${rpaths}")
    set(dep_list "")
    foreach(dependency IN LISTS deps)
      gp_append_unique(dep_list ${dependency})
    endforeach(dependency)

    # Output value
    set(${dependency_list} "${dep_list}" PARENT_SCOPE)
  else()
    message(WARNING "'${target_path}' isn't executable.")
  endif()
endfunction(getPrerequisites)


function(getResolvedPrerequisites dependency_list target_path exe_path dirs rpaths resolved_dep_list)
  include(GetPrerequisites)
  # Get resolved dependency list
  set(resolved_list "")
  foreach(dependency IN LISTS dependency_list)
    gp_resolve_item("${target_path}" ${dependency} "${exe_path}" "${dirs}" dependency "${rpaths}")
    get_filename_component(dependency ${dependency} REALPATH)
    list(APPEND resolved_list ${dependency})
  endforeach(dependency)

  # Output value
  set(${resolved_dep_list} ${resolved_list} PARENT_SCOPE)
endfunction(getResolvedPrerequisites)


function(getPrerequisitesString target_path exe_path dirs rpaths dependency_list_string)
  include(GetPrerequisites)

  # Get dependency list
  getPrerequisites(${target_path} "${exe_path}" "${dirs}" "${rpaths}" dependency_list)
  getResolvedPrerequisites("${dependency_list}" ${target_path} "${exe_path}" "${dirs}" "${rpaths}" resolved_dep_list)

  #
  list(LENGTH dependency_list num_of_dependencies)
  set(text "")
  foreach(index RANGE 1 ${num_of_dependencies})
    math(EXPR index "${index} - 1")
    list(GET dependency_list ${index} dependency)
    list(GET resolved_dep_list ${index} resolved_dep)

    # Get type string
    gp_file_type(${target_path} ${resolved_dep} dependency_type)
    string(LENGTH "${dependency_type}" type_length)
    foreach(count RANGE ${type_length} 7)
      string(APPEND dependency_type " ")
    endforeach(count)

    # Get dependency string
    string(APPEND text "[${dependency_type}] ${dependency}\n"
                       "        -> ${resolved_dep}\n")
  endforeach(index)
  set(${dependency_list_string} ${text} PARENT_SCOPE)
endfunction(getPrerequisitesString)


function(savePrerequisites target output_dir exe_path dirs rpaths)
  set(prerequisite_target ${target}_prerequisite)

  # Check the output directory
  if(NOT (EXISTS "${output_dir}" AND IS_DIRECTORY "${output_dir}"))
    message(FATAL_ERROR "The directory '${output_dir}' not found.")
  endif()

  # Generate a prerequisite script
  set(script "")
  set(prerequisite_file ${output_dir}/${prerequisite_target}.txt)
  string(APPEND script
      "include(\"${__general_path__}\")\n"
      "getPrerequisitesString(\${target_path} \"${exe_path}\" \"${dirs}\" \"${rpaths}\" text)\n"
      "file(WRITE \"${prerequisite_file}\" \${text})\n"
      )

  # Save the script into a file
  set(script_file ${output_dir}/${prerequisite_target}.cmake)
  file(WRITE ${script_file} ${script})

  add_custom_target(
      ${prerequisite_target} ALL
      ${CMAKE_COMMAND} -D target_path=$<TARGET_FILE:${target}> -P ${script_file}
      DEPENDS ${target}
      WORKING_DIRECTORY "${output_dir}"
      COMMENT "Get the list of shared libraries required by '${target}'."
      SOURCES ${script_file})
endfunction(savePrerequisites)
