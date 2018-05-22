/*!
  \file xorshift_star_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XORSHIFT_STAR_ENGINE_INL_HPP
#define ZISC_XORSHIFT_STAR_ENGINE_INL_HPP

#include "xorshift_star_engine-inl.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::XorshiftStarEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xc1f651c67c62c6e0ull);
  setSeed(seed);
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::XorshiftStarEngine(
    const SeedType seed) noexcept
{
  setSeed(seed);
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
auto XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::generate() noexcept
    -> ResultType
{
  constexpr std::size_t result_bit_size = sizeof(ResultType) * 8;
  constexpr std::size_t seed_bit_size = sizeof(SeedType) * 8;
  static_assert(result_bit_size <= seed_bit_size);
  const ResultType result =
      (state_ * multiplier()) >> (seed_bit_size - result_bit_size);
  advance();
  return result;
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
constexpr std::size_t XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::
    getPeriodPow2() noexcept
{
  constexpr std::size_t period_pow2 = sizeof(SeedType) * 8;
  return period_pow2;
}


/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC>
template <typename UnsignedInteger> inline
constexpr bool XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::
    isEndOfPeriod(const UnsignedInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer type.");
  constexpr std::size_t sample_bit_size = sizeof(UnsignedInteger) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr UnsignedInteger end_of_period = (sample_bit_size == period_pow2)
        ? std::numeric_limits<UnsignedInteger>::max() - 1
        : std::numeric_limits<UnsignedInteger>::max();
    return sample == end_of_period;
  }
  else {
    constexpr UnsignedInteger end_of_period =
        (cast<UnsignedInteger>(1u) << period_pow2) - 2;
    return sample == end_of_period;
  }
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
void XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::setSeed(
    const SeedType seed) noexcept
{
  state_ = seed;
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
void XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::advance() noexcept
{
  state_ ^= state_ >> kA;
  state_ ^= state_ << kB;
  state_ ^= state_ >> kC;
}

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8 kA, uint8 kB, uint8 kC> inline
constexpr auto XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::multiplier()
    noexcept -> SeedType
{
  return kMultiplier;
}

} // namespace zisc

#endif // ZISC_XORSHIFT_STAR_ENGINE_INL_HPP
