/*!
  \file float_manipulation_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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

template <zisc::FloatingPoint Float> inline
constexpr Float makeFrLdexpInput(const Float x) noexcept
{
  const Float y = makeNormal(x);
  return y;
}

template <zisc::FloatingPoint Float> inline
constexpr Float makeFrLdexpSubnormalInput(const Float x) noexcept
{
  const Float y = makeSubnormal(x);
  return y;
}

void testFrLdexpF(std::ostream* output) noexcept;

void testFrLdexpD(std::ostream* output) noexcept;

void testFrLdexpSubnormalF(std::ostream* output) noexcept;

void testFrLdexpSubnormalD(std::ostream* output) noexcept;

template <zisc::FloatingPoint Float> inline
constexpr Float makeIlogbInput(const Float x) noexcept
{
  const Float y = makeNormal(x);
  return y;
}

template <zisc::FloatingPoint Float> inline
constexpr Float makeIlogbSubnormalInput(const Float x) noexcept
{
  const Float y = makeSubnormal(x);
  return y;
}

void testIlogbF(std::ostream* output) noexcept;

void testIlogbD(std::ostream* output) noexcept;

void testIlogbSubnormalF(std::ostream* output) noexcept;

void testIlogbSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_FLOAT_MANIPULATION_TEST_HPP
