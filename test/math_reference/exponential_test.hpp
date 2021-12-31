/*!
  \file exponential_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_EXPONENTIAL_TEST_HPP
#define ZISC_MATH_REFERENCE_EXPONENTIAL_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

void testExpF(std::ostream* output) noexcept;

void testExpD(std::ostream* output) noexcept;

void testExpIntF(std::ostream* output) noexcept;

void testExpIntD(std::ostream* output) noexcept;

void testExp2F(std::ostream* output) noexcept;

void testExp2D(std::ostream* output) noexcept;

void testExp2IntF(std::ostream* output) noexcept;

void testExp2IntD(std::ostream* output) noexcept;

void testExpm1F(std::ostream* output) noexcept;

void testExpm1D(std::ostream* output) noexcept;

void testExpm1IntF(std::ostream* output) noexcept;

void testExpm1IntD(std::ostream* output) noexcept;

void testLogF(std::ostream* output) noexcept;

void testLogD(std::ostream* output) noexcept;

void testLogSubnormalF(std::ostream* output) noexcept;

void testLogSubnormalD(std::ostream* output) noexcept;

void testLog10F(std::ostream* output) noexcept;

void testLog10D(std::ostream* output) noexcept;

void testLog10SubnormalF(std::ostream* output) noexcept;

void testLog10SubnormalD(std::ostream* output) noexcept;

void testLog2F(std::ostream* output) noexcept;

void testLog2D(std::ostream* output) noexcept;

void testLog2SubnormalF(std::ostream* output) noexcept;

void testLog2SubnormalD(std::ostream* output) noexcept;

void testLog1pF(std::ostream* output) noexcept;

void testLog1pD(std::ostream* output) noexcept;

void testLog1pSubnormalF(std::ostream* output) noexcept;

void testLog1pSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_EXPONENTIAL_TEST_HPP
