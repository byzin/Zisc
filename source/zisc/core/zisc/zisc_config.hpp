/*!
  \file zisc_config.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONFIG_HPP
#define ZISC_CONFIG_HPP

// Standard C++ library
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace zisc {

// General
using int8b = std::int8_t; //!< 8-bit signed integer type
using int16b = std::int16_t; //!< 16-bit signed integer type
using int32b = std::int32_t; //!< 32-bit signed integer type
using int64b = std::int64_t; //!< 64-bit signed integer type
using uint = unsigned int; //!< Unsigned integer type
using uint8b = std::uint8_t; //!< 8-bit unsigned integer type
using uint16b = std::uint16_t; //!< 16-bit unsigned integer type
using uint32b = std::uint32_t; //!< 32-bit unsigned integer type
using uint64b = std::uint64_t; //!< 64-bit unsigned integer type

//! Represent padding in structure
template <std::size_t kSize>
using Padding = std::aligned_storage_t<kSize, 1>;

/*!
  \brief Configuration of zisc source

  No detailed description.
  */
class Config
{
 public:
  //! Return the value of the major component of the Zisc version number
  static constexpr int versionMajor() noexcept;

  //! Return the value of the minor component of the Zisc version number
  static constexpr int versionMinor() noexcept;

  //! Return the value of the patch component of the Zisc version number
  static constexpr int versionPatch() noexcept;

  //! Return the version string of Zisc
  static constexpr std::string_view versionString() noexcept;

  //! Return the L1 cache line size in bytes
  static constexpr std::size_t l1CacheLineSize() noexcept;

  // Algorithm config

  // Atomic config
  static constexpr bool isAtomicOsSpecifiedWaitUsed() noexcept;
};

} // namespace zisc

#include "zisc/zisc_config-inl.hpp"

#endif // ZISC_CONFIG_HPP
