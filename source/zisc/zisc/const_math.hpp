/*!
  \file const_math.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONST_MATH_HPP
#define ZISC_CONST_MATH_HPP

// Standard C++ library
#include <cstddef>
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

//! Return the greatest common divisor of two integer m and n
template <typename Integral>
constexpr Integral gcd(Integral m, Integral n) noexcept;

//! Return a inverse number
template <typename Float>
constexpr Float invert(const Float x) noexcept;

//!
template <typename Integer>
constexpr Integer sequence(const Integer m, const Integer n) noexcept;

//!
template <typename Integer>
constexpr Integer factorial(const Integer x) noexcept;

// Power functions

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

} // namespace consttant

} // namespace zisc

#include "const_math-inl.hpp"

#endif // ZISC_CONST_MATH_HPP
