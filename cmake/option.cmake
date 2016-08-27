# file: option.cmake
# author: Sho Ikeda
#
# Copyright (c) 2015-2016 Sho Ikeda
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php
# 

function(setCommandOption)
  set(option_description "Enable include-what-you-use.")
  setBooleanOption(ZISC_ENABLE_INCLUDE_WHAT_YOU_USE OFF ${option_description})

  set(option_description "Enable C++ sanitizer (Address, Thread, Memory).")
  setBooleanOption(ZISC_ENABLE_CXX_SANITIZER OFF ${option_description})
endfunction(setCommandOption)
