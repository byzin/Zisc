# file: option.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2023 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(initProjectOptions)
  include("${PROJECT_SOURCE_DIR}/source/zisc/cmake/general.cmake")

  set(option_description "Build examples.")
  Zisc_setBooleanOption(ZISC_BUILD_EXAMPLES ON "${option_description}")

  set(option_description "Build unit tests.")
  Zisc_setBooleanOption(ZISC_BUILD_TESTS OFF "${option_description}")

  set(option_description "Suppress excessive warnings.")
  Zisc_setBooleanOption(ZISC_SUPPRESS_EXCESSIVE_WARNING ON "${option_description}")
endfunction(initProjectOptions)
