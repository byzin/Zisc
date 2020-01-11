/*!
  \file hash_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const char* seed) noexcept
{
  const std::size_t n = getSize(seed);
  const ResultType h = hashValue(seed, n);
  return h;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const std::string_view seed) noexcept
{
  const ResultType h = hashValue(seed.data(), seed.size());
  return h;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  \param [in] n No description.
  \return No description
  */
template <typename HashClass, typename ResultType> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const uint8b* seed,
    const std::size_t n) noexcept
{
  const ResultType h = hashValue(seed, n);
  return h;
}

/*!
  \details No detailed description

  \tparam UnsignedInteger No description.
  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, typename ResultType>
template <typename UnsignedInteger> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hash(
    const UnsignedInteger seed) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer type.");
  // Make a seed array
  std::array<uint8b, sizeof(UnsignedInteger)> seed_array{};
  for (std::size_t i = 0; i < seed_array.size(); ++i)
    seed_array[i] = cast<uint8b>(seed >> (8 * i));
  // Hash the seed
  const ResultType h = hash(seed_array.data(), seed_array.size());
  return h;
}

/*!
  \details No detailed description

  \tparam Int8 No description.
  \param [in] seed No description.
  \param [in] n No description.
  \return No description
  */
template <typename HashClass, typename ResultType> template <typename Int8> inline
constexpr ResultType HashEngine<HashClass, ResultType>::hashValue(
    const Int8* seed,
    const std::size_t n) noexcept
{
  const ResultType h = HashClass::hashValue(seed, n);
  return h;
}

} // namespace zisc

#endif // ZISC_HASH_ENGINE_INL_HPP
