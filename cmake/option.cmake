# file: option.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2018 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 


function(initCommandOption)
  set(option_description "Build unit tests.")
  setBooleanOption(ZISC_BUILD_TESTS OFF ${option_description})

  set(option_description "Suppress excessive warnings.")
  setBooleanOption(ZISC_SUPPRESS_EXCESSIVE_WARNING ON ${option_description})
endfunction(initCommandOption)
