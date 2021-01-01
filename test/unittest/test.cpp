/*!
  \file test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/zisc_config.hpp"

/*!
  \details
  No detailed.
  */
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

// OS test
#if defined(_WIN64)
  #if !defined(Z_WINDOWS)
  static_assert(false, "The macro 'Z_WINDOW' isn't defined.");
  #endif // Z_WINDOWS
#elif defined(__APPLE__)
  #if !defined(Z_MAC)
  static_assert(false, "The macro 'Z_MAC' isn't defined.");
  #endif // Z_MAC
#elif defined(__linux__)
  #if !defined(Z_LINUX)
  static_assert(false, "The macro 'Z_LINUX' isn't defined.");
  #endif // Z_LINUX
#else
static_assert(false, "Zisc is built on unsupported OS.");
#endif

// Compiler test
#if defined(__clang__)
  #if !defined(Z_CLANG)
  static_assert(false, "The macro 'Z_CLANG' isn't defined.");
  #endif // Z_CLANG
#elif defined(__GNUC__)
  #if !defined(Z_GCC)
  static_assert(false, "The macro 'Z_GCC' isn't defined.");
  #endif // Z_GCC
#elif defined(_MSC_VER)
  #if !defined(Z_MSVC)
  static_assert(false, "The macro 'Z_MSVC' isn't defined.");
  #endif // Z_MSVC
#else
static_assert(false, "Zisc is built with unsupported compiler.");
#endif
