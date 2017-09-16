# file: googletest.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2017 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


# Require GoogleTest project files:
function(buildGoogleTest gtest_project_root gtest_include_dir gtest_libraries)
  include(ExternalProject)
  set(googletest_build_dir ${PROJECT_BINARY_DIR}/googletest)
  set(googletest_build_command ${CMAKE_COMMAND} --build .)
  if(Z_VISUAL_STUDIO)
    if(Z_DEBUG_MODE)
      list(APPEND googletest_build_command --config Debug)
    else()
      list(APPEND googletest_build_command --config Release)
    endif()
    set(googletest_cmake_options -Dgtest_force_shared_crt=ON)
    set(libgtest_file_name "gtest.lib")
    set(libgtestmain_file_name "gtest_main.lib")
  else()
    if(Z_WINDOWS)
      # For Windows mingw
      set(googletest_cmake_options -Dgtest_disable_pthreads=ON)
    endif()
    set(googletest_c_compiler ${CMAKE_C_COMPILER})
    set(googletest_cxx_compiler ${CMAKE_CXX_COMPILER})
    set(libgtest_file_name "libgtest.a")
    set(libgtestmain_file_name "libgtest_main.a")
  endif()
  getCompilerOption(googletest_flags googletest_linker_flgas googletest_definitions)
  string(REPLACE ";" " " googletest_flags "${googletest_flags}")
  string(REPLACE ";" " " googletest_linker_flgas "${googletest_linker_flgas}")
  string(REPLACE ";" " " googletest_definitions "${googletest_definitions}")
  externalproject_add(GoogleTest
                      PREFIX ${googletest_build_dir}
                      SOURCE_DIR ${gtest_project_root}
                      CMAKE_GENERATOR ${CMAKE_GENERATOR}
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
  set(libgtest_path ${googletest_build_dir}/lib/${libgtest_file_name})
  set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${libgtest_path})
  add_dependencies(gtest GoogleTest)
  # libgtest_main
  add_library(gtestmain STATIC IMPORTED)
  set(libgtestmain_path ${googletest_build_dir}/lib/${libgtestmain_file_name})
  set_target_properties(gtestmain PROPERTIES IMPORTED_LOCATION ${libgtestmain_path})
  add_dependencies(gtestmain GoogleTest)


  # Output variables
  set(${gtest_include_dir} ${googletest_build_dir}/include PARENT_SCOPE)
  set(${gtest_libraries} gtest gtestmain PARENT_SCOPE)
endfunction(buildGoogleTest)
