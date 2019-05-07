/*!
  \file pcg_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
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
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
PcgEngine<Base, Method, Seed, Result>::PcgEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xcafef00dd15ea5e5ull);
  setSeed(seed);
}

/*!
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
PcgEngine<Base, Method, Seed, Result>::PcgEngine(const SeedType seed) noexcept
{
  setSeed(seed);
}

namespace inner {

/*!
  */
template <typename ResultType, typename BitCountType> inline
ResultType rotateRightPcg(const ResultType value, const BitCountType rot) noexcept
{
  constexpr BitCountType bits = cast<BitCountType>(sizeof(ResultType) * 8);
  constexpr BitCountType mask = bits - 1;
  return cast<ResultType>(value >> rot) | cast<ResultType>(value << ((-rot) & mask));
}

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

template <typename Type> inline constexpr Type mcgMultiplierPcg();
template <> inline constexpr uint8b mcgMultiplierPcg<uint8b>() {return 217u;}
template <> inline constexpr uint16b mcgMultiplierPcg<uint16b>() {return 62169u;}
template <> inline constexpr uint32b mcgMultiplierPcg<uint32b>() {return 277803737u;}
template <> inline constexpr uint64b mcgMultiplierPcg<uint64b>() {return 12605985483714917081ull;}

/*!
  */
template <typename SeedType, typename ResultType, typename BitCountType>
struct PcgMixin<PcgMethod::RxsMXs, SeedType, ResultType, BitCountType>
{
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
    internal *= mcgMultiplierPcg<SeedType>();
    ResultType result = internal >> shift;
    result ^= result >> xshift;
    return result;
  }
};

} // namespace inner 

/*!
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
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr std::size_t PcgEngine<Base, Method, Seed, Result>::getPeriodPow2() noexcept
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  constexpr std::size_t period_pow2 = sizeof(SeedType) * 8 - (is_mcg ? 2 : 0);
  return period_pow2;
}

/*!
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result>
template <typename UnsignedInteger> inline
constexpr bool PcgEngine<Base, Method, Seed, Result>::isEndOfPeriod(
    const UnsignedInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer type.");
  constexpr std::size_t sample_bit_size = sizeof(UnsignedInteger) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr UnsignedInteger end_of_period =
        std::numeric_limits<UnsignedInteger>::max();
    return sample == end_of_period;
  }
  else {
    constexpr UnsignedInteger end_of_period =
        (cast<UnsignedInteger>(1u) << period_pow2) - 1;
    return sample == end_of_period;
  }
}

/*!
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
void PcgEngine<Base, Method, Seed, Result>::setSeed(const SeedType seed) noexcept
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  state_ = is_mcg ? (seed | cast<SeedType>(3u)) : bump(seed + increment());
}

/*!
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
auto PcgEngine<Base, Method, Seed, Result>::bump(const SeedType state) const noexcept
    -> SeedType
{
  return state * multiplier() + increment();
}

/*!
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

namespace inner {

template <typename Type> inline constexpr Type incrementPcg();
template <> inline constexpr uint8b incrementPcg<uint8b>() {return 77u;}
template <> inline constexpr uint16b incrementPcg<uint16b>() {return 47989u;}
template <> inline constexpr uint32b incrementPcg<uint32b>() {return 2891336453u;}
template <> inline constexpr uint64b incrementPcg<uint64b>() {return 1442695040888963407ull;}

template <typename Type> inline constexpr Type multiplierPcg();
template <> inline constexpr uint8b multiplierPcg<uint8b>() {return 141u;}
template <> inline constexpr uint16b multiplierPcg<uint16b>() {return 12829u;}
template <> inline constexpr uint32b multiplierPcg<uint32b>() {return 747796405u;}
template <> inline constexpr uint64b multiplierPcg<uint64b>() {return 6364136223846793005ull;}

} // namespace inner

template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Method, Seed, Result>::increment() noexcept
    -> SeedType
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  return is_mcg ? 0 : inner::incrementPcg<SeedType>();
}

template <PcgBase Base, PcgMethod Method, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Method, Seed, Result>::multiplier() noexcept
    -> SeedType
{
  return inner::multiplierPcg<SeedType>();
}

} // namespace zisc

#endif // ZISC_PCG_ENGINE_INL_HPP
