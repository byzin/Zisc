/*!
  \file hash_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam HashClass No description.
  \tparam ResultType No description.
  */
template <typename HashClass, typename ResultType>
class HashEngine
{
  static_assert(std::is_same_v<uint32b, ResultType> ||
                std::is_same_v<uint64b, ResultType>,
                "ResultType isn't supported.");
 public:
  //! Compute a hash value
  static constexpr ResultType hash(const char* seed) noexcept;

  //! Compute a hash value
  static constexpr ResultType hash(const std::string_view seed) noexcept;

  //! Compute a hash value
  static constexpr ResultType hash(const uint8b* seed,
                                   const std::size_t n) noexcept;

  //! Compute a hash value
  template <typename UnsignedInteger>
  static constexpr ResultType hash(const UnsignedInteger seed) noexcept;

 private:
  //! Implementation of the hash function
  template <typename Int8>
  static constexpr ResultType hashValue(const Int8* seed,
                                        const std::size_t n) noexcept;
};

} // namespace zisc

#include "hash_engine-inl.hpp"

#endif // ZISC_HASH_ENGINE_HPP
