/*!
  \file correlated_multi_jittered_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CORRELATED_MULTI_JITTERED_ENGINE_INL_HPP
#define ZISC_CORRELATED_MULTI_JITTERED_ENGINE_INL_HPP

#include "correlated_multi_jittered_engine.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <cstdint>
// Zisc
#include "floating_point_bit.hpp"
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <uint32 kRootN> template <typename Float> inline
Float CorrelatedMultiJitteredEngine<kRootN>::generate1D(
    const uint32 s,
    const uint32 p) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr uint32 n = getPeriod();

  const uint32 sx = permute<n>(s, p * 0x68bc21eb);
  Float x = mapTo01Float<Float>(hashInteger(s, p * 0x967a889b));

  // Random jitter
  constexpr Float inv_n = zisc::invert(cast<Float>(n));
  x = inv_n * (cast<Float>(sx) + x);

  return x;
}

/*!
  */
template <uint32 kRootN> template <typename Float> inline
std::array<Float, 2> CorrelatedMultiJitteredEngine<kRootN>::generate2D(
    uint32 s,
    const uint32 p) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr uint32 n = getPeriod();

  s = permute<n>(s, p * 0x51633e2d);

  const uint32 s_div = s / kRootN;
  const uint32 s_mod = s  - s_div * kRootN;

  const uint32 sx = permute<kRootN>(s_mod, p * 0x68bc21eb);
  const uint32 sy = permute<kRootN>(s_div, p * 0x02e5be93);
  Float x = mapTo01Float<Float>(hashInteger(s, p * 0x967a889b));
  Float y = mapTo01Float<Float>(hashInteger(s, p * 0x368cc8b7));

  // Random jitter
  constexpr Float inv_n = zisc::invert(cast<Float>(n));
  constexpr Float inv_root_n = zisc::invert(cast<Float>(kRootN));
  x = inv_root_n * (cast<Float>(sx) + inv_root_n * (cast<Float>(sy) + x));
  y = inv_n * (cast<Float>(s) + y);

  return std::array<Float, 2>{{x, y}};
}

/*!
  */
template <uint32 kRootN> inline
constexpr std::size_t CorrelatedMultiJitteredEngine<kRootN>::getPeriod() noexcept
{
  constexpr std::size_t period = power<2>(kRootN);
  return period;
}

/*!
  */
template <uint32 kRootN> template <typename UnsignedInteger> inline
constexpr bool CorrelatedMultiJitteredEngine<kRootN>::isEndOfPeriod(
    const UnsignedInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer.");
  if (std::numeric_limits<UnsignedInteger>::max() < getPeriod()) {
    constexpr UnsignedInteger end_of_period =
        std::numeric_limits<UnsignedInteger>::max();
    return sample == end_of_period;
  }
  else {
    constexpr UnsignedInteger end_of_period = getPeriod() - 1;
    return sample == end_of_period;
  }
}

/*!
  */

/*!
  */
template <uint32 kRootN> inline
uint32 CorrelatedMultiJitteredEngine<kRootN>::hashInteger(uint32 i,
                                                          const uint32 p) noexcept
{
  i ^= p;
  i ^= i >> 17;
  i ^= i >> 10;
  i *= 0xb36534e5;
  i ^= i >> 12;
  i ^= i >> 21;
  i *= 0x93fc4795;
  i ^= 0xdf6e307f;
  i ^= i >> 17;
  i *= 1 | p >> 18;
  return i;
}

/*!
  */
template <uint32 kRootN> inline
constexpr uint32 CorrelatedMultiJitteredEngine<kRootN>::makeWMask(
    uint32 w) noexcept
{
  w |= w >> 1;
  w |= w >> 2;
  w |= w >> 4;
  w |= w >> 8;
  w |= w >> 16;
  return w;
}

/*!
  */
template <uint32 kRootN> template <typename Float> inline
Float CorrelatedMultiJitteredEngine<kRootN>::mapTo01Float(const uint32 x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  return FloatingPointBit<Float>::mapTo01Float(x);
}

/*!
  */
template <uint32 kRootN> template <uint32 l> inline
uint32 CorrelatedMultiJitteredEngine<kRootN>::permute(uint32 i,
                                                      const uint32 p) noexcept
{
  constexpr bool is_power_of_2 = (1 < l) && isPowerOf2(l);
  constexpr uint32 w = makeWMask(l - 1);
  if (is_power_of_2) {
    // fast case
    i = permuteImpl<w>(i, p);
    i = (i + p) & w;
  }
  else {
    // slow case
    do {
      i = permuteImpl<w>(i, p);
    } while (l <= i);
    i = (i + p) % l;
  }
  return i;
}

/*!
  */
template <uint32 kRootN> template <uint32 w> inline
uint32 CorrelatedMultiJitteredEngine<kRootN>::permuteImpl(uint32 i,
                                                          const uint32 p) noexcept
{
  i ^= p;
  i *= 0xe170893d;
  i ^= p >> 16;
  i ^= (i & w) >> 4;
  i ^= p >> 8;
  i *= 0x0929eb3f;
  i ^= p >> 23;
  i ^= (i & w) >> 1;
  i *= 1 | p >> 27;
  i *= 0x6935fa69;
  i ^= (i & w) >> 11;
  i *= 0x74dcb303;
  i ^= (i & w) >> 2;
  i *= 0x9e501cc3;
  i ^= (i & w) >> 2;
  i *= 0xc860a3df;
  i &= w;
  i ^= i >> 5;
  return i;
}

} // namespace zisc

#endif // ZISC_CORRELATED_MULTI_JITTERED_ENGINE_INL_HPP
