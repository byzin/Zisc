/*!
  \file float_manipulation_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_FLOAT_MANIPULATION_TEST_HPP
#define ZISC_MATH_REFERENCE_FLOAT_MANIPULATION_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

void testFrLdexpF(std::ostream* output) noexcept;

void testFrLdexpD(std::ostream* output) noexcept;

void testFrLdexpSubnormalF(std::ostream* output) noexcept;

void testFrLdexpSubnormalD(std::ostream* output) noexcept;

void testIlogbF(std::ostream* output) noexcept;

void testIlogbD(std::ostream* output) noexcept;

void testIlogbSubnormalF(std::ostream* output) noexcept;

void testIlogbSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_FLOAT_MANIPULATION_TEST_HPP
