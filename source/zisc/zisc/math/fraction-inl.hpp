/*!
  \file fraction-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FRACTION_INL_HPP
#define ZISC_FRACTION_INL_HPP

#include "fraction.hpp"
// Standard C++ library
#include <numeric>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::Fraction() noexcept
{
}

/*!
  \details No detailed description

  \param [in] n No description.
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::Fraction(const Int n) noexcept
    : numerator_{n}
{
}

/*!
  \details No detailed description

  \param [in] n No description.
  \param [in] d No description.
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::Fraction(const Int n, const Int d) noexcept
    : numerator_{(signbit(d) ? -n : n) / std::gcd(n, d)},
      denominator_{abs(d) / std::gcd(n, d)}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::operator-() const noexcept -> Fraction
{
  const Fraction frac{-numer(), denom()};
  return frac;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::operator+=(const Fraction& other) noexcept -> Fraction&
{
  *this = (*this + other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::operator-=(const Fraction& other) noexcept -> Fraction&
{
  *this = (*this - other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::operator*=(const Fraction& other) noexcept -> Fraction&
{
  *this = (*this * other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::operator/=(const Fraction& other) noexcept -> Fraction&
{
  *this = (*this / other);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::operator float() const noexcept
{
  const float result = toFloat<float>();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::operator double() const noexcept
{
  const double result = toFloat<double>();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int>::operator long double() const noexcept
{
  const long double result = toFloat<long double>();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::denom() noexcept -> Type&
{
  return denominator_;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::denom() const noexcept -> const Type&
{
  return denominator_;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::numer() noexcept -> Type&
{
  return numerator_;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::numer() const noexcept -> const Type&
{
  return numerator_;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> inline
constexpr auto Fraction<Int>::invert() const noexcept -> Fraction
{
  const Fraction inv_frac{denom(), numer()};
  return inv_frac;
}

/*!
  \details No detailed description

  \return No description
  */
template <SignedInteger Int> template <FloatingPoint Float> inline
constexpr Float Fraction<Int>::toFloat() const noexcept
{
  const Float value = cast<Float>(numer()) / cast<Float>(denom());
  return value;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int> operator+(const Fraction<Int>& lhs,
                                  const Fraction<Int>& rhs) noexcept
{
  const auto denom = std::lcm(lhs.denom(), rhs.denom());
  const auto numer = lhs.numer() * (denom / std::gcd(denom, lhs.denom())) +
                     rhs.numer() * (denom / std::gcd(denom, rhs.denom()));
  const Fraction<Int> result{numer, denom};
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int> operator-(const Fraction<Int>& lhs,
                                  const Fraction<Int>& rhs) noexcept
{
  const auto result = lhs + (-rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int> operator*(const Fraction<Int>& lhs,
                                  const Fraction<Int>& rhs) noexcept
{
  const auto s = std::gcd(lhs.numer(), rhs.denom());
  const auto t = std::gcd(lhs.denom(), rhs.numer());
  const auto numer = (lhs.numer() / s) * (rhs.numer() / t);
  const auto denom = (lhs.denom() / t) * (rhs.denom() / s);
  const Fraction<Int> result{numer, denom};
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr Fraction<Int> operator/(const Fraction<Int>& lhs,
                                  const Fraction<Int>& rhs) noexcept
{
  const auto result = lhs * rhs.invert();
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator==(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept
{
  const bool result = (lhs.numer() == rhs.numer()) && (lhs.denom() == rhs.denom());
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator!=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept
{
  const bool result = !(lhs == rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator<(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept
{
  const bool result = (lhs.numer() * rhs.denom()) < (lhs.denom() * rhs.numer());
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator<=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept
{
  const bool result = (lhs == rhs) || (lhs < rhs);
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator>(const Fraction<Int>& lhs,
                         const Fraction<Int>& rhs) noexcept
{
  const bool result = rhs < lhs;
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <SignedInteger Int> inline
constexpr bool operator>=(const Fraction<Int>& lhs,
                          const Fraction<Int>& rhs) noexcept
{
  const bool result = rhs <= lhs;
  return result;
}

} // namespace zisc

#endif // ZISC_FRACTION_INL_HPP
