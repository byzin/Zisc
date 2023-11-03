/*!
  \file hash_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_HASH_ENGINE_HPP
#define ZISC_HASH_ENGINE_HPP

// Standard C++ library
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

//! Specify a type is a byte integer
template <typename Type>
concept HashKeyElement = Integer<Type> && (sizeof(Type) == 1);

//! Specify a type is 4 or 8 bytes unsigned integer
template <typename Type>
concept HashValue = UnsignedInteger<Type> && (sizeof(Type) == 4 || sizeof(Type) == 8);

/*!
  \brief No brief description

  No detailed description.

  \tparam HashClass No description.
  \tparam T No description.
  */
template <typename HashClass, HashValue T>
class HashEngine
{
 public:
  using HashT = HashClass;
  using ValueT = T;


  //! Compute a hash value
  static constexpr auto hash(const char* seed) noexcept -> ValueT;

  //! Compute a hash value
  static constexpr auto hash(const std::string_view seed) noexcept -> ValueT;

  //! Compute a hash value
  template <HashKeyElement Int8>
  static constexpr auto hash(const Int8* seed, const std::size_t n) noexcept -> ValueT;

  //! Compute a hash value
  template <UnsignedInteger Integer>
  static constexpr auto hash(const Integer seed) noexcept -> ValueT;

 private:
  //! Implementation of the hash function
  template <HashKeyElement Int8>
  static constexpr auto hashValue(const Int8* seed,
                                  const std::size_t n) noexcept -> ValueT;
};

} // namespace zisc

#include "hash_engine-inl.hpp"

#endif // ZISC_HASH_ENGINE_HPP
