/*!
  \file hash_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
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
  \tparam ValueT No description.
  */
template <typename HashClass, HashValue ValueT>
class HashEngine
{
 public:
  using HashType = HashClass;
  using ValueType = ValueT;


  //! Compute a hash value
  static constexpr ValueType hash(const char* seed) noexcept;

  //! Compute a hash value
  static constexpr ValueType hash(const std::string_view seed) noexcept;

  //! Compute a hash value
  template <HashKeyElement Int8>
  static constexpr ValueType hash(const Int8* seed, const std::size_t n) noexcept;

  //! Compute a hash value
  template <UnsignedInteger Integer>
  static constexpr ValueType hash(const Integer seed) noexcept;

 private:
  //! Implementation of the hash function
  template <HashKeyElement Int8>
  static constexpr ValueType hashValue(const Int8* seed,
                                       const std::size_t n) noexcept;
};

} // namespace zisc

#include "hash_engine-inl.hpp"

#endif // ZISC_HASH_ENGINE_HPP
