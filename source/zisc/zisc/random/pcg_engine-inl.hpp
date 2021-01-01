/*!
  \file pcg_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PCG_ENGINE_INL_HPP
#define ZISC_PCG_ENGINE_INL_HPP

#include "pcg_engine.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
PcgEngine<ValueT, kBase>::PcgEngine() noexcept
{
  setSeed(BaseEngine::defaultSeed());
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
PcgEngine<ValueT, kBase>::PcgEngine(const ValueType seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
auto PcgEngine<ValueT, kBase>::generate() noexcept -> ValueType
{
  const ValueType base = generateBase();
  const ValueType result = output(base);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
constexpr std::size_t PcgEngine<ValueT, kBase>::getPeriodPow2() noexcept
{
  constexpr bool is_mcg = kBase == PcgBase::Mcg;
  constexpr std::size_t period_pow2 = 8 * sizeof(ValueType) - (is_mcg ? 2 : 0);
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase>
template <UnsignedInteger Integer> inline
constexpr bool PcgEngine<ValueT, kBase>::isEndOfPeriod(
    const Integer sample) noexcept
{
  constexpr std::size_t sample_bit_size = sizeof(Integer) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr Integer end_of_period = std::numeric_limits<Integer>::max();
    return sample == end_of_period;
  }
  else {
    constexpr Integer end_of_period = (cast<Integer>(1u) << period_pow2) - 1;
    return sample == end_of_period;
  }
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
void PcgEngine<ValueT, kBase>::setSeed(const ValueType seed) noexcept
{
  constexpr bool is_mcg = (kBase == PcgBase::Mcg);
  state_ = is_mcg ? (seed | cast<ValueType>(3)) : bump(seed + increment());
}

/*!
  \details No detailed description

  \param [in] state No description.
  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
auto PcgEngine<ValueT, kBase>::bump(const ValueType state) const noexcept -> ValueType
{
  const ValueType result = state * multiplier() + increment();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
auto PcgEngine<ValueT, kBase>::generateBase() noexcept -> ValueType
{
  if constexpr (kOutputPrevious) {
    const ValueType old_state = state_;
    state_ = bump(state_);
    return old_state;
  }
  else {
    state_ = bump(state_);
    return state_;
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
constexpr auto PcgEngine<ValueT, kBase>::increment() noexcept -> ValueType
{
  ValueType i = 0;
  constexpr bool is_mcg = kBase == PcgBase::Mcg;
  if constexpr (!is_mcg) {
    if constexpr (sizeof(ValueType) == 1)
      i = 77u;
    else if constexpr (sizeof(ValueType) == 2)
      i = 47989u;
    else if constexpr (sizeof(ValueType) == 4)
      i = 2891336453u;
    else if constexpr (sizeof(ValueType) == 8)
      i = 1442695040888963407ull;
  }
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
constexpr auto PcgEngine<ValueT, kBase>::mcgMultiplier() noexcept -> ValueType
{
  ValueType m = 0;
  if constexpr (sizeof(ValueType) == 1)
    m = 217u;
  else if constexpr (sizeof(ValueType) == 2)
    m = 62169u;
  else if constexpr (sizeof(ValueType) == 4)
    m = 277803737u;
  else if constexpr (sizeof(ValueType) == 8)
    m = 12605985483714917081ull;
  return m;
}

/*!
  \details No detailed description

  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
constexpr auto PcgEngine<ValueT, kBase>::multiplier() noexcept -> ValueType
{
  ValueType m = 0;
  if constexpr (sizeof(ValueType) == 1)
    m = 141u;
  else if constexpr (sizeof(ValueType) == 2)
    m = 12829u;
  else if constexpr (sizeof(ValueType) == 4)
    m = 747796405u;
  else if constexpr (sizeof(ValueType) == 8)
    m = 6364136223846793005ull;
  return m;
}

/*!
  \details No detailed description

  \param [in] internal No description.
  \return No description
  */
template <UnsignedInteger ValueT, PcgBase kBase> inline
auto PcgEngine<ValueT, kBase>::output(ValueType internal) noexcept -> ValueType
{
  // Constant values
  constexpr BitCountType xtypebits = cast<BitCountType>(8 * sizeof(ValueType));
  constexpr BitCountType bits = cast<BitCountType>(8 * sizeof(ValueType));
  constexpr BitCountType opbits = (128 <= xtypebits) ? 6 :
                                  ( 64 <= xtypebits) ? 5 :
                                  ( 32 <= xtypebits) ? 4 :
                                  ( 16 <= xtypebits) ? 3 :
                                                       2;
  constexpr BitCountType shift = bits - xtypebits;
  constexpr BitCountType mask = (1 << opbits) - 1;
  constexpr BitCountType xshift = (2u * xtypebits + 2u) / 3u;

  const BitCountType rshift = opbits
      ? cast<BitCountType>(internal >> (bits - opbits)) & mask
      : 0;
  internal ^= internal >> (opbits + rshift);
  internal *= mcgMultiplier();
  ValueType result = internal >> shift;
  result ^= result >> xshift;
  return result;
}

} // namespace zisc

#endif // ZISC_PCG_ENGINE_INL_HPP
