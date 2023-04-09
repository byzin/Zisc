/*!
  \file ieee_754_binary_hardware_impl-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP
#define ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP

#include "ieee_754_binary_hardware_impl.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
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
    data_{static_cast<DataT>(value)}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>::Ieee754BinaryHardwareImpl(const HalfT value) noexcept :
    data_{static_cast<DataT>(value)}
{
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>::Ieee754BinaryHardwareImpl(const BitT bits) noexcept :
    data_{zisc::bit_cast<DataT>(bits)}
{
}

/*!
  \details No detailed description

  \tparam kDstFormat No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <Ieee754BinaryFormat kDstFormat> inline
constexpr auto Ieee754BinaryHardwareImpl<kFormat>::convertTo() const noexcept
    -> Ieee754BinaryHardwareImpl<kDstFormat>
{
  using DstBinaryT = Ieee754BinaryHardwareImpl<kDstFormat>;
  using DstDataT = typename DstBinaryT::DataT;
  const DstBinaryT dst{static_cast<DstDataT>(data())};
  return dst;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kRMode No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> template <std::floating_point Float> inline
constexpr Float Ieee754BinaryHardwareImpl<kFormat>::convertTo() const noexcept
{
  return static_cast<Float>(data());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinaryHardwareImpl<kFormat>::data() noexcept -> DataT&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754BinaryHardwareImpl<kFormat>::data() const noexcept -> const DataT&
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

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::float_round_style Ieee754BinaryHardwareImpl<kFormat>::roundStyle() noexcept
{
  if constexpr (format() == Ieee754BinaryFormat::kHalf)
    return std::float_round_style::round_to_nearest; // We assume half use round to nearest
  else
    return std::numeric_limits<DataT>::round_style;
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr void Ieee754BinaryHardwareImpl<kFormat>::setBits(const BitT bits) noexcept
{
  data_ = zisc::bit_cast<DataT>(bits);
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator+=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  lhs.data() += rhs.data();
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator-=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  lhs.data() -= rhs.data();
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator*=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  lhs.data() *= rhs.data();
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat>& operator/=(
    Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  lhs.data() /= rhs.data();
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator+(
    const Ieee754BinaryHardwareImpl<kFormat>& value) noexcept
{
  return value;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator-(
    const Ieee754BinaryHardwareImpl<kFormat>& value) noexcept
{
  const Ieee754BinaryHardwareImpl<kFormat> result{-value.data()};
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator+(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  const Ieee754BinaryHardwareImpl<kFormat> result{lhs.data() + rhs.data()};
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator-(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  const Ieee754BinaryHardwareImpl<kFormat> result{lhs.data() - rhs.data()};
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator*(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  const Ieee754BinaryHardwareImpl<kFormat> result{lhs.data() * rhs.data()};
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryHardwareImpl<kFormat> operator/(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  const Ieee754BinaryHardwareImpl<kFormat> result{lhs.data() / rhs.data()};
  return result;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator==(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() == rhs.data();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator!=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() != rhs.data();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator<(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() < rhs.data();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator<=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() <= rhs.data();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator>(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() > rhs.data();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator>=(
    const Ieee754BinaryHardwareImpl<kFormat>& lhs,
    const Ieee754BinaryHardwareImpl<kFormat>& rhs) noexcept
{
  return lhs.data() >= rhs.data();
}

} // namespace zisc

#endif // ZISC_IEEE_754_BINARY_HARDWARE_IMPL_INL_HPP
