# file: option.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2017 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(setCommandOption)

  set(option_description "Build unit tests.")
  setBooleanOption(ZISC_BUILD_TESTS OFF ${option_description})

endfunction(setCommandOption)
