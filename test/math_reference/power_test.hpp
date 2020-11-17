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

// Standard C++ library
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
  const Float sign = zisc::isNegative(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
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
  const Float sign = zisc::isNegative(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  // Significand
  Float subnormal = x * subnormal_max;
  subnormal = sign * zisc::clamp(subnormal, Limits::denorm_min(), subnormal_max);
  return subnormal;
}

template <zisc::FloatingPoint Float> inline
constexpr Float makePowInput(const Float x) noexcept
{
  constexpr Float k = zisc::cast<Float>(8.0);
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
