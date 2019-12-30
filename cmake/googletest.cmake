# file: googletest.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


function(parseGoogleTestFlags googletest_compile_flags)
  set(compile_flags "")
  foreach(flag IN LISTS ARGN)
    if(NOT flag MATCHES "^SHELL:")
      list(APPEND compile_flags ${flag})
    endif()
  endforeach(flag)


  # Output variables
  set(${googletest_compile_flags} ${compile_flags} PARENT_SCOPE)
endfunction(parseGoogleTestFlags)


# Require GoogleTest project files:
function(buildGoogleTest gtest_project_root gtest_include_dir gtest_libraries)
  include(ExternalProject)
  set(googletest_build_dir ${PROJECT_BINARY_DIR}/googletest)
  set(googletest_build_command ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel 4)

  getCxxCompilerFlags(googletest_flags googletest_linker_flgas googletest_definitions)
  parseGoogleTestFlags(googletest_flags ${googletest_flags})
  string(REPLACE ";" " " googletest_flags "${googletest_flags}")
  string(REPLACE ";" " " googletest_linker_flgas "${googletest_linker_flgas}")
  string(REPLACE ";" " " googletest_definitions "${googletest_definitions}")
  set(googletest_cmake_options "")

  set(googletest_postfix "")
  if(Z_DEBUG_MODE)
    set(googletest_postfix "d")
    list(APPEND googletest_cmake_options
        -DCMAKE_DEBUG_POSTFIX=${googletest_postfix}
        -DCMAKE_RELWITHDEBINFO_POSTFIX=${googletest_postfix})
  endif()
  if(Z_VISUAL_STUDIO)
    list(APPEND googletest_cmake_options -Dgtest_force_shared_crt=ON
        -DCMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE=${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE})
    set(googletest_flags "${googletest_flags} /w")
    set(libgtest_lib_name "gtest${googletest_postfix}.lib")
    set(libgtestmain_lib_name "gtest_main${googletest_postfix}.lib")
    set(libgmock_lib_name "gmock${googletest_postfix}.lib")
    set(libgmockmain_lib_name "gmock_main${googletest_postfix}.lib")
  elseif(Z_XCODE)
    set(googletest_toolset ${CMAKE_GENERATOR_TOOLSET})
    set(googletest_c_compiler ${CMAKE_C_COMPILER})
    set(googletest_cxx_compiler ${CMAKE_CXX_COMPILER})
    set(libgtest_lib_name "libgtest${googletest_postfix}.a")
    set(libgtestmain_lib_name "libgtest_main${googletest_postfix}.a")
    set(libgmock_lib_name "libgmock${googletest_postfix}.a")
    set(libgmockmain_lib_name "libgmock_main${googletest_postfix}.a")
  else()
    if(Z_WINDOWS)
      # For Windows mingw
      list(APPEND googletest_cmake_options -Dgtest_disable_pthreads=ON)
    endif()
    set(googletest_toolset ${CMAKE_GENERATOR_TOOLSET})
    set(googletest_c_compiler ${CMAKE_C_COMPILER})
    set(googletest_cxx_compiler ${CMAKE_CXX_COMPILER})
    set(libgtest_lib_name "libgtest${googletest_postfix}.a")
    set(libgtestmain_lib_name "libgtest_main${googletest_postfix}.a")
    set(libgmock_lib_name "libgmock${googletest_postfix}.a")
    set(libgmockmain_lib_name "libgmock_main${googletest_postfix}.a")
  endif()

  externalproject_add(GoogleTest
                      PREFIX ${googletest_build_dir}
                      SOURCE_DIR ${gtest_project_root}
                      CMAKE_GENERATOR ${CMAKE_GENERATOR}
                      CMAKE_GENERATOR_PLATFORM ${CMAKE_GENERATOR_PLATFORM}
                      CMAKE_GENERATOR_TOOLSET ${googletest_toolset}
                      CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                 -DCMAKE_INSTALL_PREFIX=${googletest_build_dir}
                                 -DCMAKE_C_COMPILER=${googletest_c_compiler}
                                 -DCMAKE_CXX_COMPILER=${googletest_cxx_compiler}
                                 -DCMAKE_C_FLAGS=${googletest_flags}
                                 -DCMAKE_CXX_FLAGS=${googletest_flags}
                                 -DCMAKE_EXE_LINKER_FLAGS=${googletest_linker_flgas}
                                 ${googletest_cmake_options}
                      BUILD_COMMAND ${googletest_build_command})
  # Set libraries
  # libgtest
  add_library(gtest STATIC IMPORTED)
  set(libgtest_path ${googletest_build_dir}/lib/${libgtest_lib_name})
  set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${libgtest_path})
  add_dependencies(gtest GoogleTest)
  # libgtest_main
  add_library(gtestmain STATIC IMPORTED)
  set(libgtestmain_path ${googletest_build_dir}/lib/${libgtestmain_lib_name})
  set_target_properties(gtestmain PROPERTIES IMPORTED_LOCATION ${libgtestmain_path})
  add_dependencies(gtestmain GoogleTest)
  # libgmock
  add_library(gmock STATIC IMPORTED)
  set(libgmock_path ${googletest_build_dir}/lib/${libgmock_lib_name})
  set_target_properties(gmock PROPERTIES IMPORTED_LOCATION ${libgmock_path})
  add_dependencies(gmock GoogleTest)
  # libgmock_main
  add_library(gmockmain STATIC IMPORTED)
  set(libgmockmain_path ${googletest_build_dir}/lib/${libgmockmain_lib_name})
  set_target_properties(gmockmain PROPERTIES IMPORTED_LOCATION ${libgmockmain_path})
  add_dependencies(gmockmain GoogleTest)


  # Output variables
  set(${gtest_include_dir} ${googletest_build_dir}/include PARENT_SCOPE)
  set(${gtest_libraries} gtest gtestmain PARENT_SCOPE)
endfunction(buildGoogleTest)
