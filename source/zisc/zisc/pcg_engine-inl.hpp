/*!
  \file pcg_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PCG_ENGINE_INL_HPP
#define ZISC_PCG_ENGINE_INL_HPP

#include "pcg_engine.hpp"
// Standard C++ library
#include <limits>
#include <type_traits>
// Zisc
#include "error.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
PcgEngine<Base, Algorithm, Seed, Result>::PcgEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xcafef00dd15ea5e5ULL);
  setSeed(seed);
}

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
PcgEngine<Base, Algorithm, Seed, Result>::PcgEngine(const SeedType seed) noexcept
{
  setSeed(seed);
}

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename SeedType, typename ResultType> inline
ResultType rotateRight(ResultType value, const SeedType rot) noexcept
{
  constexpr SeedType bits = cast<SeedType>(sizeof(ResultType) * 8);
  constexpr SeedType mask = bits - 1;
  return (value >> rot) | (value << ((-rot) & mask));
}

template <PcgAlgorithm Algorithm, typename SeedType, typename ResultType>
struct PcgMixin;

/*!
  \details
  No detailed.
  */
template <typename SeedType, typename ResultType>
struct PcgMixin<PcgAlgorithm::XshRr, SeedType, ResultType>
{
  static ResultType output(SeedType internal) noexcept
  {
    // Constant values
    constexpr SeedType bits = cast<SeedType>(sizeof(SeedType) * 8);
    constexpr SeedType xtypebits = cast<SeedType>(sizeof(ResultType) * 8);
    constexpr SeedType sparebits = bits - xtypebits;
    constexpr SeedType wantedopbits = xtypebits >= 128 ? 7 :
                                      xtypebits >=  64 ? 6 :
                                      xtypebits >=  32 ? 5 :
                                      xtypebits >=  16 ? 4 :
                                                         3;
    constexpr SeedType opbits = (sparebits >= wantedopbits) ? wantedopbits
                                                            : sparebits;
    constexpr SeedType amplifier = wantedopbits - opbits;
    constexpr SeedType mask = (1 << opbits) - 1;
    constexpr SeedType topspare = opbits;
    constexpr SeedType bottomspare = sparebits - topspare;
    constexpr SeedType xshift = (topspare + xtypebits) / 2;

    // Calc a random number
    SeedType rot = opbits ? SeedType(internal >> (bits - opbits)) & mask : 0;
    SeedType amprot = (rot << amplifier) & mask;
    internal ^= internal >> xshift;
    ResultType result = cast<ResultType>(internal >> bottomspare);
    result = rotateRight(result, amprot);
    return result;
  }
};

template <typename Type> constexpr Type mcgMultiplier() noexcept;
template <> inline constexpr uint8 mcgMultiplier() noexcept {return 217U;}
template <> inline constexpr uint16 mcgMultiplier() noexcept {return 62169U;}
template <> inline constexpr uint32 mcgMultiplier() noexcept {return 277803737U;}
template <> inline constexpr uint64 mcgMultiplier() noexcept {return 12605985483714917081ULL;}

/*!
  \details
  No detailed.
  */
template <typename SeedType, typename ResultType>
struct PcgMixin<PcgAlgorithm::RxsMXs, SeedType, ResultType>
{
  static ResultType output(SeedType internal) noexcept
  {
    // Constant values
    constexpr SeedType xtypebits = cast<SeedType>(sizeof(ResultType) * 8);
    constexpr SeedType bits = cast<SeedType>(sizeof(SeedType) * 8);
    constexpr SeedType opbits = xtypebits >= 128 ? 6 :
                                xtypebits >=  64 ? 5 :
                                xtypebits >=  32 ? 4 :
                                xtypebits >=  16 ? 3 :
                                                   2;
    constexpr SeedType shift = bits - xtypebits;
    constexpr SeedType mask = (1 << opbits) - 1;

    // Calc a random number
    SeedType rshift = opbits ? cast<SeedType>(internal >> (bits - opbits)) & mask : 0;
    internal ^= internal >> (opbits + rshift);
    internal *= mcgMultiplier<ResultType>();
    ResultType result = internal >> shift;
    result ^= result >> ((2U * xtypebits + 2U) / 3U);
    return result;
  }
};

} // namespace inner 

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
auto PcgEngine<Base, Algorithm, Seed, Result>::generate() noexcept -> ResultType
{
  const auto random = inner::PcgMixin<Algorithm, Seed, Result>::output(state_);
  next();
  return random;
}

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
void PcgEngine<Base, Algorithm, Seed, Result>::setSeed(const SeedType seed) noexcept
{
  constexpr bool is_lcg = (Base == PcgBase::Lcg);
  state_ = is_lcg ? bump(seed + increment()) : (seed | cast<SeedType>(3U));
}

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
auto PcgEngine<Base, Algorithm, Seed, Result>::bump(const SeedType state) const noexcept
    -> SeedType
{
  return state * multiplier() + increment();
}

namespace inner {

template <typename Type> constexpr Type increment() noexcept;
template <> constexpr uint8 increment() noexcept {return 77U;}
template <> constexpr uint16 increment() noexcept {return 47989U;}
template <> constexpr uint32 increment() noexcept {return 2891336453U;}
template <> constexpr uint64 increment() noexcept {return 1442695040888963407ULL;}

template <typename Type> constexpr Type multiplier() noexcept;
template <> constexpr uint8 multiplier() noexcept {return 141U;}
template <> constexpr uint16 multiplier() noexcept {return 12829U;}
template <> constexpr uint32 multiplier() noexcept {return 747796405U;}
template <> constexpr uint64 multiplier() noexcept {return 6364136223846793005ULL;}

} // namespace inner

template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Algorithm, Seed, Result>::increment() noexcept -> SeedType
{
  constexpr bool is_mcg = (Base == PcgBase::Mcg);
  return is_mcg ? 0 : inner::increment<SeedType>();
}

template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
constexpr auto PcgEngine<Base, Algorithm, Seed, Result>::multiplier() noexcept -> SeedType
{
  return inner::multiplier<SeedType>();
}

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result> inline
void PcgEngine<Base, Algorithm, Seed, Result>::next() noexcept
{
  state_ = bump(state_);
}

} // namespace zisc

#endif // ZISC_PCG_ENGINE_INL_HPP
