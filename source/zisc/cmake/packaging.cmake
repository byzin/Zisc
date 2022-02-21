# file: packaging.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(Zisc_getSaveDependencyCode target output_dir exe_path dirs rpaths code)
  set(dependency_code "")
  string(APPEND dependency_code 
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "Zisc_saveDependencyList(${target} \"$<TARGET_FILE:${target}>\" \"${output_dir}\" \"${exe_path}\" \"${dirs}\" \"${rpaths}\")\n"
      )


  # Output variable
  set(${code} ${dependency_code} PARENT_SCOPE)
endfunction(Zisc_getSaveDependencyCode)


function(Zisc_getDeployingNonSystemDependencyCode target output_dir exe_path dirs rpaths code)
  set(dependency_code "")
  string(APPEND dependency_code
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "Zisc_deployNonSystemDependencies(${target} \"$<TARGET_FILE:${target}>\" \"\${CMAKE_INSTALL_PREFIX}/${output_dir}\" \"${exe_path}\" \"${dirs}\" \"${rpaths}\")\n"
      )


  # Output variable
  set(${code} ${dependency_code} PARENT_SCOPE)
endfunction(Zisc_getDeployingNonSystemDependencyCode)


function(Zisc_getMakeSimpleRunScriptCode target bin_dir lib_dir env_vars script_name output_dir code)
  if(Z_WINDOWS)
    set(script_ext "cmd")
    set(env_exporter "set")
    set(script_path_cmd "")
  elseif(Z_MAC)
    set(script_ext "sh")
    set(env_exporter "export")
    set(script_path_cmd "$(cd \\\"$(dirname \\\"\\\${BASH_SOURCE}\\\")\\\"; pwd)")
  elseif(Z_LINUX)
    set(script_ext "sh")
    set(env_exporter "export")
    set(script_path_cmd "$(dirname \\\"$(readlink -f \\\"\\\${BASH_SOURCE:-$_}\\\" )\\\" )")
  endif()

  set(install_code "")
  string(APPEND install_code
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_FILE}\")\n"
      "cmake_path(SET dest_dir NORMALIZE \"\${CMAKE_INSTALL_PREFIX}/${output_dir}\")\n"
      "cmake_path(SET target_path \"$<TARGET_FILE:${target}>\")\n"
      "cmake_path(GET target_path FILENAME exe_name)\n"
      "cmake_path(APPEND exe_path \"${bin_dir}\" \"\${exe_name}\")\n"
      "Zisc_makeSimpleRunScript(\"${script_name}\" \"\${exe_path}\" \"${env_vars}\" \"\${dest_dir}\" \"${script_ext}\" \"${env_exporter}\" \"${script_path_cmd}\")\n"
      )


  # Output variables
  set(${code} ${install_code} PARENT_SCOPE)
endfunction(Zisc_getMakeSimpleRunScriptCode)

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
  math(EXPR num_of_dependencies "${num_of_dependencies} - 1")
  foreach(index RANGE ${num_of_dependencies})
    # Get resolved dependency
    list(GET dependency_list ${index} dependency)
    Zisc_getResolvedDependency("${dependency}" "${target_path}" "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)

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
    Zisc_getResolvedDependency("${dependency}" ${target_path} "${exe_path}" "${dirs}" "${rpaths}" resolved_dep)
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
