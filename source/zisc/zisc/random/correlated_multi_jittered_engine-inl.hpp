/*!
  \file correlated_multi_jittered_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] s No description.
  \param [in] p No description.
  \return No description
  */
template <uint32b kRootN> template <FloatingPoint Float> inline
Float CorrelatedMultiJitteredEngine<kRootN>::generate1d(
    const ValueType s,
    const ValueType p) noexcept
{
  constexpr ValueType n = getPeriod();

  // Generate a random
  const ValueType u = hashInteger(s, p * 0x967a889b);
  Float x = mapTo01<Float>(u);

  // Random jitter
  constexpr Float inv_n = invert(cast<Float>(n));
  const ValueType sx = permute<n>(s, p * 0x68bc21eb);
  x = inv_n * (cast<Float>(sx) + x);

  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] s No description.
  \param [in] p No description.
  \return No description
  */
template <uint32b kRootN> template <FloatingPoint Float> inline
std::array<Float, 2> CorrelatedMultiJitteredEngine<kRootN>::generate2d(
    ValueType s,
    const ValueType p) noexcept
{
  constexpr ValueType n = getPeriod();

  // Generate randoms
  s = permute<n>(s, p * 0x51633e2d);
  const ValueType u1 = hashInteger(s, p * 0x967a889b);
  const ValueType u2 = hashInteger(s, p * 0x368cc8b7);
  Float x = mapTo01<Float>(u1);
  Float y = mapTo01<Float>(u2);

  // Random jitter
  constexpr Float inv_n = invert(cast<Float>(n));
  constexpr Float inv_root_n = invert(cast<Float>(kRootN));
  const ValueType s_div = s / kRootN;
  const ValueType s_mod = s  - s_div * kRootN;
  const ValueType sx = permute<kRootN>(s_mod, p * 0x68bc21eb);
  const ValueType sy = permute<kRootN>(s_div, p * 0x02e5be93);
  x = inv_root_n * (cast<Float>(sx) + inv_root_n * (cast<Float>(sy) + x));
  y = inv_n * (cast<Float>(s) + y);

  return std::array<Float, 2>{{x, y}};
}

/*!
  \details No detailed description

  \return No description
  */
template <uint32b kRootN> inline
constexpr std::size_t CorrelatedMultiJitteredEngine<kRootN>::getPeriod() noexcept
{
  constexpr std::size_t period = kRootN * kRootN;
  return period;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <uint32b kRootN> template <UnsignedInteger Integer> inline
constexpr bool CorrelatedMultiJitteredEngine<kRootN>::isEndOfPeriod(
    const Integer sample) noexcept
{
  constexpr Integer umax = (std::numeric_limits<Integer>::max)();
  constexpr Integer end_of_period = (umax < getPeriod()) ? umax : getPeriod() - 1;
  const bool is_end_of_period = sample == end_of_period;
  return is_end_of_period;
}

/*!
  \details No detailed description

  \param [in] i No description.
  \param [in] p No description.
  \return No description
  */
template <uint32b kRootN> inline
auto CorrelatedMultiJitteredEngine<kRootN>::hashInteger(
    ValueType i,
    const ValueType p) noexcept -> ValueType
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
  \details No detailed description

  \param [in] w No description.
  \return No description
  */
template <uint32b kRootN> inline
constexpr auto CorrelatedMultiJitteredEngine<kRootN>::makeWMask(ValueType w) noexcept
    -> ValueType
{
  w |= w >> 1;
  w |= w >> 2;
  w |= w >> 4;
  w |= w >> 8;
  w |= w >> 16;
  return w;
}

/*!
  \details No detailed description

  \tparam l No description.
  \param [in] i No description.
  \param [in] p No description.
  \return No description
  */
template <uint32b kRootN> template <uint32b l> inline
auto CorrelatedMultiJitteredEngine<kRootN>::permute(
    ValueType i,
    const ValueType p) noexcept -> ValueType
{
  constexpr bool is_power_of_2 = (1 < l) && has_single_bit(l);
  constexpr ValueType w = makeWMask(l - 1);
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
  \details No detailed description

  \tparam w No description.
  \param [in] i No description.
  \param [in] p No description.
  \return No description
  */
template <uint32b kRootN> template <uint32b w> inline
auto CorrelatedMultiJitteredEngine<kRootN>::permuteImpl(
    ValueType i,
    const ValueType p) noexcept -> ValueType
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
