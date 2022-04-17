/*!
  \file ieee_754_binary_hardware_impl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP
#define ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief The hardware implementation of IEEE 754 binary interchange format

  No detailed description.

  \tparam kFormat Encoding format used in the interchange
  */
template <Ieee754BinaryFormat kFormat>
class Ieee754BinaryHardwareImpl
{
 public:
  //! Bit representation type
  using BitT = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   uint16b,
               std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, uint32b,
                                                                           uint64b>>;
 /*!
    \brief Internal data type

    Define the data type struct because direct passing and returning half is prohibited
    */
  struct DataType
  {
#if defined(Z_CLANG)
    using Half = __fp16;
#define ZISC_HAS_HARDWARE_HALF_IMPL 1
#else
    using Half = uint16b;
#define ZISC_HAS_HARDWARE_HALF_IMPL 0
#endif
    using T = std::conditional_t<kFormat == Ieee754BinaryFormat::kHalf,   Half,
              std::conditional_t<kFormat == Ieee754BinaryFormat::kSingle, float,
                                                                          double>>;
    T value_;
  };


  //! Initialize a value with 0
  constexpr Ieee754BinaryHardwareImpl() noexcept;

  //! Initialize a value with the given floating point value
  template <std::floating_point Float>
  constexpr Ieee754BinaryHardwareImpl(const Float value) noexcept;

  //! Initialize a value with the given bits data
  constexpr Ieee754BinaryHardwareImpl(const BitT bits) noexcept;


  //! Convert to other format
  template <Ieee754BinaryFormat kDstFormat,
            Ieee754RoundingMode kRMode = kDefaultIeee754RoundingMode>
  constexpr Ieee754BinaryHardwareImpl<kDstFormat> convertTo() const noexcept;

  //! Convert to a floating point
  template <std::floating_point Float,
            Ieee754RoundingMode kRMode = kDefaultIeee754RoundingMode>
  constexpr Float convertTo() const noexcept;

  //! Return the underlying floating point value
  constexpr DataType data() const noexcept;

  //! Return the bit length used in the interchange encoding
  static constexpr std::size_t bitSize() noexcept;

  //! Return the format
  static constexpr Ieee754BinaryFormat format() noexcept;

  //! Set the data to the given bits
  constexpr void setBits(const BitT bits) noexcept;

 private:
  //! Initialize a value with the given floating point value
  constexpr Ieee754BinaryHardwareImpl(const typename DataType::Half& value) noexcept;


  DataType data_;
};

} // namespace zisc

#include "ieee_754_binary_hardware_impl-inl.hpp"

#endif // ZISC_IEEE_754_BINARY_HARDWARE_IMPL_HPP
