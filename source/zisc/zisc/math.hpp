/*!
  \file math.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "math_const.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Math
{
 public:
  // Basic operations

  //! Compute the greatest common divisor of the integers m and n
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                              Integer2 n) noexcept;

  //! Compute the least common multiple of the integers m and n
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                              Integer2 n) noexcept;

  //!
  template <typename Integer>
  static constexpr Integer factorial(const Integer n) noexcept;

  //!
  template <typename Integer1, typename Integer2>
  static constexpr std::common_type_t<Integer1, Integer2> sequence(
      const Integer1 m,
      const Integer2 n) noexcept;

  // Power functions

  //! Raise a number to the given power
  template <int kExponent, typename Arithmetic>
  static constexpr Arithmetic power(Arithmetic base) noexcept;

  //! Raise a number to the given power
  template <typename Arithmetic, typename SignedInteger>
  static constexpr Arithmetic power(const Arithmetic base,
                                    const SignedInteger exponent) noexcept;

  //! Raise a number to the given power
  template <typename Float>
  static Float pow(const Float base, const Float exponent) noexcept;

  //! Calculate the square root
  template <typename Float>
  static Float sqrt(const Float n) noexcept;

  //! Calculate the cubic root
  template <typename Float>
  static Float cbrt(const Float n) noexcept;

  // Exponential functions

  //! Calculate e raised to the given power
  template <typename Float>
  static Float exp(const Float n) noexcept;

  //! Calculate the natural logarithm
  template <typename Float>
  static Float log(const Float x) noexcept;

  //! Calculate the binary logarithm
  template <typename Float>
  static Float log2(const Float x) noexcept;

  //! Calculate the common logarithm
  template <typename Float>
  static Float log10(const Float x) noexcept;

  // Trigonometric functions

  //! Calculate sin(theta)
  template <typename Float>
  static Float sin(const Float theta) noexcept;

  //! Calculate cos(theta)
  template <typename Float>
  static Float cos(const Float theta) noexcept;

  //! Calculate tan(theta)
  template <typename Float>
  static Float tan(const Float theta) noexcept;

  //! Calculate asin(x)
  template <typename Float>
  static Float asin(const Float x) noexcept;

  //! Calculate acos(x)
  template <typename Float>
  static Float acos(const Float x) noexcept;

  //! Calculate atan(x)
  template <typename Float>
  static Float atan(const Float x) noexcept;

  // Polynomial

  //! Solve a quadratic equation
  template <typename Float>
  static std::tuple<std::array<Float, 2>, uint> solveQuadratic(
      const Float a, 
      const Float b, 
      const Float c) noexcept;

  //! Solve a cubic equation
  template <typename Float>
  static Float solveCubicOne(const Float b, 
                             const Float c, 
                             const Float d) noexcept;

  //! Solve a cubic equation
  template <typename Float>
  static Float solveCubicOne(const Float a, 
                             const Float b, 
                             const Float c, 
                             const Float d) noexcept;

  //! Solve a quartic equation
  template <typename Float>
  static std::tuple<std::array<Float, 4>, uint> solveQuartic(
      const Float a, 
      const Float b, 
      const Float c, 
      const Float d, 
      const Float e) noexcept;

  class Stl
  {
   public:
    //! Raise a number to the given power
    template <typename Float>
    static Float pow(const Float base, const Float exponent) noexcept;

    //! Calculate the square root
    template <typename Float>
    static Float sqrt(const Float n) noexcept;

    //! Calculate the cubic root
    template <typename Float>
    static Float cbrt(const Float n) noexcept;

    // Exponential functions

    //! Calculate e raised to the given power
    template <typename Float>
    static Float exp(const Float n) noexcept;

    //! Calculate the natural logarithm
    template <typename Float>
    static Float log(const Float x) noexcept;

    //! Calculate the binary logarithm
    template <typename Float>
    static Float log2(const Float x) noexcept;

    //! Calculate the common logarithm
    template <typename Float>
    static Float log10(const Float x) noexcept;

    // Trigonometric functions

    //! Calculate sin(theta)
    template <typename Float>
    static Float sin(const Float theta) noexcept;

    //! Calculate cos(theta)
    template <typename Float>
    static Float cos(const Float theta) noexcept;

    //! Calculate tan(theta)
    template <typename Float>
    static Float tan(const Float theta) noexcept;

    //! Calculate asin(x)
    template <typename Float>
    static Float asin(const Float x) noexcept;

    //! Calculate acos(x)
    template <typename Float>
    static Float acos(const Float x) noexcept;

    //! Calculate atan(x)
    template <typename Float>
    static Float atan(const Float x) noexcept;
  };

  class Zisc
  {
   public:
    //! Raise a number to the given power
    template <typename Float>
    static Float pow(const Float base, const Float exponent) noexcept;

    //! Calculate the square root
    template <typename Float>
    static Float sqrt(const Float n) noexcept;

    //! Calculate the cubic root
    template <typename Float>
    static Float cbrt(const Float n) noexcept;

    // Exponential functions

    //! Calculate e raised to the given power
    template <typename Float>
    static Float exp(const Float n) noexcept;

    //! Calculate the natural logarithm
    template <typename Float>
    static Float log(const Float x) noexcept;

    //! Calculate the binary logarithm
    template <typename Float>
    static Float log2(const Float x) noexcept;

    //! Calculate the common logarithm
    template <typename Float>
    static Float log10(const Float x) noexcept;

    // Trigonometric functions

    //! Calculate sin(theta)
    template <typename Float>
    static Float sin(const Float theta) noexcept;

    //! Calculate cos(theta)
    template <typename Float>
    static Float cos(const Float theta) noexcept;

    //! Calculate tan(theta)
    template <typename Float>
    static Float tan(const Float theta) noexcept;

    //! Calculate asin(x)
    template <typename Float>
    static Float asin(const Float x) noexcept;

    //! Calculate acos(x)
    template <typename Float>
    static Float acos(const Float x) noexcept;

    //! Calculate atan(x)
    template <typename Float>
    static Float atan(const Float x) noexcept;
  };

 private:
  template <typename Float>
  static Float solveCubicOneY(const Float p, const Float q) noexcept;

  template <typename Float>
  static void sortQuarticResults(const std::array<Float, 2>& x1,
                                 const std::array<Float, 2>& x2,
                                 std::array<Float, 4>& x) noexcept;
};

// Constant values

//! Napier's constant
template <typename Float>
constexpr Float kE = constant::Math::exp(static_cast<Float>(1.0));

//! Circular constant
template <typename Float>
constexpr Float kPi = constant::Math::pi<Float>();

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

} // namespace zisc

#include "math-inl.hpp"

#endif // ZISC_MATH_HPP
