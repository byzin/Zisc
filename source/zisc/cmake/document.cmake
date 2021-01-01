# \file: document.cmake
# \author: Sho Ikeda
#
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(Zisc_addDoxygenDoc target destination)
  set(options "")
  set(one_value_args "")
  set(multi_value_args SOURCE_FILES_OR_DIRS)
  cmake_parse_arguments(PARSE_ARGV 2 ZDOXYGEN "${options}" "${one_value_args}" "${multi_value_args}")
  if(NOT ZDOXYGEN_SOURCE_FILES_OR_DIRS)
    message(FATAL_ERROR "Source files or directories not found.")
  endif()

  # Build a target document
  set(dest_dir ${destination}/${target})
  file(MAKE_DIRECTORY ${dest_dir})

  set(DOXYGEN_OUTPUT_DIRECTORY ${dest_dir})
  set(DOXYGEN_CREATE_SUBDIRS YES)
  set(DOXYGEN_ALLOW_UNICODE_NAMES YES)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
  set(DOXYGEN_GENERATE_TODOLIST NO)
  set(DOXYGEN_QUIET YES)
  set(DOXYGEN_WARNINGS NO)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_EXCLUDE_SYMLINKS YES)

  foreach(source IN LISTS ZDOXYGEN_SOURCE_FILES_OR_DIRS)
    if("${source}" MATCHES ".*\.md")
      set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${source})
    endif()
  endforeach(source)
  doxygen_add_docs(${target}
    ${ZDOXYGEN_SOURCE_FILES_OR_DIRS}
    WORKING_DIRECTORY ${dest_dir} 
    COMMENT "Building document '${target}'")

  # Build a target document for debug
  set(target_debug ${target}-debug)
  set(dest_dir ${destination}/${target_debug})
  file(MAKE_DIRECTORY ${dest_dir})

  set(DOXYGEN_OUTPUT_DIRECTORY ${dest_dir})
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
  set(DOXYGEN_EXTRACT_LOCAL_METHODS YES)
  set(DOXYGEN_WARNINGS YES)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
  set(DOXYGEN_SOURCE_BROWSER YES)
  set(DOXYGEN_GENERATE_TODOLIST YES)
  doxygen_add_docs(${target_debug}
    ${ZDOXYGEN_SOURCE_FILES_OR_DIRS}
    WORKING_DIRECTORY ${dest_dir}
    COMMENT "Building document '${target_debug}'")
endfunction(Zisc_addDoxygenDoc)
