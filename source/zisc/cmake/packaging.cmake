# file: packaging.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2025 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


include_guard()


#
function(Zisc_installRunningTargetScript target bin_dir script_name)
  # Parse arguments
  set(options)
  set(one_value_args COMPONENT INSTALL_SCRIPT_DIR)
  # TODO. Support multiple PRE_COMMANDS and POST_COMMANDS
  set(multi_value_args ENV_VARIABLES PRE_COMMANDS POST_COMMANDS)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  # Check script type
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(script_type "cmd")
    set(script_comment_out "rem")
    set(script_path_cmd)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(script_type "sh")
    set(script_comment_out "#")
    set(script_path_cmd [=[ [[$(dirname "$(readlink -f "${BASH_SOURCE:-$0}" )" )]] ]=])
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(script_type "sh")
    set(script_comment_out "#")
    set(script_path_cmd [=[ [[$(cd "$(dirname "${BASH_SOURCE:-$0}")"; pwd)]] ]=])
  else()
    message(WARNING "The installation of running script isn't supported on the architecture: '${CMAKE_SYSTEM_NAME}'.")
  endif()

  # Initialize the install code. It will be exended later and installed at the end of the function
  set(code)

  # Check install path
  set(install_script_dir ".")
  if(ZISC_INSTALL_SCRIPT_DIR)
    set(install_script_dir "${ZISC_INSTALL_SCRIPT_DIR}")
  endif()
  string(APPEND code
    "set(target_file_name $<TARGET_FILE_NAME:${target}>)\n"
    "set(target_bin_dir \"${bin_dir}\")\n"
    "set(install_script_dir \"${install_script_dir}\")\n"
  [[
    cmake_path(SET target_path NORMALIZE "${target_bin_dir}/${target_file_name}")
    cmake_path(RELATIVE_PATH target_path BASE_DIRECTORY ${install_script_dir} OUTPUT_VARIABLE exe_path)
    cmake_path(NATIVE_PATH exe_path NORMALIZE zisc_exe_path)
  ]])

  #
  string(APPEND code
    "set(zisc_script_path_cmd ${script_path_cmd})\n"
  [[
    set(zisc_script_pre_commands)
    set(zisc_script_post_commands)
  ]])

  # Set the environment variables
  if(ZISC_ENV_VARIABLES)
    if(script_type STREQUAL "cmd")
      set(script_env_var_setter "set ")
    elseif(script_type STREQUAL "sh")
      set(script_env_var_setter "")
    endif()
    string(APPEND code
      "string(APPEND zisc_script_pre_commands \"${script_comment_out} Set env variables\")\n"
      "set(env_var_setter \"${script_env_var_setter}\")\n"
      "set(env_variables ${ZISC_ENV_VARIABLES})\n"
      [[
      string(APPEND zisc_script_pre_commands "\n")
      foreach(env_var IN LISTS env_variables)
        string(APPEND zisc_script_pre_commands "${env_var_setter}${env_var}\n")
      endforeach(env_var)
      ]]
    )
  endif()

  # Set the given pre commands
  if(ZISC_PRE_COMMANDS)
    string(APPEND code
      "string(APPEND zisc_script_pre_commands \"${script_comment_out} Pre process\")\n"
      "set(pre_commands ${ZISC_PRE_COMMANDS})\n"
      [[
      string(APPEND zisc_script_pre_commands "\n")
      foreach(command IN LISTS pre_commands)
        string(APPEND zisc_script_pre_commands "${command}\n")
      endforeach(command)
      ]]
    )
  endif()

  # Set the given post commands
  if(ZISC_POST_COMMANDS)
    string(APPEND code
      "string(APPEND zisc_script_post_commands \"${script_comment_out} Post process\")\n"
      "set(post_commands ${ZISC_POST_COMMANDS})\n"
      [[
      string(APPEND zisc_script_post_commands "\n")
      foreach(command IN LISTS post_commands)
        string(APPEND zisc_script_post_commands "${command}\n")
      endforeach(command)
      ]]
    )
  endif()

  # Generate the actual script from the template
  cmake_path(SET script_in_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/script")
  cmake_path(SET script_in_path "${script_in_dir}/run_template.${script_type}.in")
  cmake_path(SET script_out_dir "$<INSTALL_PREFIX>/${install_script_dir}")
  cmake_path(APPEND script_out_path "${script_out_dir}" "${script_name}.${script_type}")
  string(APPEND code
    "configure_file(\"${script_in_path}\" \"${script_out_path}\"\n"
    "               FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE\n"
    "                                OWNER_READ OWNER_WRITE OWNER_EXECUTE\n"
    "                                WORLD_READ WORLD_EXECUTE\n"
    "               @ONLY)\n"
  )

  # Install the code with the given component
  set(component)
  if(ZISC_COMPONENT)
    set(component COMPONENT ${ZISC_COMPONENT})
  endif()
  install(CODE "${code}" ${component})
endfunction(Zisc_installRunningTargetScript)


# Return the search path list to find dependencies from environment variables
# (Windows: PATH, Linux and macOS: LD_LIBRARY_PATH)
function(Zisc_getDependencySearchPathList search_path_list)
  set(path_list)

  # TODO. Windows

  # Unix
  set(ld_lib_os_list "Linux" "Darwin")
  if(CMAKE_SYSTEM_NAME IN_LIST ld_lib_os_list)
    string(REPLACE ":" ";" lib_path_list $ENV{LD_LIBRARY_PATH})
    list(APPEND path_list ${lib_path_list})
  endif()

  # Actually set to the output variable
  set(${search_path_list} ${path_list} PARENT_SCOPE)
endfunction(Zisc_getDependencySearchPathList)


#
function(Zisc_installDependencySet dependency_set)
  # Parse arguments
  set(options)
  set(one_value_args COMPONENT DESTINATION)
  set(multi_value_args)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  # Exclude system libraries
  set(pre_exclude_regexes)
  set(post_exclude_regexes)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND pre_exclude_regexes "api-ms-*."
                                    "ext-ms-*."
                                    "azure"
                                    "wpaxholder.dll"
                                    )
    list(APPEND post_exclude_regexes "^C:/WINDOWS/system32*."
                                     "^C:\\WINDOWS\\system32*."
                                     )
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND post_exclude_regexes "^/lib/*.")
  endif()
  # TODO. macOS

  # Set the output directory
  set(destination)
  if(ZISC_DESTINATION)
    list(APPEND destination RUNTIME DESTINATION "${ZISC_DESTINATION}")
    list(APPEND destination LIBRARY DESTINATION "${ZISC_DESTINATION}")
  endif()

  # Set the component
  set(component)
  if(ZISC_COMPONENT)
    set(component COMPONENT ${ZISC_COMPONENT})
  endif()

  Zisc_getDependencySearchPathList(dep_search_path_list)
  install(RUNTIME_DEPENDENCY_SET ${dependency_set}
          ${component}
          DIRECTORIES ${dep_search_path_list}
          PRE_EXCLUDE_REGEXES ${pre_exclude_regexes}
          POST_EXCLUDE_REGEXES ${post_exclude_regexes}
          ${destination}
         )
