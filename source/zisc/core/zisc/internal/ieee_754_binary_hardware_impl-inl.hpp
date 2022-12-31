/*!
  \file ieee_754_binary_hardware_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP
#define ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP

#include "ieee_754_binary_hardware_impl.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/bit.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> template <std::floating_point Float> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>::Ieee754BinaryHardwareImpl(const Float value) noexcept :
    data_{static_cast<typename DataType::T>(value)}
{
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>::Ieee754BinaryHardwareImpl(const BitT bits) noexcept :
    data_{zisc::bit_cast<DataType>(bits)}
{
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <Ieee754BinaryFormat kDstFormat, Ieee754RoundingMode kRMode> inline
constexpr auto Ieee754BinaryHardwareImpl<kFormat>::convertTo() const noexcept
    -> Ieee754BinaryHardwareImpl<kDstFormat>
{
  using DstBinaryT = Ieee754BinaryHardwareImpl<kDstFormat>;
  using DataT = typename DstBinaryT::DataType;
  const DstBinaryT dst{static_cast<typename DataT::T>(data().value_)};
  return dst;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat>
template <std::floating_point Float, Ieee754RoundingMode kRMode> inline
constexpr Float Ieee754BinaryHardwareImpl<kFormat>::convertTo() const noexcept
{
  return static_cast<Float>(data().value_);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinaryHardwareImpl<kFormat>::data() const noexcept -> DataType
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754BinaryHardwareImpl<kFormat>::bitSize() noexcept
{
  constexpr std::size_t bit_size = kBinFormatBitSize<kFormat>;
  static_assert(bit_size == 8 * sizeof(Ieee754BinaryHardwareImpl),
                "The size of underlying data doesn't match with the format.");
  return bit_size;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryFormat Ieee754BinaryHardwareImpl<kFormat>::format() noexcept
{
  return kFormat;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr void Ieee754BinaryHardwareImpl<kFormat>::setBits(const BitT bits) noexcept
{
  data_ = zisc::bit_cast<DataType>(bits);
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>::Ieee754BinaryHardwareImpl(const typename DataType::Half& value) noexcept :
    data_{static_cast<typename DataType::T>(value)}
{
}

} // namespace zisc

#endif // ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP
