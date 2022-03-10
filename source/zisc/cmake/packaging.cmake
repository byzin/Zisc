# file: packaging.cmake
# author: Sho Ikeda
# 
# Copyright (c) 2015-2022 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

#
function(Zisc_getSaveDependencyCode target output_dir exe_path dirs rpaths code)
  set(dependency_code "")
  string(APPEND dependency_code 
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/packaging_internal.cmake\")\n"
      "Zisc_saveDependencyList(${target} \"$<TARGET_FILE:${target}>\" \"${output_dir}\" \"${exe_path}\" \"${dirs}\" \"${rpaths}\")\n"
      )


  # Output variable
  set(${code} ${dependency_code} PARENT_SCOPE)
endfunction(Zisc_getSaveDependencyCode)


#
function(Zisc_getDeployingNonSystemDependencyCode target output_dir exe_path dirs rpaths code)
  set(dependency_code "")
  string(APPEND dependency_code
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/packaging_internal.cmake\")\n"
      "Zisc_deployNonSystemDependencies(${target} \"$<TARGET_FILE:${target}>\" \"\${CMAKE_INSTALL_PREFIX}/${output_dir}\" \"${exe_path}\" \"${dirs}\" \"${rpaths}\")\n"
      )


  # Output variable
  set(${code} ${dependency_code} PARENT_SCOPE)
endfunction(Zisc_getDeployingNonSystemDependencyCode)


#
function(Zisc_getMakeSimpleRunScriptCode target bin_dir lib_dir env_vars script_name output_dir code)
  if(Z_WINDOWS)
    set(script_ext "cmd")
    set(env_exporter "set")
    set(script_path_cmd "")
  elseif(Z_MAC)
    set(script_ext "sh")
    set(env_exporter "export")
    set(script_path_cmd "$(cd \\\"$(dirname \\\"\\\${BASH_SOURCE}\\\")\\\"$<SEMICOLON> pwd)")
  elseif(Z_LINUX)
    set(script_ext "sh")
    set(env_exporter "export")
    set(script_path_cmd "$(dirname \\\"$(readlink -f \\\"\\\${BASH_SOURCE:-$_}\\\" )\\\" )")
  endif()

  set(install_code "")
  string(APPEND install_code
      "include(\"${CMAKE_CURRENT_FUNCTION_LIST_DIR}/packaging_internal.cmake\")\n"
      "cmake_path(SET dest_dir NORMALIZE \"\${CMAKE_INSTALL_PREFIX}/${output_dir}\")\n"
      "cmake_path(SET target_path \"$<TARGET_FILE:${target}>\")\n"
      "cmake_path(GET target_path FILENAME exe_name)\n"
      "cmake_path(APPEND exe_path \"${bin_dir}\" \"\${exe_name}\")\n"
      "set(env_vars \"\")\n"
      )
  # Add environment variables
  foreach(env_var IN LISTS env_vars)
    string(APPEND install_code "list(APPEND env_vars \"${env_var}\")\n")
  endforeach(env_var)
  string(APPEND install_code
      "Zisc_makeSimpleRunScript(\"${script_name}\" \"\${exe_path}\" \"\${env_vars}\" \"\${dest_dir}\" \"${script_ext}\" \"${env_exporter}\" \"${script_path_cmd}\")\n"
      )


  # Output variables
  set(${code} ${install_code} PARENT_SCOPE)
endfunction(Zisc_getMakeSimpleRunScriptCode)
