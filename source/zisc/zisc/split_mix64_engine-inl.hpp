/*!
  \file split_mix64_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPLIT_MIX64_ENGINE_INL_HPP
#define ZISC_SPLIT_MIX64_ENGINE_INL_HPP

#include "split_mix64_engine.hpp"
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
SplitMix64Engine::SplitMix64Engine() noexcept
{
  setSeed(0);
}

/*!
 \details
 No detailed.
 */
inline
SplitMix64Engine::SplitMix64Engine(const SeedType seed) noexcept
{
  setSeed(seed);
}

/*!
 \details
 No detailed.
 */
inline
auto SplitMix64Engine::generate() noexcept -> ResultType
{
  constexpr ResultType v1 = cast<ResultType>(0x9E3779B97F4A7C15ull);
  constexpr ResultType v2 = cast<ResultType>(0xBF58476D1CE4E5B9ull);
  constexpr ResultType v3 = cast<ResultType>(0x94D049BB133111EBull);
  seed_ = seed_ + v1;
  ResultType u = seed_;
  u = (u ^ (u >> 30)) * v2;
  u = (u ^ (u >> 27)) * v3;
  return u ^ (u >> 31);
}

/*!
 \details
 No detailed.
 */
inline
void SplitMix64Engine::setSeed(const SeedType seed) noexcept
{
  seed_ = seed;
}

} // namespace zisc

#endif // ZISC_SPLIT_MIX64_ENGINE_INL_HPP
