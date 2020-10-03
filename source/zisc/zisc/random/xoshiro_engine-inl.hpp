/*!
  \file xoshiro_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_ENGINE_INL_HPP
#define ZISC_XOSHIRO_ENGINE_INL_HPP

#include "xoshiro_engine.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
XoshiroEngine<ValueT, kMethod>::XoshiroEngine() noexcept
{
  setSeed(BaseEngine::defaultSeed());
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
XoshiroEngine<ValueT, kMethod>::XoshiroEngine(const ValueType seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
auto XoshiroEngine<ValueT, kMethod>::generate() noexcept -> ValueType
{
  const ValueType result = generateRandom();
  nextState();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
constexpr std::size_t XoshiroEngine<ValueT, kMethod>::getPeriodPow2() noexcept
{
  constexpr std::size_t period_pow2 = 4 * std::numeric_limits<ValueType>::digits;
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod>
template <UnsignedInteger Integer> inline
constexpr bool XoshiroEngine<ValueT, kMethod>::isEndOfPeriod(
    const Integer sample) noexcept
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
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
void XoshiroEngine<ValueT, kMethod>::setSeed(const ValueType seed) noexcept
{
  state_.fill(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<ValueT, kMethod>::a() noexcept -> ValueType
{
  ValueType value = 0;
  if constexpr (sizeof(ValueType) == 4)
    value = 7;
  else if constexpr (sizeof(ValueType) == 8)
    value = 23;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<ValueT, kMethod>::b() noexcept -> ValueType
{
  ValueType value = 0;
  if constexpr (sizeof(ValueType) == 4)
    value = 9;
  else if constexpr (sizeof(ValueType) == 8)
    value = 17;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
constexpr auto XoshiroEngine<ValueT, kMethod>::c() noexcept -> ValueType
{
  ValueType value = 0;
  if constexpr (sizeof(ValueType) == 4)
    value = 11;
  else if constexpr (sizeof(ValueType) == 8)
    value = 45;
  return value;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
auto XoshiroEngine<ValueT, kMethod>::generateRandom() noexcept -> ValueType
{
  ValueType result = 0;
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
template <UnsignedInteger ValueT, XoshiroMethod kMethod> inline
void XoshiroEngine<ValueT, kMethod>::nextState() noexcept
{
  const ValueType t = state_[1] << b();
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
template <UnsignedInteger ValueT, XoshiroMethod kMethod>
template <ValueT k> inline
auto XoshiroEngine<ValueT, kMethod>::rotateLeft(const ValueType x) noexcept
    -> ValueType
{
  constexpr ValueType bit_size = std::numeric_limits<ValueType>::digits;
  const ValueType result = cast<ValueType>((x << k) | (x >> (bit_size - k)));
  return result;
}

} // namespace zisc

#endif // ZISC_XOSHIRO_ENGINE_INL_HPP
