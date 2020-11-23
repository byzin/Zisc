/*!
  \file pow_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_POWER_TEST_HPP
#define ZISC_MATH_REFERENCE_POWER_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

template <zisc::FloatingPoint Float> inline
constexpr Float makePowInput(const Float x) noexcept
{
  constexpr Float k = zisc::cast<Float>(16.0);
  const Float e = k * x;
  return e;
}

void testPowF(std::ostream* output) noexcept;

void testPowD(std::ostream* output) noexcept;

void testPowIntF(std::ostream* output) noexcept;

void testPowIntD(std::ostream* output) noexcept;

template <zisc::FloatingPoint Float> inline
constexpr Float makeSqrtInput(const Float x) noexcept
{
  const Float y = makeNormal(x);
  return y;
}

template <zisc::FloatingPoint Float> inline
constexpr Float makeSqrtInputSub(const Float x) noexcept
{
  const Float y = makeSubnormal(x);
  return y;
}

void testSqrtF(std::ostream* output) noexcept;

void testSqrtD(std::ostream* output) noexcept;

void testSqrtSubnormalF(std::ostream* output) noexcept;

void testSqrtSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_POWER_TEST_HPP
