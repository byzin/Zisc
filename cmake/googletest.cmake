# file: googletest.txt
# author: Sho Ikeda
#
# Copyright (c) 2015-2020 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
#


function(addZiscGoogleTest)
  include(${PROJECT_SOURCE_DIR}/source/zisc/cmake/general.cmake)
  Zisc_addGoogleTest(${PROJECT_SOURCE_DIR}/test/dependencies/googletest
                     ${PROJECT_BINARY_DIR}/GoogleTest)
  Zisc_checkTarget(Zisc)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  get_target_property(zisc_compile_flags Zisc INTERFACE_COMPILE_OPTIONS)
  if(zisc_compile_flags)
    target_compile_options(gtest PRIVATE ${zisc_compile_flags})
    target_compile_options(gtest_main PRIVATE ${zisc_compile_flags})
  endif()
  get_target_property(zisc_libraries Zisc INTERFACE_LINK_LIBRARIES)
  if(zisc_libraries)
    target_link_libraries(gtest PRIVATE ${zisc_libraries})
    target_link_libraries(gtest_main PRIVATE ${zisc_libraries})
  endif()
  get_target_property(zisc_linker_flags Zisc INTERFACE_LINK_OPTIONS)
  if(zisc_linker_flags)
    target_link_options(gtest PRIVATE ${zisc_linker_flags})
    target_link_options(gtest_main PRIVATE ${zisc_linker_flags})
  endif()
endfunction(addZiscGoogleTest)
