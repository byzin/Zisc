/*!
  \file const_math.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONST_MATH_HPP
#define ZISC_CONST_MATH_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace constant {

//! Return circular constant
template <typename Float>
constexpr Float pi() noexcept;

// Basic operations

//! Return the absolute value
template <typename Arithmetic>
constexpr Arithmetic abs(const Arithmetic x) noexcept;

//! Check whether given two values are almost same
template <std::size_t kUlps = 4, typename Float>
constexpr bool isAlmostEqual(const Float lhs, const Float rhs) noexcept;

//! Check whether given value is almost equal to zero
template <std::size_t kUlps = 4, typename Float>
constexpr bool isAlmostEqualToZero(const Float value) noexcept;

//! Compute the greatest common divisor of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                     Integer2 n) noexcept;

//! Compute the least common multiple of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                     Integer2 n) noexcept;

//! Return a inverse number
template <typename Float>
constexpr Float invert(const Float x) noexcept;

//! Compute the remainder of the division operation x/y
template <long y, typename Integer>
constexpr Integer mod(Integer x) noexcept;

//!
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> sequence(const Integer1 m,
                                                          const Integer2 n) noexcept;

//!
template <typename Integer>
constexpr Integer factorial(const Integer x) noexcept;

// Power functions

//! Calculate the value of base raised to the power exponent
template <int kExponent, typename Arithmetic>
constexpr Arithmetic power(Arithmetic base) noexcept;

//! Calculate the value of base raised to the power exponent
template <typename Arithmetic, typename SignedInteger>
constexpr Arithmetic pow(Arithmetic base, SignedInteger exponent) noexcept;

//! Calculate the square root
template <typename Float>
constexpr Float sqrt(const Float x) noexcept;

//! Calculate the cubic root
template <typename Float>
constexpr Float cbrt(const Float x) noexcept;

// Exponential functions

//! Calculate e raised to the given power
template <typename Float>
constexpr Float exp(Float x) noexcept;

//! Calculate the natural logarithm
template <typename Float>
constexpr Float log(Float x) noexcept;

//! Calculate the binary logarithm
template <typename Float>
constexpr Float log2(const Float x) noexcept;

//! Calculate the common logarithm
template <typename Float>
constexpr Float log10(const Float x) noexcept;

// Trigonometric functions

//! Calculate sin(theta)
template <typename Float>
constexpr Float sin(const Float x) noexcept;

//! Calculate cos(theta)
template <typename Float>
constexpr Float cos(const Float x) noexcept;

//! Calculate tan(theta)
template <typename Float>
constexpr Float tan(const Float x) noexcept;

//! Calculate asin(x)
template <typename Float>
constexpr Float asin(const Float x) noexcept;

//! Calculate acos(x)
template <typename Float>
constexpr Float acos(const Float x) noexcept;

//! Calculate atan(x)
template <typename Float>
constexpr Float atan(const Float x) noexcept;

// Hyperbolic functions

//! Calculate hyperbolic sine
template <typename Float>
constexpr Float sinh(const Float x) noexcept;

//! Calculate hyperbolic cosine
template <typename Float>
constexpr Float cosh(const Float x) noexcept;

//! Calculate hyperbolic tangent
template <typename Float>
constexpr Float tanh(const Float x) noexcept;

//! Calculate inverse hyperbolic sine
template <typename Float>
constexpr Float asinh(const Float x) noexcept;

//! Calculate inverse hyperbolic cosine
template <typename Float>
constexpr Float acosh(const Float x) noexcept;

//! Calculate inverse hyperbolic tangent
template <typename Float>
constexpr Float atanh(const Float x) noexcept;

// Classification and comparison

//! Check if the given number has finite value
template <typename Float>
constexpr bool isFinite(const Float x) noexcept;

//! Check if the given number is infinite
template <typename Float>
constexpr bool isInf(const Float x) noexcept;

//! Check if the given number is NaN
template <typename Float>
constexpr bool isNan(const Float x) noexcept;

//! Check if the given number is normal
template <typename Float>
constexpr bool isNormal(const Float x) noexcept;

//! Check if the given number is subnormal
template <typename Float>
constexpr bool isSubnormal(const Float x) noexcept;

} // namespace consttant

} // namespace zisc

#include "const_math-inl.hpp"

#endif // ZISC_CONST_MATH_HPP
