/*!
  \file googletest.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2025 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_GOOGLE_TEST_HPP
#define ZISC_GOOGLE_TEST_HPP

#if defined(Z_COMPILER_GNU) || defined(Z_COMPILER_CLANG) || defined(Z_COMPILER_APPLE_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#endif // Z_COMPILER_GNU || Z_COMPILER_CLANG || Z_COMPILER_APPLE_CLANG
#if defined(Z_COMPILER_CLANG) || defined(Z_COMPILER_APPLE_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weverything"
#endif // Z_COMPILER_CLANG || Z_COMPILER_APPLE_CLANG

#include "gtest/gtest.h"

#if defined(Z_COMPILER_CLANG) || defined(Z_COMPILER_APPLE_CLANG)
#pragma GCC diagnostic pop
#endif // Z_COMPILER_CLANG || Z_COMPILER_APPLE_CLANG
#if defined(Z_COMPILER_GNU) || defined(Z_COMPILER_CLANG) || defined(Z_COMPILER_APPLE_CLANG)
#pragma GCC diagnostic pop
#endif // Z_COMPILER_GNU || Z_COMPILER_CLANG || Z_COMPILER_APPLE_CLANG

#endif // ZISC_GOOGLE_TEST_HPP
