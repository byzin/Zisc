/*!
  \file error_function_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_ERROR_FUNCTION_TEST_HPP
#define ZISC_MATH_REFERENCE_ERROR_FUNCTION_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

void testErfF(std::ostream* output) noexcept;

void testErfD(std::ostream* output) noexcept;

void testErfSubnormalF(std::ostream* output) noexcept;

void testErfSubnormalD(std::ostream* output) noexcept;

void testErfcF(std::ostream* output) noexcept;

void testErfcD(std::ostream* output) noexcept;

void testErfcSubnormalF(std::ostream* output) noexcept;

void testErfcSubnormalD(std::ostream* output) noexcept;

void testTgammaF(std::ostream* output) noexcept;

void testTgammaD(std::ostream* output) noexcept;

void testTgammaSubnormalF(std::ostream* output) noexcept;

void testTgammaSubnormalD(std::ostream* output) noexcept;

void testLgammaF(std::ostream* output) noexcept;

void testLgammaD(std::ostream* output) noexcept;

void testLgammaSubnormalF(std::ostream* output) noexcept;

void testLgammaSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_ERROR_FUNCTION_TEST_HPP
