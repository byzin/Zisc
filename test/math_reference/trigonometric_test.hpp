/*!
  \file trigonometric_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_TRIGONOMETRIC_TEST_HPP
#define ZISC_MATH_REFERENCE_TRIGONOMETRIC_TEST_HPP

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

template <zisc::FloatingPoint Float> inline
constexpr F2<Float> makeAtan2Input(const Float x) noexcept
{
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  using BitT = typename Binary::BitType;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;

  constexpr Float t = zisc::cast<Float>(2.0);
  constexpr Float o = zisc::cast<Float>(1.0);

//  const Float x1 = makeNormal(x);
  const Float x1 = x;

  const BitT u2 = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
//  const Float x2 = makeNormal(t * zisc::mapTo01<Float>(u2) - o);
  const Float x2 = t * zisc::mapTo01<Float>(u2) - o;

  return F2<Float>{x1, x2};
}

void testSinF(std::ostream* output) noexcept;

void testSinD(std::ostream* output) noexcept;

void testSinSubnormalF(std::ostream* output) noexcept;

void testSinSubnormalD(std::ostream* output) noexcept;

void testCosF(std::ostream* output) noexcept;

void testCosD(std::ostream* output) noexcept;

void testCosSubnormalF(std::ostream* output) noexcept;

void testCosSubnormalD(std::ostream* output) noexcept;

void testTanF(std::ostream* output) noexcept;

void testTanD(std::ostream* output) noexcept;

void testTanSubnormalF(std::ostream* output) noexcept;

void testTanSubnormalD(std::ostream* output) noexcept;

void testAsinF(std::ostream* output) noexcept;

void testAsinD(std::ostream* output) noexcept;

void testAsinSubnormalF(std::ostream* output) noexcept;

void testAsinSubnormalD(std::ostream* output) noexcept;

void testAcosF(std::ostream* output) noexcept;

void testAcosD(std::ostream* output) noexcept;

void testAcosSubnormalF(std::ostream* output) noexcept;

void testAcosSubnormalD(std::ostream* output) noexcept;

void testAtanF(std::ostream* output) noexcept;

void testAtanD(std::ostream* output) noexcept;

void testAtanSubnormalF(std::ostream* output) noexcept;

void testAtanSubnormalD(std::ostream* output) noexcept;

void testAtan2F(std::ostream* output) noexcept;

void testAtan2D(std::ostream* output) noexcept;

void testSinhF(std::ostream* output) noexcept;

void testSinhD(std::ostream* output) noexcept;

void testSinhSubnormalF(std::ostream* output) noexcept;

void testSinhSubnormalD(std::ostream* output) noexcept;

void testCoshF(std::ostream* output) noexcept;

void testCoshD(std::ostream* output) noexcept;

void testCoshSubnormalF(std::ostream* output) noexcept;

void testCoshSubnormalD(std::ostream* output) noexcept;

void testTanhF(std::ostream* output) noexcept;

void testTanhD(std::ostream* output) noexcept;

void testTanhSubnormalF(std::ostream* output) noexcept;

void testTanhSubnormalD(std::ostream* output) noexcept;

void testAsinhF(std::ostream* output) noexcept;

void testAsinhD(std::ostream* output) noexcept;

void testAsinhSubnormalF(std::ostream* output) noexcept;

void testAsinhSubnormalD(std::ostream* output) noexcept;

void testAcoshF(std::ostream* output) noexcept;

void testAcoshD(std::ostream* output) noexcept;

void testAcoshSubnormalF(std::ostream* output) noexcept;

void testAcoshSubnormalD(std::ostream* output) noexcept;

void testAtanhF(std::ostream* output) noexcept;

void testAtanhD(std::ostream* output) noexcept;

void testAtanhSubnormalF(std::ostream* output) noexcept;

void testAtanhSubnormalD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_TRIGONOMETRIC_TEST_HPP
