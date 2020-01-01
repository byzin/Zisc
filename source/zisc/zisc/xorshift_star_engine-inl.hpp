/*!
  \file xorshift_star_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XORSHIFT_STAR_ENGINE_INL_HPP
#define ZISC_XORSHIFT_STAR_ENGINE_INL_HPP

#include "xorshift_star_engine.hpp"
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
  \details No detailed description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::XorshiftStarEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xc1f651c67c62c6e0ull);
  setSeed(seed);
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::XorshiftStarEngine(
    const SeedType seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
auto XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::generate() noexcept
    -> ResultType
{
  constexpr std::size_t result_bit_size = sizeof(ResultType) * 8;
  constexpr std::size_t seed_bit_size = sizeof(SeedType) * 8;
  static_assert(result_bit_size <= seed_bit_size);
  const ResultType result = (state_ * multiplier()) >>
                            (seed_bit_size - result_bit_size);
  advance();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
constexpr std::size_t XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::
    getPeriodPow2() noexcept
{
  constexpr std::size_t period_pow2 = sizeof(SeedType) * 8;
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam UInteger No description.
  \param [in] sample No description.
  \return No description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC>
template <typename UInteger> inline
constexpr bool XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::
    isEndOfPeriod(const UInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UInteger>,
                "UInteger isn't unsigned integer type.");
  constexpr std::size_t sample_bit_size = sizeof(UInteger) * 8;
  constexpr std::size_t period_pow2 = getPeriodPow2();
  if constexpr (sample_bit_size <= period_pow2) {
    // Workaround
    constexpr UInteger end_of_period = (sample_bit_size == period_pow2)
        ? std::numeric_limits<UInteger>::max() - 1
        : std::numeric_limits<UInteger>::max();
    return sample == end_of_period;
  }
  else {
    constexpr UInteger end_of_period = (cast<UInteger>(1u) << period_pow2) - 2;
    return sample == end_of_period;
  }
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
void XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::setSeed(
    const SeedType seed) noexcept
{
  state_ = seed;
}

/*!
  \details No detailed description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
void XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::advance() noexcept
{
  state_ ^= state_ >> kA;
  state_ ^= state_ << kB;
  state_ ^= state_ >> kC;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC> inline
constexpr auto XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>::multiplier()
    noexcept -> SeedType
{
  return kMultiplier;
}

} // namespace zisc

#endif // ZISC_XORSHIFT_STAR_ENGINE_INL_HPP