endfunction(Zisc_installDependencySet)


#
function(Zisc_saveDependencyListWhenInstallation target output_dir)
  # Include dependency
  include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/general.cmake")

  #
  Zisc_checkTarget(${target})

  # Parse arguments
  set(options)
  set(one_value_args COMPONENT)
  set(multi_value_args)
  cmake_parse_arguments(PARSE_ARGV 1 ZISC "${options}" "${one_value_args}" "${multi_value_args}")

  # Check if the given output directory is absolute
  cmake_path(IS_ABSOLUTE output_dir is_absolute)
  if(NOT is_absolute)
    message(FATAL_ERROR "The given output directory is not absolute: '${output_dir}'.")
  endif()

  # Create the saving code
  set(script)
  # Set the given parameters into the script
  string(APPEND script
    "set(target_path $<TARGET_FILE:${target}>)\n"
    "cmake_path(SET output_dir NORMALIZE \"${output_dir}\")\n"
  )

  # Check target type
  get_target_property(target_type ${target} TYPE)
  if(target_type STREQUAL "EXECUTABLE")
    string(APPEND script [[
      set(executable_target_path ${target_path})
    ]])
  elseif(target_type STREQUAL "SHARED_LIBRARY")
    string(APPEND script [[
      set(library_target_path ${target_path})
    ]])
  else()
    message(FATAL_ERROR "The '${target_type}' target '${target}' isn't supported in ${CMAKE_CURRENT_FUNCTION}")
  endif()

  # Set the search pathes of the dependencies
  Zisc_getDependencySearchPathList(dep_search_path_list)
  string(APPEND script
    "set(dep_search_path_list ${dep_search_path_list})\n"
  )
  # Actual the script of saving the dependency list
  string(APPEND script [[
    file(GET_RUNTIME_DEPENDENCIES
      RESOLVED_DEPENDENCIES_VAR resolved_deps
      UNRESOLVED_DEPENDENCIES_VAR unresolved_deps
      CONFLICTING_DEPENDENCIES_PREFIX conflicting_deps
      EXECUTABLES ${executable_target_path}
      LIBRARIES ${library_target_path}
      DIRECTORIES ${dep_search_path_list}
      )
    # Create the result text
    set(result)
    string(APPEND result
      "Dependency list\n"
      "===============\n"
      "\n"
    )

    # Resolved dependencies
    string(APPEND result
      "Resolved dependencies\n"
      "---------------\n")
    list(SORT resolved_deps)
    foreach(dep IN LISTS resolved_deps)
      cmake_path(GET dep FILENAME lib_name)
      string(APPEND result
        "* [${lib_name}](${dep})\n"
      )
    endforeach(dep)
    string(APPEND result
      "\n"
    )

    # Unresolved dependencies
    string(APPEND result
      "Unresolved dependencies\n"
      "---------------\n")
    list(SORT unresolved_deps)
    foreach(dep IN LISTS unresolved_deps)
      string(APPEND result
        "* [${dep}]\n"
      )
    endforeach(dep)
    string(APPEND result
      "\n"
    )

    # Conflict dependencies
    string(APPEND result
      "Conflict dependencies\n"
      "---------------\n")
    list(SORT conflicting_deps)
    foreach(dep IN LISTS conflicting_deps)
      string(APPEND result
        "* [${dep}]\n"
      )
    endforeach(dep)

    # Save the result
    file(WRITE "${output_dir}/dependency_list.md" ${result})
  ]])

  #
  set(component)
  if(ZISC_COMPONENT)
    set(component COMPONENT ${ZISC_COMPONENT})
  endif()
  install(CODE "${script}" ${component})
endfunction(Zisc_saveDependencyListWhenInstallation)
