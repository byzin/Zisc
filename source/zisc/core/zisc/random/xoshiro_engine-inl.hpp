/*!
  \file xoshiro_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_ENGINE_INL_HPP
#define ZISC_XOSHIRO_ENGINE_INL_HPP

#include "xoshiro_engine.hpp"
// Standard C++ library
#include <array>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
XoshiroEngine<T, kMethod>::XoshiroEngine() noexcept
{
  setSeed(BaseEngineT::defaultSeed());
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
XoshiroEngine<T, kMethod>::XoshiroEngine(const ValueT seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
auto XoshiroEngine<T, kMethod>::generate() noexcept -> ValueT
{
  const ValueT result = generateRandom();
  nextState();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
constexpr std::size_t XoshiroEngine<T, kMethod>::getPeriodPow2() noexcept
{
  constexpr std::size_t period_pow2 = 4 * std::numeric_limits<ValueT>::digits;
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod>
template <std::unsigned_integral Integer> inline
constexpr bool XoshiroEngine<T, kMethod>::isEndOfPeriod(const Integer sample) noexcept
{
  constexpr std::size_t sample_bit_size = sizeof(Integer) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr Integer end_of_period = (sample_bit_size == period_pow2)
        ? std::numeric_limits<Integer>::max() - 1
        : std::numeric_limits<Integer>::max();
    return sample == end_of_period;
  }
  else {
    constexpr Integer end_of_period = (cast<Integer>(1u) << period_pow2) - 2;
    return sample == end_of_period;
  }
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
void XoshiroEngine<T, kMethod>::setSeed(const ValueT seed) noexcept
{
  state_.fill(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<T, kMethod>::a() noexcept -> ValueT
{
  ValueT value = 0;
  if constexpr (sizeof(ValueT) == 4)
    value = 7;
  else if constexpr (sizeof(ValueT) == 8)
    value = 23;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<T, kMethod>::b() noexcept -> ValueT
{
  ValueT value = 0;
  if constexpr (sizeof(ValueT) == 4)
    value = 9;
  else if constexpr (sizeof(ValueT) == 8)
    value = 17;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<T, kMethod>::c() noexcept -> ValueT
{
  ValueT value = 0;
  if constexpr (sizeof(ValueT) == 4)
    value = 11;
  else if constexpr (sizeof(ValueT) == 8)
    value = 45;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
auto XoshiroEngine<T, kMethod>::generateRandom() noexcept -> ValueT
{
  ValueT result = 0;
  if constexpr (kMethod == XoshiroMethod::Plus)
    result = state_[0] + state_[3];
  else if constexpr (kMethod == XoshiroMethod::PlusPlus)
    result = rotateLeft<a()>(state_[0] + state_[3]) + state_[0];
  else if constexpr (kMethod == XoshiroMethod::StarStar)
    result = rotateLeft<7>(state_[1] * 5u) * 9u;
  return result;
}

/*!
  \details No detailed description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> inline
void XoshiroEngine<T, kMethod>::nextState() noexcept
{
  const ValueT t = state_[1] << b();
  state_[2] ^= state_[0];
  state_[3] ^= state_[1];
  state_[1] ^= state_[2];
  state_[0] ^= state_[3];
  state_[2] ^= t;
  state_[3] = rotateLeft<c()>(state_[3]);
}

/*!
  \details No detailed description

  \tparam k No description.
  \param [in] x No description.
  \return No description
  */
template <std::unsigned_integral T, XoshiroMethod kMethod> template <T k> inline
auto XoshiroEngine<T, kMethod>::rotateLeft(const ValueT x) noexcept -> ValueT
{
  constexpr ValueT bit_size = std::numeric_limits<ValueT>::digits;
  const auto result = cast<ValueT>((x << k) | (x >> (bit_size - k)));
  return result;
}

} // namespace zisc

#endif // ZISC_XOSHIRO_ENGINE_INL_HPP
