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

#endif // ZISC_MATH_REFERENCE_TRIGONOMETRIC_TEST_HPP
