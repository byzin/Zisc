/*!
  \file math.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_HPP
#define ZISC_MATH_HPP

// Standard C++ library
#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
// Zisc
#include "const_math.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Constant values

//! Napier's constant
template <typename Float>
constexpr Float kE = constant::exp(static_cast<Float>(1.0));

//! Circular constant
template <typename Float>
constexpr Float kPi = constant::pi<Float>();

// Basic operations

//! Compute the greatest common divisor of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                     Integer2 n) noexcept;

//! Compute the least common multiple of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                     Integer2 n) noexcept;

//!
template <typename Integer>
constexpr Integer factorial(const Integer n) noexcept;

//! Return a inverse number
template <typename Float>
constexpr Float invert(const Float x) noexcept;

//!
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> sequence(const Integer1 m,
                                                          const Integer2 n) noexcept;

// Power functions

//! Raise a number to the given power
template <int kExponent, typename Arithmetic>
constexpr Arithmetic power(Arithmetic base) noexcept;

//! Raise a number to the given power
template <typename Arithmetic, typename SignedInteger>
constexpr Arithmetic power(const Arithmetic base,
                           const SignedInteger exponent) noexcept;

//! Raise a number to the given power
template <typename Float>
Float pow(const Float base, const Float exponent) noexcept;

//! Calculate the square root
template <typename Float>
Float sqrt(const Float n) noexcept;

//! Calculate the cubic root
template <typename Float>
Float cbrt(const Float n) noexcept;

// Exponential functions

//! Calculate e raised to the given power
template <typename Float>
Float exp(const Float n) noexcept;

//! Calculate the natural logarithm
template <typename Float>
Float log(const Float x) noexcept;

//! Calculate the binary logarithm
float log2(const float x) noexcept;

//! Calculate the common logarithm
float log10(const float x) noexcept;

// Trigonometric functions

//! Calculate sin(theta)
template <typename Float>
Float sin(const Float theta) noexcept;

//! Calculate cos(theta)
template <typename Float>
Float cos(const Float theta) noexcept;

//! Calculate tan(theta)
template <typename Float>
Float tan(const Float theta) noexcept;

//! Calculate asin(x)
template <typename Float>
Float asin(const Float x) noexcept;

//! Calculate acos(x)
template <typename Float>
Float acos(const Float x) noexcept;

//! Calculate atan(x)
template <typename Float>
Float atan(const Float x) noexcept;

// Hyperbolic functions

//! Calculate hyperbolic sine
template <typename Float>
Float sinh(const Float x) noexcept;

//! Calculate hyperbolic cosine
template <typename Float>
Float cosh(const Float x) noexcept;

//! Calculate hyperbolic tangent
template <typename Float>
Float tanh(const Float x) noexcept;

//! Calculate inverse hyperbolic sine
template <typename Float>
Float asinh(const Float x) noexcept;

//! Calculate inverse hyperbolic cosine
template <typename Float>
Float acosh(const Float x) noexcept;

//! Calculate inverse hyperbolic tangent
template <typename Float>
Float atanh(const Float x) noexcept;

// Polynomial

//! Solve a quadratic equation
template <typename Float>
std::tuple<std::array<Float, 2>, uint> solveQuadratic(const Float a, 
                                                      const Float b, 
                                                      const Float c) noexcept;

//! Solve a cubic equation
template <typename Float>
Float solveCubicOne(const Float b, 
                    const Float c, 
                    const Float d) noexcept;

//! Solve a cubic equation
template <typename Float>
Float solveCubicOne(const Float a, 
                    const Float b, 
                    const Float c, 
                    const Float d) noexcept;

//! Solve a quartic equation
template <typename Float>
std::tuple<std::array<Float, 4>, uint> solveQuartic(const Float a, 
                                                    const Float b, 
                                                    const Float c, 
                                                    const Float d, 
                                                    const Float e) noexcept;

} // namespace zisc

#include "math-inl.hpp"

#endif // ZISC_MATH_HPP
