# file: googletest.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2018 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


# Require GoogleTest project files:
function(buildGoogleTest gtest_project_root gtest_include_dir gtest_libraries)
  include(ExternalProject)
  set(googletest_build_dir ${PROJECT_BINARY_DIR}/googletest)
  set(googletest_build_command ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE})

  getCompilerOption(googletest_flags googletest_linker_flgas googletest_definitions)
  string(REPLACE ";" " " googletest_flags "${googletest_flags}")
  string(REPLACE ";" " " googletest_linker_flgas "${googletest_linker_flgas}")
  string(REPLACE ";" " " googletest_definitions "${googletest_definitions}")
  set(googletest_cmake_options "")

  if(Z_VISUAL_STUDIO)
    list(APPEND googletest_cmake_options -Dgtest_force_shared_crt=ON)
    if(Z_RELEASE_MODE)
      set(libgtest_lib_name "gtest.lib")
      set(libgtestmain_lib_name "gtest_main.lib")
      set(libgmock_lib_name "gmock.lib")
      set(libgmockmain_lib_name "gmock_main.lib")
    else()
      set(libgtest_lib_name "gtestd.lib")
      set(libgtestmain_lib_name "gtest_maind.lib")
      set(libgmock_lib_name "gmockd.lib")
      set(libgmockmain_lib_name "gmock_maind.lib")
    endif()
  else()
    if(Z_WINDOWS)
      # For Windows mingw
      list(APPEND googletest_cmake_options -Dgtest_disable_pthreads=ON)
    endif()
    set(googletest_c_compiler ${CMAKE_C_COMPILER})
    set(googletest_cxx_compiler ${CMAKE_CXX_COMPILER})
    set(libgtest_lib_name "libgtest.a")
    set(libgtestmain_lib_name "libgtest_main.a")
    set(libgmock_lib_name "libgmock.a")
    set(libgmockmain_lib_name "libgmock_main.a")
  endif()

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
