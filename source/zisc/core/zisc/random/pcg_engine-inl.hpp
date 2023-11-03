/*!
  \file pcg_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PCG_ENGINE_INL_HPP
#define ZISC_PCG_ENGINE_INL_HPP

#include "pcg_engine.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
PcgEngine<T, kBase>::PcgEngine() noexcept
{
  setSeed(BaseEngineT::defaultSeed());
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <std::unsigned_integral T, PcgBase kBase> inline
PcgEngine<T, kBase>::PcgEngine(const ValueT seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
auto PcgEngine<T, kBase>::generate() noexcept -> ValueT
{
  const ValueT base = generateBase();
  const ValueT result = output(base);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
constexpr auto PcgEngine<T, kBase>::getPeriodPow2() noexcept -> std::size_t
{
  constexpr bool is_mcg = kBase == PcgBase::Mcg;
  constexpr std::size_t period_pow2 = 8 * sizeof(ValueT) - (is_mcg ? 2 : 0);
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase>
template <std::unsigned_integral Integer> inline
constexpr auto PcgEngine<T, kBase>::isEndOfPeriod(const Integer sample) noexcept -> bool
{
  constexpr std::size_t sample_bit_size = sizeof(Integer) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr Integer end_of_period = std::numeric_limits<Integer>::max();
    return sample == end_of_period;
  }
  else {
    constexpr Integer end_of_period = (cast<Integer>(1U) << period_pow2) - 1;
    return sample == end_of_period;
  }
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <std::unsigned_integral T, PcgBase kBase> inline
void PcgEngine<T, kBase>::setSeed(const ValueT seed) noexcept
{
  constexpr bool is_mcg = (kBase == PcgBase::Mcg);
  state_ = is_mcg ? (seed | cast<ValueT>(3)) : bump(seed + increment());
}

/*!
  \details No detailed description

  \param [in] state No description.
  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
auto PcgEngine<T, kBase>::bump(const ValueT state) const noexcept -> ValueT
{
  const ValueT result = state * multiplier() + increment();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
auto PcgEngine<T, kBase>::generateBase() noexcept -> ValueT
{
  if constexpr (kOutputPrevious) {
    const ValueT old_state = state_;
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
template <std::unsigned_integral T, PcgBase kBase> inline
constexpr auto PcgEngine<T, kBase>::increment() noexcept -> ValueT
{
  ValueT i = 0;
  constexpr bool is_mcg = kBase == PcgBase::Mcg;
  if constexpr (!is_mcg) {
    if constexpr (sizeof(ValueT) == 1)
      i = 77U;
    else if constexpr (sizeof(ValueT) == 2)
      i = 47989U;
    else if constexpr (sizeof(ValueT) == 4)
      i = 2891336453U;
    else if constexpr (sizeof(ValueT) == 8)
      i = 1442695040888963407ULL;
  }
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
constexpr auto PcgEngine<T, kBase>::mcgMultiplier() noexcept -> ValueT
{
  ValueT m = 0;
  if constexpr (sizeof(ValueT) == 1)
    m = 217U;
  else if constexpr (sizeof(ValueT) == 2)
    m = 62169U;
  else if constexpr (sizeof(ValueT) == 4)
    m = 277803737U;
  else if constexpr (sizeof(ValueT) == 8)
    m = 12605985483714917081ULL;
  return m;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
constexpr auto PcgEngine<T, kBase>::multiplier() noexcept -> ValueT
{
  ValueT m = 0;
  if constexpr (sizeof(ValueT) == 1)
    m = 141U;
  else if constexpr (sizeof(ValueT) == 2)
    m = 12829U;
  else if constexpr (sizeof(ValueT) == 4)
    m = 747796405U;
  else if constexpr (sizeof(ValueT) == 8)
    m = 6364136223846793005ULL;
  return m;
}

/*!
  \details No detailed description

  \param [in] internal No description.
  \return No description
  */
template <std::unsigned_integral T, PcgBase kBase> inline
auto PcgEngine<T, kBase>::output(ValueT internal) noexcept -> ValueT
{
  // Constant values
  constexpr auto xtypebits = cast<BitCountType>(8 * sizeof(ValueT));
  constexpr auto bits = cast<BitCountType>(8 * sizeof(ValueT));
  constexpr BitCountType opbits = (128 <= xtypebits) ? 6 :
                                  ( 64 <= xtypebits) ? 5 :
                                  ( 32 <= xtypebits) ? 4 :
                                  ( 16 <= xtypebits) ? 3 :
                                                       2;
  constexpr BitCountType shift = bits - xtypebits;
  constexpr BitCountType mask = (1 << opbits) - 1;
  constexpr BitCountType xshift = (2U * xtypebits + 2U) / 3U;

  const BitCountType rshift = (opbits != 0U)
      ? cast<BitCountType>(internal >> (bits - opbits)) & mask
      : 0;
  internal ^= internal >> (opbits + rshift);
  internal *= mcgMultiplier();
  ValueT result = internal >> shift;
  result ^= result >> xshift;
  return result;
}

} // namespace zisc

#endif // ZISC_PCG_ENGINE_INL_HPP
