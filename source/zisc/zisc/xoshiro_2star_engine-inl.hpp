/*!
  \file xoshiro_2star_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_2STAR_ENGINE_INL_HPP
#define ZISC_XOSHIRO_2STAR_ENGINE_INL_HPP

#include "xoshiro_2star_engine.hpp"
// Standard C++ library
#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC> inline
Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::Xoshiro2StarEngine() noexcept
{
  constexpr SeedType seed = cast<SeedType>(0xc1f651c67c62c6e0ull);
  setSeed(seed);
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC> inline
Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::Xoshiro2StarEngine(
    const SeedType seed) noexcept
{
  setSeed(seed);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC> inline
auto Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::generate() noexcept
    -> ResultType
{
  const ResultType result = rotateLeft<7>(state_[kA] * 5u) * 9u;
  const SeedType t = state_[1] << kB;

  state_[2] ^= state_[0];
  state_[3] ^= state_[1];
  state_[1] ^= state_[2];
  state_[0] ^= state_[3];

  state_[2] ^= t;

  state_[3] = rotateLeft<kC>(state_[3]);

  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC> inline
constexpr std::size_t Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::getPeriodPow2()
    noexcept
{
  constexpr std::size_t period_pow2 = sizeof(SeedType) * 8 * 4;
  return period_pow2;
}

/*!
  \details No detailed description

  \tparam UInteger No description.
  \param [in] sample No description.
  \return No description
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC>
template <typename UInteger> inline
constexpr bool Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::isEndOfPeriod(
    const UInteger sample) noexcept
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
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC> inline
void Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::setSeed(const SeedType seed)
    noexcept
{
  state_.fill(seed);
}

/*!
  \details No detailed description

  \tparam k No description.
  \param [in] x No description.
  \return No description
  */
template <typename Seed, typename Result, uint8b kA, uint8b kB, uint8b kC>
template <Seed k> inline
auto Xoshiro2StarEngine<Seed, Result, kA, kB, kC>::rotateLeft(const SeedType x)
    noexcept -> ResultType
{
  constexpr SeedType bit_size = sizeof(SeedType) * 8;
  const ResultType result = cast<ResultType>((x << k) | (x >> (bit_size - k)));
  return result;
}

} // namespace zisc

#endif // ZISC_XOSHIRO_2STAR_ENGINE_INL_HPP
