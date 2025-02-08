/*!
  \file test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2025 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/export.hpp"
#include "zisc/zisc_config.hpp"

/*!
  \details No detailed description
  */
int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

// OS test
#if defined(_WIN64)
  #if !defined(Z_SYSTEM_WINDOWS)
  static_assert(false, "The macro 'Z_WINDOW' isn't defined.");
  #endif // Z_SYSTEM_WINDOWS
#elif defined(__APPLE__)
  #if !defined(Z_SYSTEM_DARWIN)
  static_assert(false, "The macro 'Z_SYSTEM_DARWIN' isn't defined.");
  #endif // Z_SYSTEM_DARWIN
#elif defined(__linux__)
  #if !defined(Z_SYSTEM_LINUX)
  static_assert(false, "The macro 'Z_SYSTEM_LINUX' isn't defined.");
  #endif // Z_SYSTEM_LINUX
#else
static_assert(false, "Zisc is built on unsupported OS.");
#endif

// Compiler test
#if defined(__clang__)
  #if !defined(Z_COMPILER_CLANG)
  static_assert(false, "The macro 'Z_COMPILER_CLANG' isn't defined.");
  #endif // Z_COMPILER_CLANG
#elif defined(__GNUC__)
  #if !defined(Z_COMPILER_GNU)
  static_assert(false, "The macro 'Z_COMPILER_GNU' isn't defined.");
  #endif // Z_COMPILER_GNU
#elif defined(_MSC_VER)
  #if !defined(Z_COMPILER_MSVC)
  static_assert(false, "The macro 'Z_COMPILER_MSVC' isn't defined.");
  #endif // Z_COMPILER_MSVC
#else
static_assert(false, "Zisc is built with unsupported compiler.");
#endif

// Export test

Z_EXPORT_C_FUNCTION
void exportCTest([[maybe_unused]] const int value)
{
}

Z_EXPORT_CXX_FUNCTION
void exportCxxTest([[maybe_unused]] const int value)
{
}
