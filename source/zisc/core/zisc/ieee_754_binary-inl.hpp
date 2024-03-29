/*!
  \file ieee_754_binary-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_INL_HPP
#define ZISC_IEEE_754_BINARY_INL_HPP

#include "ieee_754_binary.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "algorithm.hpp"
#include "bit.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"
#include "internal/ieee_754_binary_software_impl.hpp"

namespace zisc {

// Type check
static_assert(std::is_standard_layout_v<Binary16>);
static_assert(std::is_trivial_v<Binary16>);
static_assert(std::is_standard_layout_v<Binary32>);
static_assert(std::is_trivial_v<Binary32>);
static_assert(std::is_standard_layout_v<Binary64>);
static_assert(std::is_trivial_v<Binary64>);

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryClass Float> inline
constexpr auto isFinite(const Float& x) noexcept -> bool
{
  return x.isFinite();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto isFinite(const Float& x) noexcept -> bool
{
  using FLimit = std::numeric_limits<Float>;
  const bool result = (FLimit::lowest() <= x) && (x <= (FLimit::max)());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryClass Float> inline
constexpr auto isInf(const Float& x) noexcept -> bool
{
  return x.isInf();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto isInf(const Float& x) noexcept -> bool
{
  using FLimit = std::numeric_limits<Float>;
  const bool result = (x == -FLimit::infinity()) || (x == FLimit::infinity());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryClass Float> inline
constexpr auto isNan(const Float& x) noexcept -> bool
{
  return x.isNan();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto isNan(const Float& x) noexcept -> bool
{
  const bool result = !(isFinite(x) || isInf(x));
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryClass Float> inline
constexpr auto isNormal(const Float& x) noexcept -> bool
{
  return x.isNormal();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto isNormal(const Float& x) noexcept -> bool
{
  using FLimit = std::numeric_limits<Float>;
  const Float d = abs(x);
  const bool result = ((FLimit::min)() <= d) && (d <= (FLimit::max)());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <Ieee754BinaryClass Float> inline
constexpr auto isSubnormal(const Float& x) noexcept -> bool
{
  return x.isSubnormal();
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto isSubnormal(const Float& x) noexcept -> bool
{
  using FLimit = std::numeric_limits<Float>;
  constexpr auto zero = static_cast<Float>(0);
  const Float d = abs(x);
  const bool result = (zero < d) && (d < (FLimit::min)());
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> template <std::floating_point Float> inline
constexpr Ieee754Binary<kFormat>::Ieee754Binary(const Float value) noexcept :
    impl_{value}
{
}

/*!
  \details No detailed description

  \param [in] bits No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::Ieee754Binary(const BitT bits) noexcept :
    impl_{bits}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::Ieee754Binary(const ImplT& impl) noexcept :
    impl_{impl}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator Binary16() const noexcept
{
  constexpr Ieee754BinaryFormat form = Ieee754BinaryFormat::kHalf;
  const Binary16 result = (kFormat == form)
      ? *this
      : Binary16{impl().template convertTo<form>()};
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator Binary32() const noexcept
{
  constexpr Ieee754BinaryFormat form = Ieee754BinaryFormat::kSingle;
  const Binary32 result = (kFormat == form)
      ? *this
      : Binary32{impl().template convertTo<form>()};
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator Binary64() const noexcept
{
  constexpr Ieee754BinaryFormat form = Ieee754BinaryFormat::kDouble;
  const Binary64 result = (kFormat == form)
      ? *this
      : Binary64{impl().template convertTo<form>()};
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator float() const noexcept
{
  return impl().template convertTo<float>();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator double() const noexcept
{
  return impl().template convertTo<double>();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754Binary<kFormat>::operator long double() const noexcept
{
  const auto d = static_cast<double>(*this);
  const auto result = static_cast<long double>(d);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::epsilon() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::epsilon()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::infinity() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::infinity()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isFinite() const noexcept -> bool
{
  return SoftImplT::isFinite(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isInf() const noexcept -> bool
{
  return SoftImplT::isInf(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isNan() const noexcept -> bool
{
  return SoftImplT::isNan(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isNormal() const noexcept -> bool
{
  return SoftImplT::isNormal(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isSubnormal() const noexcept -> bool
{
  return SoftImplT::isSubnormal(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::isZero() const noexcept -> bool
{
  return SoftImplT::isZero(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::max() noexcept -> Ieee754Binary
{
  return Ieee754Binary{(SoftImplT::max)()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::min() noexcept -> Ieee754Binary
{
  return Ieee754Binary{(SoftImplT::min)()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::one() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::one()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::quietNan() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::quietNan()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::roundError() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::roundError()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::roundStyle() noexcept -> std::float_round_style
{
  return ImplT::roundStyle();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::signalingNan() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::signalingNan()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::zero() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftImplT::zero()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::bits() const noexcept -> BitT
{
  return bit_cast<BitT>(impl());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::bitSize() noexcept -> std::size_t
{
  constexpr std::size_t bit_size = ImplT::bitSize();
  static_assert(bit_size == sizeof(Ieee754Binary),
                "The size of underlying data doesn't match with the format.");
  return bit_size;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::exponentBias() noexcept -> std::size_t
{
  return SoftImplT::exponentBias();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::exponentBitMask() noexcept -> BitT
{
  return SoftImplT::exponentBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::exponentBitSize() noexcept -> std::size_t
{
  return SoftImplT::exponentBitSize();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::format() noexcept -> Ieee754BinaryFormat
{
  return ImplT::format();
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getBiasedExponent(const BitT bits) noexcept -> BitT
{
  return SoftImplT::getBiasedExponent(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getExponent(const BitT bits) noexcept -> int
{
  return SoftImplT::getExponent(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getExponentBits(const BitT bits) noexcept -> BitT
{
  return SoftImplT::getExponentBits(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSignBit(const BitT bits) noexcept -> BitT
{
  return SoftImplT::getSignBit(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSigned(const BitT bits) noexcept -> std::make_signed_t<BitT>
{
  return SoftImplT::getSigned(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSignificandBits(const BitT bits) noexcept -> BitT
{
  return SoftImplT::getSignificandBits(bits);
}

/*!
  \details No detailed description

  \param [in] bits No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::getSignificandExponentBits(const BitT bits) noexcept -> BitT
{
  return SoftImplT::getSignificandExponentBits(bits);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::impl() noexcept -> ImplT&
{
  return impl_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::impl() const noexcept -> const ImplT&
{
  return impl_;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::implicitBit() noexcept -> BitT
{
  return SoftImplT::implicitBit();
}

/*!
  \details No detailed description

  \param [in] data No description.
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr void Ieee754Binary<kFormat>::setBits(const BitT data) noexcept
{
  impl().setBits(data);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::signBitMask() noexcept -> BitT
{
  return SoftImplT::signBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandBitMask() noexcept -> BitT
{
  return SoftImplT::significandBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandBitSize() noexcept -> std::size_t
{
  return SoftImplT::significandBitSize();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandExponentBitMask() noexcept -> BitT
{
  return SoftImplT::significandExponentBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandExponentBitSize() noexcept -> std::size_t
{
  return SoftImplT::significandExponentBitSize();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in,out] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator+=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&
{
  lhs.impl() += rhs.impl();
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
constexpr auto operator-=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&
{
  lhs.impl() -= rhs.impl();
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
constexpr auto operator*=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&
{
  lhs.impl() *= rhs.impl();
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
constexpr auto operator/=(Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>&
{
  lhs.impl() /= rhs.impl();
  return lhs;
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator+(const Ieee754Binary<kFormat>& value) noexcept
    -> Ieee754Binary<kFormat>
{
  return {+value.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] value No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator-(const Ieee754Binary<kFormat>& value) noexcept
    -> Ieee754Binary<kFormat>
{
  return {-value.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator+(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>
{
  return {lhs.impl() + rhs.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator-(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>
{
  return {lhs.impl() - rhs.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator*(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>
{
  return {lhs.impl() * rhs.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator/(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
    -> Ieee754Binary<kFormat>
{
  return {lhs.impl() / rhs.impl()};
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator==(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() == rhs.impl();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator!=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() != rhs.impl();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator<(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() < rhs.impl();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator<=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() <= rhs.impl();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator>(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() > rhs.impl();
}

/*!
  \details No detailed description

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto operator>=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept -> bool
{
  return lhs.impl() >= rhs.impl();
}

} // namespace zisc

// Specialize std::numeric_limits

namespace std {

template <zisc::Ieee754BinaryFormat kFormat>
class numeric_limits<zisc::Ieee754Binary<kFormat>>
{
 private:
  using BinaryT = zisc::Ieee754Binary<kFormat>;

 public:
  //! Identify types for which std::numeric_limits is specialized
  static constexpr bool is_specialized = true;

  //! Identify signed types
  static constexpr bool is_signed = true;

  //! Identify integer types
  static constexpr bool is_integer = false;

  //! Identify exact types
  static constexpr bool is_exact = false;

  //! Identify floating-point types that can represent the special value "positive infinity"
  static constexpr bool has_infinity = true;

  //! Identify floating-point types that can represent the special value "quiet not-a-number"
  static constexpr bool has_quiet_NaN = true;

  //! Identify floating-point types that can represent the special value "signalig not-a-number"
  static constexpr bool has_signaling_NaN = true;

  //! Identify the denormalization style used by the floating-point type
  static constexpr bool has_denorm = true;

  //!
  static constexpr bool has_denorm_loss = false; 

  //! Identify the rounding style used by the type
  static constexpr std::float_round_style round_style = BinaryT::roundStyle();

  //! Identify the IEC 559/IEEE 754 floating-point type
  static constexpr bool is_iec559 =
      (kFormat == zisc::Ieee754BinaryFormat::kSingle) ||
      (kFormat == zisc::Ieee754BinaryFormat::kDouble);

  //! Identify types that represent a finite set of values
  static constexpr bool is_bounded = false;

  //! Identify types that handle overflows with modulo arithmetic
  static constexpr bool is_modulo = false;

  //! Number of radix digits that can be represented without change
  static constexpr int digits = zisc::cast<int>(BinaryT::significandBitSize() + 1);

  //! Number of base-10 digits that can be represented by the type
  static constexpr int digits10 = 
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 3 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::digits10
          : std::numeric_limits<double>::digits10;

  //! Number of base-10 digits necessary to differentiate all values of this type
  static constexpr int max_digits10 = 
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 5 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_digits10
          : std::numeric_limits<double>::max_digits10;

  //! The radix used by the representation
  static constexpr int radix = 2;

  //! One more than the smallest negative power of the radix
  static constexpr int min_exponent =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? -13 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent
          : std::numeric_limits<double>::min_exponent;

  //! The smallest negative power of ten
  static constexpr int min_exponent10 =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? -4 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::min_exponent10
          : std::numeric_limits<double>::min_exponent10;

  //! One more than the largest integer power of the radix
  static constexpr int max_exponent =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 16 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent
          : std::numeric_limits<double>::max_exponent;

  //! The largest integer power of ten
  static constexpr int max_exponent10 =
      (kFormat == zisc::Ieee754BinaryFormat::kHalf)
          ? 4 :
      (kFormat == zisc::Ieee754BinaryFormat::kSingle)
          ? std::numeric_limits<float>::max_exponent10
          : std::numeric_limits<double>::max_exponent10;

  //! Identify types which can cause arithmetic operations to trap
  static constexpr bool traps = false;

  //! Identify floating-point types that detect tinyness before rounding
  static constexpr bool tinyness_before = false;

  //! Return the smallest finite value
  static constexpr auto min() -> BinaryT
  {
    return (BinaryT::min)();
  }

  //! Return the lowest finite value
  static constexpr auto lowest() -> BinaryT
  {
    return -(BinaryT::max)();
  }

  //! Return the largest finite value
  static constexpr auto max() -> BinaryT
  {
    return +(BinaryT::max)();
  }

  //! Return the difference between 1.0 and the next representable value
  static constexpr auto epsilon() -> BinaryT
  {
    return BinaryT::epsilon();
  }

  //! Return the maximum rounding error
  static constexpr auto round_error() -> BinaryT
  {
    return BinaryT::roundError();
  }

  //! Return the positive infinity value
  static constexpr auto infinity() -> BinaryT
  {
    return BinaryT::infinity();
  }

  //! Return a quiet NaN value
  static constexpr auto quiet_NaN() -> BinaryT
  {
    return BinaryT::quietNan();
  }

  //! Return a signaling NaN value
  static constexpr auto signaling_NaN() -> BinaryT
  {
    return BinaryT::signalingNan();
  }

  //! Return the smallest positive subnormal value
  static constexpr auto denorm_min() -> BinaryT
  {
    return BinaryT{0b1U};
  }
};

} // namespace std

#endif // ZISC_IEEE_754_BINARY_INL_HPP
