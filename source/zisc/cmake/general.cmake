# file: general.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


# Set boolean value option
macro(Zisc_setBooleanOption variable value doc_string)
  set(${variable} ${value} CACHE BOOL ${doc_string})
endmacro(Zisc_setBooleanOption)


# Set string value option
macro(Zisc_setStringOption variable value doc_string)
  set(${variable} ${value} CACHE STRING ${doc_string})
endmacro(Zisc_setStringOption)


# Prohibit building on all directories except debug and release directory.
function(Zisc_restrictBuildDirectory build_dir)
  if(NOT PROJECT_BINARY_DIR MATCHES "${build_dir}(/*)?")
    message(FATAL_ERROR "\"${PROJECT_BINARY_DIR}\" dir is not allowed to build.\n"
                        "Please build on \"${build_dir}\".")
  endif()
endfunction(Zisc_restrictBuildDirectory)


#
function(Zisc_checkSubmodule submodule_path)
  if(NOT EXISTS ${submodule_path})
    get_filename_component(submodule_name "${submodule_path}" NAME)
    message(FATAL_ERROR
            "Submodule '${submodule_name}' not found. Please init the submodule.")
  endif()
endfunction(Zisc_checkSubmodule)


#
function(Zisc_checkTarget target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR
            "Target '${target}' not found. Please init the target.")
  endif()
endfunction(Zisc_checkTarget)


# Build GoogleTest libraries
function(Zisc_addGoogleTest source_dir binary_dir)
  message(STATUS "Add GoogleTest library.")
  Zisc_checkSubmodule(${source_dir})
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Prevent overriding the parent project's compiler/linker settings on Windows
  set(gtest_build_tests OFF CACHE BOOL "" FORCE)
  set(gtest_build_samples OFF CACHE BOOL "" FORCE)
  add_subdirectory(${source_dir} ${binary_dir})
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
endfunction(Zisc_printSystemInfo)


function(Zisc_setVariablesOnCMake)
  foreach(value IN LISTS ARGV)
    set(${value} ON PARENT_SCOPE)
  endforeach(value)
endfunction(Zisc_setVariablesOnCMake)


function(Zisc_getSaveDependencyCode target output_dir exe_path dirs rpaths code)
  set(dependency_code "")
  string(APPEND dependency_code 
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "Zisc_saveDependencyList(${target} \"$<TARGET_FILE:${target}>\" \"${output_dir}\" \"${exe_path}\" \"${dirs}\" \"${rpaths}\")\n"
      )


  # Output variable
  set(${code} ${dependency_code} PARENT_SCOPE)
endfunction(Zisc_getSaveDependencyCode)

## Functions used in dependency code

function(Zisc_getDependencyTreeImpl target_path exclude_system exe_path dirs rpaths level dependency_list dependency_tree)
  include(GetPrerequisites)

  set(deps "")
  get_prerequisites("${target_path}" deps ${exclude_system} 0 "${exe_path}" "${dirs}" "${rpaths}")
  set(new_list "")
  set(dep_tree "")
  math(EXPR next_level "${level} + 1")
  foreach(dependency IN LISTS deps)
    list(FIND dep_list "${dependency}" result)
    list(APPEND dep_tree "${level} ${dependency}")
    if(${result} EQUAL -1)
      list(APPEND dep_list "${dependency}")
      list(APPEND new_list "${dependency}")
      Zisc_getDependencyTreeImpl("${dependency}" ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" ${next_level} child_list child_tree)
      list(APPEND dep_list ${child_list})
      list(APPEND new_list ${child_list})
      list(APPEND dep_tree ${child_tree})
    endif()
  endforeach(dependency)

  # Output value
  set(${dependency_list} "${new_list}" PARENT_SCOPE)
  set(${dependency_tree} "${dep_tree}" PARENT_SCOPE)
endfunction(Zisc_getDependencyTreeImpl)


function(Zisc_getDependencyTree target_path exclude_system exe_path dirs rpaths dependency_tree)
  include(GetPrerequisites)

  is_file_executable("${target_path}" is_executable)
  if(is_executable)
    set(dep_list "")
    Zisc_getDependencyTreeImpl(${target_path} ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" 0 dep_list dep_tree)
  else()
    message(WARNING "'${target_path}' isn't executable.")
  endif()

  # Output value
  set(${dependency_tree} "${dep_tree}" PARENT_SCOPE)
endfunction(Zisc_getDependencyTree)


function(Zisc_getResolvedDependency dependency target_path exe_path dirs rpaths resolved_dependency)
  include(GetPrerequisites)

  # Get resolved dependency
  gp_resolve_item("${target_path}" "${dependency}" "${exe_path}" "${dirs}" resolved_dep "${rpaths}")
  get_filename_component(resolved_dep "${resolved_dep}" REALPATH)

  # Output value
  set(${resolved_dependency} "${resolved_dep}" PARENT_SCOPE)
endfunction(Zisc_getResolvedDependency)


function(Zisc_getDependenciesString target_path exclude_system is_tree_view exe_path dirs rpaths dependency_string)
  include(GetPrerequisites)

  Zisc_getDependencyTree(${target_path} ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" dependency_tree)

  #
  set(dependency_list "")
  set(level_list "")
  foreach(dependency IN LISTS dependency_tree)
    string(REGEX MATCH "([0-9]*) (.*)" result "${dependency}")
    set(level ${CMAKE_MATCH_1})
    set(dependency ${CMAKE_MATCH_2})
    if(is_tree_view)
      list(APPEND dependency_list "${dependency}")
      list(APPEND level_list ${level})
    else()
      gp_append_unique(dependency_list ${dependency})
      list(APPEND level_list 0)
    endif()
  endforeach(dependency)

  #
  set(text "")
  list(LENGTH dependency_list num_of_dependencies)
  math(EXPR num_of_dependencies "${num_of_dependencies} - 1")
  foreach(index RANGE ${num_of_dependencies})
    # Get resolved dependency
    list(GET dependency_list ${index} dependency)
    Zisc_getResolvedDependency("${dependency}" ${target_path} "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)

    # Get type string
    gp_file_type(${target_path} "${resolved_dep}" dependency_type)
    string(LENGTH "${dependency_type}" type_length)
    foreach(count RANGE ${type_length} 7)
      string(APPEND dependency_type " ")
    endforeach(count)

    # Get dependency string
    list(GET level_list ${index} level)
    string(REPEAT "  " ${level} indent)
    string(APPEND text "[${indent}${dependency_type}] ${dependency}\n"
                       "${indent}        => ${resolved_dep}\n")
  endforeach(index)


  # Output
  set(${dependency_string} ${text} PARENT_SCOPE)
endfunction(Zisc_getDependenciesString)


function(Zisc_saveDependencyList target target_path output_dir exe_path dirs rpaths)
  set(exclude_system 0)

  set(dependency_file "${output_dir}/${target}_dependency.txt")
  set(is_tree_view 0)
  Zisc_getDependenciesString(${target_path} ${exclude_system} ${is_tree_view} "${exe_path}" "${dirs}" "${rpaths}" text)
  file(WRITE ${dependency_file} ${text})

  set(dependency_tree_file "${output_dir}/${target}_dependency-tree.txt")
  set(is_tree_view 1)
  Zisc_getDependenciesString(${target_path} ${exclude_system} ${is_tree_view} "${exe_path}" "${dirs}" "${rpaths}" text)
  file(WRITE ${dependency_tree_file} ${text})
endfunction(Zisc_saveDependencyList)
