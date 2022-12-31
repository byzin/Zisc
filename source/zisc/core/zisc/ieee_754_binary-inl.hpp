/*!
  \file ieee_754_binary-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_IEEE_754_BINARY_INL_HPP
#define ZISC_IEEE_754_BINARY_INL_HPP

#include "ieee_754_binary.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
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
constexpr bool isFinite(const Float& x) noexcept
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
constexpr bool isFinite(const Float& x) noexcept
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
constexpr bool isInf(const Float& x) noexcept
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
constexpr bool isInf(const Float& x) noexcept
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
constexpr bool isNan(const Float& x) noexcept
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
constexpr bool isNan(const Float& x) noexcept
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
constexpr bool isNormal(const Float& x) noexcept
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
constexpr bool isNormal(const Float& x) noexcept
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
constexpr bool isSubnormal(const Float& x) noexcept
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
constexpr bool isSubnormal(const Float& x) noexcept
{
  using FLimit = std::numeric_limits<Float>;
  constexpr Float zero = cast<Float>(0);
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

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::operator+() const noexcept -> Ieee754Binary
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::operator-() const noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::negateBits(bits())};
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
  const double d = cast<double>(*this);
  const auto result = cast<long double>(d);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::epsilon() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::epsilon()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::infinity() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::infinity()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isFinite() const noexcept
{
  return SoftwareImplT::isFinite(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isInf() const noexcept
{
  return SoftwareImplT::isInf(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isNan() const noexcept
{
  return SoftwareImplT::isNan(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isNormal() const noexcept
{
  return SoftwareImplT::isNormal(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isSubnormal() const noexcept
{
  return SoftwareImplT::isSubnormal(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool Ieee754Binary<kFormat>::isZero() const noexcept
{
  return SoftwareImplT::isZero(bits());
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::max() noexcept -> Ieee754Binary
{
  return Ieee754Binary{(SoftwareImplT::max)()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::min() noexcept -> Ieee754Binary
{
  return Ieee754Binary{(SoftwareImplT::min)()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::one() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::one()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::quietNan() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::quietNan()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::roundError() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::roundError()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::signalingNan() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::signalingNan()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::zero() noexcept -> Ieee754Binary
{
  return Ieee754Binary{SoftwareImplT::zero()};
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::bits() const noexcept -> BitT
{
  const auto data = impl().data();
  using DataT = std::remove_cvref_t<decltype(data)>;
  static_assert(std::is_trivially_copyable_v<DataT>,
                "DataType of the impl isn't trivially copyable.");
  if constexpr (std::is_same_v<BitT, DataT>)
    return data;
  else
    return bit_cast<BitT>(data);
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::bitSize() noexcept
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
constexpr std::size_t Ieee754Binary<kFormat>::exponentBias() noexcept
{
  return SoftwareImplT::exponentBias();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::exponentBitMask() noexcept -> BitT
{
  return SoftwareImplT::exponentBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::exponentBitSize() noexcept
{
  return SoftwareImplT::exponentBitSize();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr Ieee754BinaryFormat Ieee754Binary<kFormat>::format() noexcept
{
  return ImplT::format();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::implicitBit() noexcept -> BitT
{
  return SoftwareImplT::implicitBit();
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
  return SoftwareImplT::signBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr auto Ieee754Binary<kFormat>::significandBitMask() noexcept -> BitT
{
  return SoftwareImplT::significandBitMask();
}

/*!
  \details No detailed description

  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr std::size_t Ieee754Binary<kFormat>::significandBitSize() noexcept
{
  return SoftwareImplT::significandBitSize();
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
constexpr auto Ieee754Binary<kFormat>::impl() const noexcept -> const ImplT&
{
  return impl_;
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

  \tparam kFormat No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <Ieee754BinaryFormat kFormat> inline
constexpr bool operator==(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = !(isNan(lhs) || isNan(rhs)) &&
                      ((lhs.isZero() && rhs.isZero()) || (lhs.bits() == rhs.bits()));
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
constexpr bool operator!=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = !(lhs == rhs);
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
constexpr bool operator<(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
{
  bool result = !(isNan(lhs) || isNan(rhs) || (lhs.isZero() && rhs.isZero()));
  if (result) {
    using BinaryT = Ieee754Binary<kFormat>;
    using BitT = typename BinaryT::BitT;
    using SignedT = std::make_signed_t<BitT>;
    const auto get_signed = [](const BitT u) noexcept
    {
      constexpr BitT m = BinaryT::signBitMask();
      SignedT s = cast<SignedT>(u & ~m);
      s = ((u & m) == 0) ? s : -s;
      return s;
    };
    const SignedT sl = get_signed(lhs.bits());
    const SignedT sr = get_signed(rhs.bits());
    result = sl < sr;
  }
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
constexpr bool operator<=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (lhs == rhs) || (lhs < rhs);
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
constexpr bool operator>(const Ieee754Binary<kFormat>& lhs,
                         const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (rhs < lhs);
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
constexpr bool operator>=(const Ieee754Binary<kFormat>& lhs,
                          const Ieee754Binary<kFormat>& rhs) noexcept
{
  const bool result = (rhs <= lhs);
  return result;
}

} // namespace zisc

// Specialize std::numeric_limits

namespace std {

template <zisc::Ieee754BinaryFormat kFormat>
class numeric_limits<zisc::Ieee754Binary<kFormat>>
{
 private:
  using BinaryType = zisc::Ieee754Binary<kFormat>;

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
  static constexpr std::float_round_style round_style = std::round_to_nearest;

  //! Identify the IEC 559/IEEE 754 floating-point type
  static constexpr bool is_iec559 =
      (kFormat == zisc::Ieee754BinaryFormat::kSingle) ||
      (kFormat == zisc::Ieee754BinaryFormat::kDouble);

  //! Identify types that represent a finite set of values
  static constexpr bool is_bounded = false;

  //! Identify types that handle overflows with modulo arithmetic
  static constexpr bool is_modulo = false;

  //! Number of radix digits that can be represented without change
  static constexpr int digits = zisc::cast<int>(BinaryType::significandBitSize() + 1);

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
  static constexpr BinaryType min()
  {
    return (BinaryType::min)();
  }

  //! Return the lowest finite value
  static constexpr BinaryType lowest()
  {
    return -(BinaryType::max)();
  }

  //! Return the largest finite value
  static constexpr BinaryType max()
  {
    return +(BinaryType::max)();
  }

  //! Return the difference between 1.0 and the next representable value
  static constexpr BinaryType epsilon()
  {
    return BinaryType::epsilon();
  }

  //! Return the maximum rounding error
  static constexpr BinaryType round_error()
  {
    return BinaryType::roundError();
  }

  //! Return the positive infinity value
  static constexpr BinaryType infinity()
  {
    return BinaryType::infinity();
  }

  //! Return a quiet NaN value
  static constexpr BinaryType quiet_NaN()
  {
    return BinaryType::quietNan();
  }

  //! Return a signaling NaN value
  static constexpr BinaryType signaling_NaN()
  {
    return BinaryType::signalingNan();
  }

  //! Return the smallest positive subnormal value
  static constexpr BinaryType denorm_min()
  {
    return BinaryType{0b1u};
  }
};

} // namespace std

#endif // ZISC_IEEE_754_BINARY_INL_HPP
