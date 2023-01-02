/*!
  \file hash_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, HashValue T> inline
constexpr auto HashEngine<HashClass, T>::hash(
    const char* seed) noexcept -> ValueT
{
  const std::string_view s{seed};
  const ValueT result = hash(s);
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, HashValue T> inline
constexpr auto HashEngine<HashClass, T>::hash(
    const std::string_view seed) noexcept -> ValueT
{
  const ValueT result = hash(seed.data(), seed.size());
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  \param [in] n No description.
  \return No description
  */
template <typename HashClass, HashValue T>
template <HashKeyElement Int8> inline
constexpr auto HashEngine<HashClass, T>::hash(
    const Int8* seed,
    const std::size_t n) noexcept -> ValueT
{
  const ValueT result = hashValue(seed, n);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] seed No description.
  \return No description
  */
template <typename HashClass, HashValue T>
template <UnsignedInteger Integer> inline
constexpr auto HashEngine<HashClass, T>::hash(
    const Integer seed) noexcept -> ValueT
{
  // Make a seed array
  std::array<uint8b, sizeof(Integer)> seed_array{};
  for (std::size_t i = 0; i < seed_array.size(); ++i)
    seed_array[i] = cast<uint8b>(seed >> (8 * i));
  // Hash the seed
  const ValueT result = hash(seed_array.data(), seed_array.size());
  return result;
}

/*!
  \details No detailed description

  \tparam Int8 No description.
  \param [in] seed No description.
  \param [in] n No description.
  \return No description
  */
template <typename HashClass, HashValue T>
template <HashKeyElement Int8> inline
constexpr auto HashEngine<HashClass, T>::hashValue(
    const Int8* seed,
    const std::size_t n) noexcept -> ValueT
{
  const ValueT result = HashT::hashValue(seed, n);
  return result;
}

} // namespace zisc

#endif // ZISC_HASH_ENGINE_INL_HPP
