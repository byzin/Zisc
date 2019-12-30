# \file: document.cmake
# \author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


#
function(addDoxygenDoc target destination)
  set(options "")
  set(one_value_args HTML_HEADER
                     HTML_FOOTER
                     HTML_EXTRA_STYLESHEET
                     HTML_EXTRA_FILES)
  set(multi_value_args SOURCE_FILEDIRS
                       EXCLUDE_FILEDIRS
                       EXAMPLE_FILEDIRS)
  cmake_parse_arguments(PARSE_ARGV 2 ZDOXYGEN "${options}" "${one_value_args}" "${multi_value_args}")
  if(NOT ZDOXYGEN_SOURCE_FILEDIRS)
    message(FATAL_ERROR "No source files for doxygen.")
  endif()

  file(MAKE_DIRECTORY ${destination})

  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_OUTPUT_DIRECTORY ${destination})
  set(DOXYGEN_CREATE_SUBDIRS YES)
  set(DOXYGEN_ALLOW_UNICODE_NAMES YES)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
  set(DOXYGEN_GENERATE_TODOLIST NO)
  set(DOXYGEN_QUIET YES)
  set(DOXYGEN_WARNINGS NO)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
  if(ZDOXYGEN_EXCLUDE_FILEDIRS)
    set(DOXYGEN_EXCLUDE ${ZDOXYGEN_EXCLUDE_FILEDIRS})
  endif()
  set(DOXYGEN_EXCLUDE_SYMLINKS YES)
  if(ZDOXYGEN_EXAMPLE_FILEDIRS)
    set(DOXYGEN_EXAMPLE_PATH ${ZDOXYGEN_EXAMPLE_FILEDIRS})
    set(DOXYGEN_EXAMPLE_RECURSIVE YES)
  endif()
  foreach(source IN LISTS ZDOXYGEN_SOURCE_FILEDIRS)
    if("${source}" MATCHES ".*\.md")
      set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${source})
    endif()
  endforeach(source)
  # HTML custom
  if(ZDOXYGEN_HTML_HEADER)
    set(DOXYGEN_HTML_HEADER ${ZDOXYGEN_HTML_HEADER})
  endif()
  if(ZDOXYGEN_HTML_FOOTER)
    set(DOXYGEN_HTML_FOOTER ${ZDOXYGEN_HTML_FOOTER})
  endif()
  if(ZDOXYGEN_HTML_EXTRA_STYLESHEET)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${ZDOXYGEN_HTML_EXTRA_STYLESHEET})
  endif()
  if(ZDOXYGEN_HTML_EXTRA_FILES)
    set(DOXYGEN_HTML_EXTRA_FILES ${ZDOXYGEN_HTML_EXTRA_FILES})
  endif()
  doxygen_add_docs(${target}
    ${ZDOXYGEN_SOURCE_FILEDIRS}
    WORKING_DIRECTORY ${destination} 
    COMMENT "Building documents")

  ## Debug
  set(target_debug ${target}-debug)
  set(destination_debug ${destination}-debug)
  file(MAKE_DIRECTORY ${destination_debug})

  set(DOXYGEN_OUTPUT_DIRECTORY ${destination_debug})
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_EXTRACT_PRIV_VIRTUAL YES)
  set(DOXYGEN_EXTRACT_LOCAL_METHODS YES)
  set(DOXYGEN_SOURCE_BROWSER YES)
  set(DOXYGEN_GENERATE_TODOLIST YES)
  set(DOXYGEN_WARNINGS YES)
  set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
  doxygen_add_docs(${target_debug}
    ${ZDOXYGEN_SOURCE_FILEDIRS}
    WORKING_DIRECTORY ${destination_debug}
    COMMENT "Building documents")
endfunction(addDoxygenDoc)
