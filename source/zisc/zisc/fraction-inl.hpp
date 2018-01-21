/*!
  \file fraction-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FRACTION_INL_HPP
#define ZISC_FRACTION_INL_HPP

#include "fraction.hpp"
// Standard C++ library
#include <numeric>
// Zisc
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// public member function

/*!
  \details
  No detailed.
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>::Fraction() noexcept :
    numerator_{cast<SignedInteger>(0)},
    denominator_{cast<SignedInteger>(1)}
{
}

/*!
  \details
  No detailed.
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>::Fraction(const SignedInteger n) noexcept
    : numerator_{n},
      denominator_{cast<SignedInteger>(1)}
{
}

/*!
  \details
  No detailed.
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>::Fraction(const SignedInteger n, 
                                            const SignedInteger d) noexcept
    : numerator_{(isNegative(d) ? -n : n) / std::gcd(n, d)},
      denominator_{abs(d) / std::gcd(n, d)}
{
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger> Fraction<SignedInteger>::operator-() const noexcept
{
  return Fraction{-numerator(), denominator()};
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>& Fraction<SignedInteger>::operator+=(
    const Fraction& other) noexcept
{
  return *this = (*this + other);
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>& Fraction<SignedInteger>::operator-=(
    const Fraction& other) noexcept
{
  return *this = (*this - other);
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>& Fraction<SignedInteger>::operator*=(
    const Fraction& other) noexcept
{
  return *this = (*this * other);
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger>& Fraction<SignedInteger>::operator/=(
    const Fraction& other) noexcept
{
  return *this = (*this / other);
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr SignedInteger& Fraction<SignedInteger>::denominator() noexcept
{
  return denominator_;
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr const SignedInteger& Fraction<SignedInteger>::denominator() const noexcept
{
  return denominator_;
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr SignedInteger& Fraction<SignedInteger>::numerator() noexcept
{
  return numerator_;
}
/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr const SignedInteger& Fraction<SignedInteger>::numerator() const noexcept
{
  return numerator_;
}

/*!
  */
template <typename SignedInteger> inline
constexpr auto Fraction<SignedInteger>::invert() const noexcept -> Fraction
{
  return Fraction{denominator(), numerator()};
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> template <typename Float> inline
constexpr Float Fraction<SignedInteger>::toFloat() const noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
  return cast<Float>(numerator()) / cast<Float>(denominator());
}

/*!
  */
template <typename SignedInteger> inline
constexpr SignedInteger Fraction<SignedInteger>::abs(const SignedInteger n) noexcept
{
  return isNegative(n) ? -n : n;
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger> operator+(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept
{
  const auto denominator = std::lcm(lhs.denominator(), rhs.denominator());
  const auto numerator =
      lhs.numerator() * (denominator / std::gcd(denominator, lhs.denominator())) +
      rhs.numerator() * (denominator / std::gcd(denominator, rhs.denominator()));
  return Fraction<SignedInteger>{numerator, denominator};
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger> operator-(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept
{
  return lhs + (-rhs);
}

/*
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger> operator*(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept
{
  const auto s = std::gcd(lhs.numerator(), rhs.denominator());
  const auto t = std::gcd(lhs.denominator(), rhs.numerator());
  const auto numerator = (lhs.numerator() / s) * (rhs.numerator() / t);
  const auto denominator = (lhs.denominator() / t) * (rhs.denominator() / s);
  return Fraction<SignedInteger>{numerator, denominator};
}

/*!
  */
template <typename SignedInteger> inline
constexpr Fraction<SignedInteger> operator/(
    const Fraction<SignedInteger>& lhs,
    const Fraction<SignedInteger>& rhs) noexcept
{
  return lhs * rhs.invert();
}

/*!
 */
template <typename SignedInteger> inline
constexpr bool operator==(const Fraction<SignedInteger>& lhs,
                          const Fraction<SignedInteger>& rhs) noexcept
{
  return (lhs.numerator() == rhs.numerator()) &&
         (lhs.denominator() == rhs.denominator());
}

/*!
 */
template <typename SignedInteger> inline
constexpr bool operator!=(const Fraction<SignedInteger>& lhs,
                          const Fraction<SignedInteger>& rhs) noexcept
{
  return !(lhs == rhs);
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr bool operator<(const Fraction<SignedInteger>& lhs,
                         const Fraction<SignedInteger>& rhs) noexcept
{
  return (lhs.numerator() * rhs.denominator()) < 
         (lhs.denominator() * rhs.numerator());
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr bool operator<=(const Fraction<SignedInteger>& lhs,
                          const Fraction<SignedInteger>& rhs) noexcept
{
  return (lhs == rhs) || (lhs < rhs);
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr bool operator>(const Fraction<SignedInteger>& lhs,
                         const Fraction<SignedInteger>& rhs) noexcept
{
  return rhs < lhs;
}

/*!
 \details
 No detailed.
 */
template <typename SignedInteger> inline
constexpr bool operator>=(const Fraction<SignedInteger>& lhs,
                          const Fraction<SignedInteger>& rhs) noexcept
{
  return rhs <= lhs;
}

} // namespace zisc

#endif // ZISC_FRACTION_INL_HPP
