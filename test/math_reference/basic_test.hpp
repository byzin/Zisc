/*!
  \file basic_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_BASIC_TEST_HPP
#define ZISC_MATH_REFERENCE_BASIC_TEST_HPP

// Standard C++ library
#include <array>
#include <limits>
#include <ostream>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"
#include "zisc/math/math.hpp"

template <zisc::FloatingPoint Float> inline
constexpr Float makeNormal(const Float x) noexcept
{
  using zisc::cast;
  // Sign
  const Float sign = zisc::signbit(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  // Exponent
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  const Float k = cast<Float>(2.0) * zisc::abs(x) - cast<Float>(1.0);
  constexpr int bias = cast<int>(Binary::exponentBias());
  const int expt = zisc::clamp(cast<int>(k * cast<Float>(bias)), -bias + 1, bias);
  // Significand
  using BitT = typename Binary::BitType;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;
  const BitT h = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float s = zisc::mapTo01<Float>(h);
  // Make a normal float
  using Limits = std::numeric_limits<Float>;
  Float y = s * zisc::pow(cast<Float>(2), expt);
  y = sign * zisc::clamp(y, (Limits::min)(), (Limits::max)());
  return y;
}

/*!
  */
template <zisc::FloatingPoint Float> inline
constexpr Float makeSubnormal(const Float x) noexcept
{
  using zisc::cast;
  using Limits = std::numeric_limits<Float>;
  constexpr Float subnormal_max = (Limits::min)() - Limits::denorm_min();
  // Sign
  const Float sign = zisc::signbit(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  // Significand
  Float subnormal = x * subnormal_max;
  subnormal = sign * zisc::clamp(subnormal, Limits::denorm_min(), subnormal_max);
  return subnormal;
}

template <zisc::FloatingPoint Float> inline
constexpr std::array<Float, 3> makeFmaInput(const Float x) noexcept
{
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  using BitT = typename Binary::BitType;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;

  constexpr Float t = zisc::cast<Float>(2.0);
  constexpr Float o = zisc::cast<Float>(1.0);
  constexpr Float k = zisc::cast<Float>(0.85);

  const Float x1 = makeNormal(k * x);

  const BitT u2 = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float x2 = makeNormal(k * (t * zisc::mapTo01<Float>(u2) - o));

  const BitT u3 = Fnv1aHash::hash(u2);
  const Float x3 = makeNormal(k * (t * zisc::mapTo01<Float>(u3) - o));

  return std::array<Float, 3>{{x1, x2, x3}};
}

void testFmaF(std::ostream* output) noexcept;

void testFmaD(std::ostream* output) noexcept;

template <zisc::FloatingPoint Float> inline
constexpr std::array<Float, 2> makeFmodInput(const Float x) noexcept
{
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  using BitT = typename Binary::BitType;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;

  constexpr Float t = zisc::cast<Float>(2.0);
  constexpr Float o = zisc::cast<Float>(1.0);

  const Float x1 = makeNormal(x);

  const BitT u2 = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float x2 = makeNormal(t * zisc::mapTo01<Float>(u2) - o);

  return std::array<Float, 2>{{x1, x2}};
}

void testFmodF(std::ostream* output) noexcept;

void testFmodD(std::ostream* output) noexcept;

#endif // ZISC_MATH_REFERENCE_BASIC_TEST_HPP
