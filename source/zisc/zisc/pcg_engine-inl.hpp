/*!
  \file pcg_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "error.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
PcgEngine<Base, Method, Seed, Result>::PcgEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xcafef00dd15ea5e5ull);
  setSeed(seed);
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
PcgEngine<Base, Method, Seed, Result>::PcgEngine(const SeedType seed) noexcept
{
  setSeed(seed);
}

namespace inner {

template <PcgMethod, typename SeedType, typename ResultType, typename BitCountType>
struct PcgMixin;

/*!
  */
template <typename SeedType, typename ResultType, typename BitCountType>
struct PcgMixin<PcgMethod::XshRs, SeedType, ResultType, BitCountType>
{
  static ResultType output(SeedType internal) noexcept
  {
    // Constant values
    constexpr BitCountType bits = cast<BitCountType>(sizeof(SeedType) * 8);
    constexpr BitCountType xtypebits = cast<BitCountType>(sizeof(ResultType) * 8);
    constexpr BitCountType sparebits = bits - xtypebits;
    constexpr BitCountType opbits = (64 <= sparebits - 5) ? 5 :
                                    (32 <= sparebits - 4) ? 4 :
                                    (16 <= sparebits - 3) ? 3 :
                                    ( 4 <= sparebits - 2) ? 2 :
                                    ( 1 <= sparebits - 1) ? 1
                                                          : 0;
    constexpr BitCountType mask = (1 << opbits) - 1;
    constexpr BitCountType maxrandshift = mask;
    constexpr BitCountType topspare = opbits;
    constexpr BitCountType bottomspare = sparebits - topspare;
    constexpr BitCountType xshift = topspare + (xtypebits + maxrandshift) / 2;

    const BitCountType rshift = opbits
        ? cast<BitCountType>(internal >> (bits - opbits)) & mask
        : 0;
    internal ^= internal >> xshift;
    const ResultType result =
        cast<ResultType>(internal >> (bottomspare - maxrandshift + rshift));
    return result;
  }
};

/*!
  */
template <typename SeedType, typename ResultType, typename BitCountType>
struct PcgMixin<PcgMethod::XshRr, SeedType, ResultType, BitCountType>
{
  static ResultType rotateRightPcg(const ResultType value,
                                   const BitCountType rot) noexcept
  {
    constexpr BitCountType bits = cast<BitCountType>(sizeof(ResultType) * 8);
    constexpr BitCountType mask = bits - 1;
    return cast<ResultType>(value >> rot) | cast<ResultType>(value << ((-rot) & mask));
  }

  static ResultType output(SeedType internal) noexcept
  {
    // Constant values
    constexpr BitCountType bits = cast<BitCountType>(sizeof(SeedType) * 8);
    constexpr BitCountType xtypebits = cast<BitCountType>(sizeof(ResultType) * 8);
    constexpr BitCountType sparebits = bits - xtypebits;
    constexpr BitCountType wantedopbits = (128 <= xtypebits) ? 7 :
                                          ( 64 <= xtypebits) ? 6 :
                                          ( 32 <= xtypebits) ? 5 :
                                          ( 16 <= xtypebits) ? 4 :
                                                               3;
    constexpr BitCountType opbits = (wantedopbits <= sparebits) ? wantedopbits
                                                                : sparebits;
    constexpr BitCountType amplifier = wantedopbits - opbits;
    constexpr BitCountType mask = (1 << opbits) - 1;
    constexpr BitCountType topspare = opbits;
    constexpr BitCountType bottomspare = sparebits - topspare;
    constexpr BitCountType xshift = (topspare + xtypebits) / 2;

    const BitCountType rot = opbits
        ? cast<BitCountType>(internal >> (bits - opbits)) & mask
        : 0;
    const BitCountType amprot = (rot << amplifier) & mask;
    internal ^= internal >> xshift;
    ResultType result = cast<ResultType>(internal >> bottomspare);
    result = rotateRightPcg(result, amprot);
    return result;
  }
};

/*!
  */
