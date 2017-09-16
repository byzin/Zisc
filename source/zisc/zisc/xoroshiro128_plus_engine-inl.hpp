/*!
  \file xoroshiro128_plus_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOROSHIRO128_PLUS_ENGINE_INL_HPP
#define ZISC_XOROSHIRO128_PLUS_ENGINE_INL_HPP

#include "xoroshiro128_plus_engine.hpp"
// Standard C++ library
#include <cstdint>
#include <limits>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \details
 No detailed.
 */
inline
Xoroshiro128PlusEngine::Xoroshiro128PlusEngine() noexcept
{
  setSeed(0);
}

/*!
 \details
 No detailed.
 */
inline
Xoroshiro128PlusEngine::Xoroshiro128PlusEngine(const SeedType seed) noexcept
{
  setSeed(seed);
}

/*!
 \details
 No detailed.
 */
inline
Xoroshiro128PlusEngine::Xoroshiro128PlusEngine(const SeedType seed1,
                                               const SeedType seed2) noexcept
{
  setSeed(seed1, seed2);
}


/*!
 \details
 No detailed.
 */
inline
auto Xoroshiro128PlusEngine::generate() noexcept -> ResultType
{
  const ResultType s0 = seed_[0];
  ResultType s1 = seed_[1];
  const ResultType u = s0 + s1;

  s1 = s1 ^ s0;
  seed_[0] = rotateLeft<55>(s0) ^ s1 ^ (s1 << 14);
  seed_[1] = rotateLeft<36>(s1);

  return u;
}

/*!
 \details
 No detailed.
 */
inline
void Xoroshiro128PlusEngine::setSeed(const SeedType seed) noexcept
{
  seed_[0] = seed;
  seed_[1] = 0;
}

/*!
 \details
 No detailed.
 */
inline
void Xoroshiro128PlusEngine::setSeed(const SeedType seed1,
                                     const SeedType seed2) noexcept
{
  seed_[0] = seed1;
  seed_[1] = seed2;
}

/*!
  */
template <int bit> inline
auto Xoroshiro128PlusEngine::rotateLeft(const ResultType x) noexcept -> ResultType
{
  const ResultType result = (x << bit) | (x >> (64 - bit));
  return result;
}

} // namespace zisc

#endif // ZISC_XOROSHIRO128_PLUS_ENGINE_INL_HPP
