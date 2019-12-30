# file: project.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


# Retrieve Zisc version from README.md
function(retrieveZiscInfo version_major version_minor version_patch brief description)
  set(version_is_found OFF)

  # Parse README.md and find Zisc info
  set(stage 0)
  file(STRINGS ${PROJECT_SOURCE_DIR}/README.md readme_text)
  foreach(readme_line ${readme_text})
    if(stage GREATER 3)
      break()
    endif()

    if(readme_line MATCHES "Version: *([0-9]+)\\.([0-9]+)\\.([0-9]+) *")
      math(EXPR stage "1 + ${stage}")
      set(version_is_found ON)
      set(major ${CMAKE_MATCH_1})
      set(minor ${CMAKE_MATCH_2})
      set(patch ${CMAKE_MATCH_3})
      math(EXPR stage "1 + ${stage}")
    endif()

    if(stage EQUAL 0)
      math(EXPR stage "1 + ${stage}")
    elseif(stage EQUAL 1)
      string(APPEND readme_brief "${readme_line}")
      math(EXPR stage "1 + ${stage}")
    elseif(stage EQUAL 2)
      string(APPEND readme_description "${readme_line}")
    endif()
  endforeach(readme_line)
  if(NOT version_is_found)
    message(FATAL_ERROR "Zisc version isn't found in README.md.")
  endif()


  # Output variables
  set(${version_major} ${major} PARENT_SCOPE)
  set(${version_minor} ${minor} PARENT_SCOPE)
  set(${version_patch} ${patch} PARENT_SCOPE)
  set(${brief} ${readme_brief} PARENT_SCOPE)
  set(${description} ${readme_description} PARENT_SCOPE)
endfunction(retrieveZiscInfo)


# Set project informations
macro(setZiscProperties)
  # Version
  retrieveZiscInfo(PROJECT_VERSION_MAJOR
                   PROJECT_VERSION_MINOR
                   PROJECT_VERSION_PATCH
                   PROJECT_DESCRIPTION
                   description)
  set(PROJECT_VERSION 
      "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
  set(zisc_version ${PROJECT_VERSION})
  set(zisc_version_major ${PROJECT_VERSION_MAJOR})
  set(zisc_version_minor ${PROJECT_VERSION_MINOR})
  set(zisc_version_patch ${PROJECT_VERSION_PATCH})
endmacro(setZiscProperties)