template <typename SeedType, typename ResultType, typename BitCountType>
struct PcgMixin<PcgMethod::RxsMXs, SeedType, ResultType, BitCountType>
{
  static constexpr SeedType mcgMultiplierPcg() noexcept
  {
    SeedType m = 0;
    if constexpr (std::is_same_v<uint8b, SeedType>)
      m = 217u;
    else if constexpr (std::is_same_v<uint16b, SeedType>)
      m = 62169u;
    else if constexpr (std::is_same_v<uint32b, SeedType>)
      m = 277803737u;
    else if constexpr (std::is_same_v<uint64b, SeedType>)
      m = 12605985483714917081ull;
    return m;
  }

  static ResultType output(SeedType internal) noexcept
  {
    // Constant values
    constexpr BitCountType xtypebits = cast<BitCountType>(sizeof(ResultType) * 8);
    constexpr BitCountType bits = cast<BitCountType>(sizeof(SeedType) * 8);
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
    internal *= mcgMultiplierPcg();
    ResultType result = internal >> shift;
    result ^= result >> xshift;
    return result;
  }
};

} // namespace inner 

/*!
  \details No detailed description

  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
auto PcgEngine<Base, Method, Seed, Result>::generate() noexcept -> ResultType
{
  const auto base = generateBase();
  const auto random =
      inner::PcgMixin<Method, SeedType, ResultType, BitCountType>::output(base);
  return random;
}

/*!
  \details No detailed description

  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr std::size_t PcgEngine<Base, Method, Seed, Result>::getPeriodPow2() noexcept
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  constexpr std::size_t period_pow2 = sizeof(SeedType) * 8 - (is_mcg ? 2 : 0);
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam UInteger No description.
  \param [in] sample No description.
  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result>
template <typename UInteger> inline
constexpr bool PcgEngine<Base, Method, Seed, Result>::isEndOfPeriod(
    const UInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UInteger>,
                "UInteger isn't unsigned integer type.");
  constexpr std::size_t sample_bit_size = sizeof(UInteger) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr UInteger end_of_period = std::numeric_limits<UInteger>::max();
    return sample == end_of_period;
  }
  else {
    constexpr UInteger end_of_period = (cast<UInteger>(1u) << period_pow2) - 1;
    return sample == end_of_period;
  }
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
void PcgEngine<Base, Method, Seed, Result>::setSeed(const SeedType seed) noexcept
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  state_ = is_mcg ? (seed | cast<SeedType>(3u)) : bump(seed + increment());
}

/*!
  \details No detailed description

  \param [in] state No description.
  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
auto PcgEngine<Base, Method, Seed, Result>::bump(const SeedType state)
    const noexcept -> SeedType
{
  const SeedType result = state * multiplier() + increment();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
auto PcgEngine<Base, Method, Seed, Result>::generateBase() noexcept -> SeedType
{
  if constexpr (kOutputPrevious) {
    const SeedType old_state = state_;
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
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Method, Seed, Result>::increment() noexcept
    -> SeedType
{
  using SType = std::remove_cv_t<SeedType>;
  SType i = 0;
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  if constexpr (!is_mcg) {
    if constexpr (std::is_same_v<uint8b, SType>)
      i = 77u;
    else if constexpr (std::is_same_v<uint16b, SType>)
      i = 47989u;
    else if constexpr (std::is_same_v<uint32b, SType>)
      i = 2891336453u;
    else if constexpr (std::is_same_v<uint64b, SType>)
      i = 1442695040888963407ull;
  }
  return i;
}

/*!
  \details No detailed description

  \return No description
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Method, Seed, Result>::multiplier() noexcept
    -> SeedType
{
  using SType = std::remove_cv_t<SeedType>;
  SType m = 0;
  if constexpr (std::is_same_v<uint8b, SType>)
    m = 141u;
  else if constexpr (std::is_same_v<uint16b, SType>)
    m = 12829u;
  else if constexpr (std::is_same_v<uint32b, SType>)
    m = 747796405u;
  else if constexpr (std::is_same_v<uint64b, SType>)
    m = 6364136223846793005ull;
  return m;
}

} // namespace zisc

#endif // ZISC_PCG_ENGINE_INL_HPP
