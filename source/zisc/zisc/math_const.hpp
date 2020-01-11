/*!
  \file math_const.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_CONST_HPP
#define ZISC_MATH_CONST_HPP

// Standard C++ library
#include <cstddef>
#include <tuple>
#include <type_traits>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

namespace constant {

/*!
  \brief No brief description

  No detailed description.
  */
class Math
{
 public:
  // Constexpr values

  //! Return circular constant
  template <typename Float>
  static constexpr Float pi() noexcept;

  // Basic operations

  //! Return the ULPs of the given value
  template <std::size_t kUlpScale = 2, typename Float>
  static constexpr Float getUlps(const Float value) noexcept;

  //! Check whether given two values are almost same
  template <std::size_t kUlpScale = 2, typename Arith>
  static constexpr bool isAlmostEqual(const Arith lhs, const Arith rhs) noexcept;

  //! Fused multiply-add operation
  template <typename Arith>
  static constexpr Arith fma(const Arith a, const Arith b, const Arith c) noexcept;

  //! Compute the greatest common divisor of the integers m and n
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                              Integer2 n) noexcept;

  //! Compute the least common multiple of the integers m and n
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                              Integer2 n) noexcept;

  //! Compute the remainder of the division operation x/y
  template <long y, typename Integer>
  static constexpr Integer mod(Integer x) noexcept;

  //!
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> sequence(
      const Integer1 m,
      const Integer2 n) noexcept;

  //!
  template <typename Integer>
  static constexpr Integer factorial(const Integer x) noexcept;

  // Power functions

  //! Calculate the value of base raised to the power exponent
  template <int kExponent, typename Arith>
  static constexpr Arith power(Arith base) noexcept;

  //! Calculate the value of base raised to the power exponent
  template <typename Arith, typename SignedInteger>
  static constexpr Arith power(Arith base, SignedInteger exponent) noexcept;

  //! Calculate the value of base raised to the power exponent
  template <typename Float>
  static constexpr Float pow(const Float base, const Float exponent) noexcept;

  //! Calculate the square root
  template <typename Float>
  static constexpr Float sqrt(const Float x) noexcept;

  //! Calculate the cubic root
  template <typename Float>
  static constexpr Float cbrt(const Float x) noexcept;

  // Exponential functions

  //! Calculate e raised to the given power
  template <typename Float>
  static constexpr Float exp(Float x) noexcept;

  //! Calculate the natural logarithm
  template <typename Float>
  static constexpr Float log(Float x) noexcept;

  //! Calculate the binary logarithm
  template <typename Float>
  static constexpr Float log2(const Float x) noexcept;

  //! Calculate the common logarithm
  template <typename Float>
  static constexpr Float log10(const Float x) noexcept;

  // Trigonometric functions

  //! Calculate sin(theta)
  template <typename Float>
  static constexpr Float sin(const Float x) noexcept;

  //! Calculate cos(theta)
  template <typename Float>
  static constexpr Float cos(const Float x) noexcept;

  //! Calculate tan(theta)
  template <typename Float>
  static constexpr Float tan(const Float x) noexcept;

  //! Calculate asin(x)
  template <typename Float>
  static constexpr Float asin(const Float x) noexcept;

  //! Calculate acos(x)
  template <typename Float>
  static constexpr Float acos(const Float x) noexcept;

  //! Calculate atan(x)
  template <typename Float>
  static constexpr Float atan(const Float x) noexcept;

  // Floating point manipulation

  //! Decompose given floating point value
  template <typename Float>
  static constexpr Float frexp(const Float x, int* e) noexcept;

  //! Multiply a floating point value x by the number 2 raised to the e power
  template <typename Float>
  static constexpr Float ldexp(const Float x, const int e) noexcept;

  // Nearest integer floating point operations

  //! Round the floating point argument to an integer value
  template <typename Float>
  static constexpr Float rint(const Float x) noexcept;

 private:
  template <typename Float>
  static constexpr Float calcPi(const int64b n) noexcept;

  template <int kN, typename Float> inline
  static constexpr Float estimateNRoot(const Float x) noexcept;

  template <typename Float>
  static constexpr Float expImpl(const Float x) noexcept;

  //! Return the upper part of log(2)
  template <typename Float>
  static constexpr Float getL2u() noexcept;

  //! Return the lower part of log(2)
  template <typename Float>
  static constexpr Float getL2l() noexcept;

  template <typename Float>
  static constexpr Float log2Impl(Float x) noexcept;

  template <typename Float>
  static constexpr std::tuple<Float, Float> sincosImpl(Float x) noexcept;

  template <typename Float>
  static constexpr Float getPiA() noexcept;

  template <typename Float>
  static constexpr Float getPiB() noexcept;

  template <typename Float>
  static constexpr Float getPiC() noexcept;

  template <typename Float>
  static constexpr Float getPiD() noexcept;

  template <typename Float>
  static constexpr std::tuple<Float, Float> sincos(const Float x) noexcept;

  template <typename Float>
  static constexpr Float atanImpl(Float x) noexcept;
};

// STL style math function aliases

// Basic operations

//! Fused multiply-add operation
template <typename Arith>
constexpr Arith fma(const Arith a, const Arith b, const Arith c) noexcept;

//! Compute the greatest common divisor of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                     Integer2 n) noexcept;

//! Compute the least common multiple of the integers m and n
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                     Integer2 n) noexcept;

//! Compute the remainder of the division operation x/y
template <long y, typename Integer>
constexpr Integer mod(Integer x) noexcept;

//!
template <typename Integer1, typename Integer2>
constexpr std::common_type_t<Integer1, Integer2> sequence(
    const Integer1 m,
    const Integer2 n) noexcept;

//!
template <typename Integer>
constexpr Integer factorial(const Integer x) noexcept;

// Power functions

//! Calculate the value of base raised to the power exponent
template <int kExponent, typename Arith>
constexpr Arith power(Arith base) noexcept;

//! Calculate the value of base raised to the power exponent
template <typename Arith, typename SignedInteger>
constexpr Arith power(Arith base, SignedInteger exponent) noexcept;

//! Calculate the value of base raised to the power exponent
template <typename Float>
constexpr Float pow(const Float base, const Float exponent) noexcept;

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

// Floating point manipulation

//! Decompose given floating point value
template <typename Float>
constexpr Float frexp(const Float x, int* e) noexcept;

//! Multiply a floating point value x by the number 2 raised to the e power
template <typename Float>
constexpr Float ldexp(const Float x, const int e) noexcept;

// Nearest integer floating point operations

//! Round the floating point argument to an integer value
template <typename Float>
constexpr Float rint(const Float x) noexcept;

} // namespace consttant

} // namespace zisc

#include "math_const-inl.hpp"

#endif // ZISC_MATH_CONST_HPP
