/*!
  \file hash_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HASH_ENGINE_INL_HPP
#define ZISC_HASH_ENGINE_INL_HPP

#include "hash_engine.hpp"
// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <type_traits>
// Zisc
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const char* seed) noexcept
{
  const std::string_view s{seed};
  return hash(s);
}

/*!
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const std::string_view seed) noexcept
{
  return hashValue(seed.data(), seed.size());
}

/*!
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const uint8* seed,
    const std::size_t n) noexcept
{
  return hashValue(seed, n);
}

/*!
  */
template <typename HashClass, typename ResultType>
template <typename UnsignedInteger> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const UnsignedInteger seed) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer type.");
  // Make a seed array
  std::array<uint8, sizeof(UnsignedInteger)> seed_array{};
  for (std::size_t i = 0; i < seed_array.size(); ++i)
    seed_array[i] = cast<uint8>(seed >> (8 * i));
  // Hash the seed
  return hash(seed_array.data(), seed_array.size());
}

/*!
  */
template <typename HashClass, typename ResultType> template <typename Int8> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hashValue(
    const Int8* seed,
    const std::size_t n) noexcept
{
  return HashClass::hashValue(seed, n);
}

} // namespace zisc

#endif // ZISC_HASH_ENGINE_INL_HPP
