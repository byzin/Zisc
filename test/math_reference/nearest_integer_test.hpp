/*!
  \file nearest_integer_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_NEAREST_INTEGER_TEST_HPP
#define ZISC_MATH_REFERENCE_NEAREST_INTEGER_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

void testRintF(std::ostream* output) noexcept;

void testRintD(std::ostream* output) noexcept;

void testRintSubnormalF(std::ostream* output) noexcept;

void testRintSubnormalD(std::ostream* output) noexcept;

void testCeilF(std::ostream* output) noexcept;

void testCeilD(std::ostream* output) noexcept;

void testCeilSubnormalF(std::ostream* output) noexcept;

void testCeilSubnormalD(std::ostream* output) noexcept;

void testFloorF(std::ostream* output) noexcept;

void testFloorD(std::ostream* output) noexcept;

void testFloorSubnormalF(std::ostream* output) noexcept;

void testFloorSubnormalD(std::ostream* output) noexcept;

void testTruncF(std::ostream* output) noexcept;

void testTruncD(std::ostream* output) noexcept;

void testTruncSubnormalF(std::ostream* output) noexcept;

void testTruncSubnormalD(std::ostream* output) noexcept;

void testRoundF(std::ostream* output) noexcept;

void testRoundD(std::ostream* output) noexcept;

void testRoundSubnormalF(std::ostream* output) noexcept;

void testRoundSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_NEAREST_INTEGER_TEST_HPP
