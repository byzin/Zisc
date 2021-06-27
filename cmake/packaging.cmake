# file: packaging.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2021 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(packageProject)
  include(InstallRequiredSystemLibraries)

  set(package_directory ${PROJECT_BINARY_DIR}/Packaging)
  file(MAKE_DIRECTORY ${package_directory})
  set(CPACK_PACKAGE_DIRECTORY ${package_directory})

  # Set project info
  set(CPACK_PACKAGE_NAME "Zisc")
  set(CPACK_PACKAGE_VENDOR "Sho Ikeda")
  #  set(CPACK_PACKAGE_VERSION_MAJOR ${Zisc_VERSION_MAJOR})
  #  set(CPACK_PACKAGE_VERSION_MINOR ${Zisc_VERSION_MINOR})
  #  set(CPACK_PACKAGE_VERSION_PATCH ${Zisc_VERSION_PATCH})
  #  set(CPACK_PACKAGE_DESCRIPTION ${Zisc_DESCRIPTION})
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ utility library")
  set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.md)
  set(CPACK_THREADS 0)

  include(CPack)
endfunction(packageProject)
