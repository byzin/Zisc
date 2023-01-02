# file: packaging_internal.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(Zisc_getResolvedDependency context dependency exe_path dirs rpaths resolved_dependency)
  include(GetPrerequisites)

  cmake_path(SET dep "${dependency}")
  cmake_path(IS_ABSOLUTE dep is_absolute_path)
  if(NOT is_absolute_path)
    # Get resolved dependency
    gp_resolve_item("${context}" "${dependency}" "${exe_path}" "${dirs}" dep "${rpaths}")
    cmake_path(SET dep "${dep}")
  endif()
  cmake_path(IS_ABSOLUTE dep is_absolute_path)
  if(is_absolute_path)
    # Resolve symlink
    file(REAL_PATH "${dep}" dep EXPAND_TILDE)
    #
    cmake_path(SET resolved_dep NORMALIZE "${dep}")
  else()
    message("--   Warning: ${dependency} not found.")
  endif()

  # Output value
  set(${resolved_dependency} "${resolved_dep}" PARENT_SCOPE)
endfunction(Zisc_getResolvedDependency)


function(Zisc_getDependencyTreeImpl context target_path exclude_system exe_path dirs rpaths level dependency_list dependency_tree)
  include(GetPrerequisites)

  set(exclusion_list KERNEL32.dll
                     )

  set(deps "")
  set(recurse 0) # 0: Only direct prerequisites are listed
  Zisc_getResolvedDependency("${context}" "${target_path}" "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)
  cmake_path(GET resolved_dep FILENAME dep_file_name)

  set(new_list "")
  set(dep_tree "")
  cmake_path(IS_ABSOLUTE resolved_dep is_absolute_path)
  list(FIND exclusion_list "${dep_file_name}" exclusion_result)
  if(is_absolute_path AND (exclusion_result EQUAL -1))
    cmake_path(NATIVE_PATH resolved_dep NORMALIZE resolved_target)
    get_prerequisites("${resolved_target}" deps ${exclude_system} ${recurse} "${exe_path}" "${dirs}" "${rpaths}")
    math(EXPR next_level "${level} + 1")
    foreach(dependency IN LISTS deps)
      list(FIND dep_list "${dependency}" result)
      list(APPEND dep_tree "${level} ${dependency}")
      if(${result} EQUAL -1)
        list(APPEND dep_list "${dependency}")
        list(APPEND new_list "${dependency}")
        Zisc_getDependencyTreeImpl("${context}" "${dependency}" ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" ${next_level} child_list child_tree)
        list(APPEND dep_list ${child_list})
        list(APPEND new_list ${child_list})
        list(APPEND dep_tree ${child_tree})
      endif()
    endforeach(dependency)
  endif()

  # Output value
  set(${dependency_list} "${new_list}" PARENT_SCOPE)
  set(${dependency_tree} "${dep_tree}" PARENT_SCOPE)
endfunction(Zisc_getDependencyTreeImpl)


function(Zisc_getDependencyTree target_path exclude_system exe_path dirs rpaths dependency_tree)
  include(GetPrerequisites)

  set(dep_list "")
  set(level 0)
  Zisc_getDependencyTreeImpl("${target_path}" "${target_path}" ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" ${level} dep_list dep_tree)

  # Output value
  set(${dependency_tree} "${dep_tree}" PARENT_SCOPE)
endfunction(Zisc_getDependencyTree)


function(Zisc_getDependenciesString target_path exclude_system is_tree_view exe_path dirs rpaths dependency_string)
  include(GetPrerequisites)

  Zisc_getDependencyTree("${target_path}" ${exclude_system} "${exe_path}" "${dirs}" "${rpaths}" dependency_tree)

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
      gp_append_unique(dependency_list "${dependency}")
      list(APPEND level_list 0)
    endif()
  endforeach(dependency)

  #
  set(text "")
  list(LENGTH dependency_list num_of_dependencies)
  if(NOT num_of_dependencies)
    return()
  endif()

  math(EXPR num_of_dependencies "${num_of_dependencies} - 1")
  foreach(index RANGE ${num_of_dependencies})
    # Get resolved dependency
    list(GET dependency_list ${index} dependency)
    Zisc_getResolvedDependency("${target_path}" "${dependency}" "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)
    cmake_path(NATIVE_PATH resolved_dep NORMALIZE resolved_dep)

    # Get type string
    gp_file_type("${target_path}" "${resolved_dep}" dependency_type)
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

  cmake_path(APPEND dependency_file "${output_dir}" "${target}_dependency.txt")
  set(is_tree_view 0)
  Zisc_getDependenciesString("${target_path}" ${exclude_system} ${is_tree_view} "${exe_path}" "${dirs}" "${rpaths}" text)
  file(WRITE "${dependency_file}" ${text})

  cmake_path(APPEND dep_tree_file "${output_dir}" "${target}_dependency-tree.txt")
  set(is_tree_view 1)
  Zisc_getDependenciesString("${target_path}" ${exclude_system} ${is_tree_view} "${exe_path}" "${dirs}" "${rpaths}" text)
  file(WRITE "${dep_tree_file}" ${text})
endfunction(Zisc_saveDependencyList)


function(Zisc_deployNonSystemDependencies target target_path output_dir exe_path dirs rpaths)
  message("-- Deploy non-system dependencies. target: '${target}'")

  include(GetPrerequisites)

  Zisc_getDependencyTree("${target_path}" 1 "${exe_path}" "${dirs}" "${rpaths}" dependency_tree)

  # Get unique dependency list
  set(dependency_list "")
  foreach(dependency IN LISTS dependency_tree)
    string(REGEX MATCH "([0-9]*) (.*)" result "${dependency}")
    set(dependency ${CMAKE_MATCH_2})
    gp_append_unique(dependency_list ${dependency})
  endforeach(dependency)

  # Create destination directory
  if(dependency_list)
    file(MAKE_DIRECTORY "${output_dir}")
  endif()

  # Copy dependencies
  foreach(dependency IN LISTS dependency_list)
    message("--   Install non-system dependency: '${dependency}'.")
    cmake_path(GET dependency FILENAME dep_name)
    Zisc_getResolvedDependency("${target_path}" "${dependency}" "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)
    cmake_path(APPEND dest_dep_path "${output_dir}" "${dep_name}")
    file(COPY_FILE "${resolved_dep}" "${dest_dep_path}"
         RESULT copy_result
         ONLY_IF_DIFFERENT)
    if(copy_result)
      message("--   Warning: ${copy_result}")
    endif()
  endforeach(dependency)
endfunction(Zisc_deployNonSystemDependencies)


function(Zisc_makeSimpleRunScript script_name exe_path env_vars output_dir script_ext env_exporter script_path_cmd)

  # Make defining environment variables code
  set(env_definitions "")
  foreach(env IN LISTS env_vars)
    string(APPEND env_definitions "${env_exporter} ${env}\n")
  endforeach(env)

  # Set script variables
  set(zisc_script_file_name "${script_name}.${script_ext}")
  set(zisc_script_path_cmd "${script_path_cmd}")
  cmake_path(NATIVE_PATH exe_path NORMALIZE zisc_script_exec)
  set(zisc_script_pre_commands "")
  string(APPEND zisc_script_pre_commands "${env_definitions}")
  set(zisc_script_post_commands "")

  # Set script path
  cmake_path(SET in_script_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/script")
  cmake_path(SET in_script_path "${in_script_dir}/run_template.${script_ext}.in")
  cmake_path(APPEND out_script_path "${output_dir}" "${zisc_script_file_name}")

  #
  configure_file("${in_script_path}" "${out_script_path}"
                 FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                  GROUP_READ GROUP_EXECUTE
                                  WORLD_READ WORLD_EXECUTE
                 @ONLY)
endfunction(Zisc_makeSimpleRunScript)
