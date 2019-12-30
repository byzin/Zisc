/*!
  \file math-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_INL_HPP
#define ZISC_MATH_INL_HPP

#include "math.hpp"
// Standard C++ library
#include <cmath>
#include <limits>
#include <type_traits>
// Zisc
#include "math_const.hpp"
#include "error.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \details
 No detailed.
 */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::gcd(
    Integer1 m,
    Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return constant::gcd(m, n);
}

/*!
 \details
 No detailed.
 */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::lcm(
    Integer1 m,
    Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return constant::lcm(m, n);
}

/*!
  */
template <typename Integer> inline
constexpr Integer Math::factorial(const Integer n) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  return constant::factorial(n);
}

/*!
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::sequence(
    const Integer1 m,
    const Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return constant::sequence(m, n);
}

/*!
  \details
  No detailed.
  */
template <int kExponent, typename Arithmetic> inline
constexpr Arithmetic Math::power(Arithmetic base) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>, 
                "Arithmetic isn't arithmetic type");
  return constant::power<kExponent>(base);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, typename SignedInteger>
constexpr Arithmetic Math::power(const Arithmetic base,
                                 const SignedInteger exponent) noexcept
{
  return constant::power(base, exponent);
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::pow(const Float base, const Float exponent) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathPowUsed()
      ? Stl::pow(base, exponent)
      : Zisc::pow(base, exponent);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::sqrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathSqrtUsed()
      ? Stl::sqrt(n)
      : Zisc::sqrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::cbrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathCbrtUsed()
      ? Stl::cbrt(n)
      : Zisc::cbrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::exp(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathExpUsed()
      ? Stl::exp(n)
      : Zisc::exp(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::log(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathLogUsed()
      ? Stl::log(x)
      : Zisc::log(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathLogUsed()
      ? Stl::log2(x)
      : Zisc::log2(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathLogUsed()
      ? Stl::log10(x)
      : Zisc::log10(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::sin(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathTrigonometricUsed()
      ? Stl::sin(theta)
      : Zisc::sin(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::cos(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathTrigonometricUsed()
      ? Stl::cos(theta)
      : Zisc::cos(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::tan(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Config::isStlMathTrigonometricUsed()
      ? Stl::tan(theta)
      : Zisc::tan(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = Config::isStlMathInvTrigonometricUsed()
      ? Stl::asin(x)
      : Zisc::asin(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = Config::isStlMathInvTrigonometricUsed()
      ? Stl::acos(x)
      : Zisc::acos(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
  const auto y = Config::isStlMathInvTrigonometricUsed()
      ? Stl::atan(x)
      : Zisc::atan(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Stl::pow(const Float base, const Float exponent) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::pow(base, exponent);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Stl::sqrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::sqrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Stl::cbrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::cbrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Stl::exp(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::exp(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::log(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::log(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::log2(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::log10(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::sin(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::sin(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::cos(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::cos(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::tan(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = std::tan(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = std::asin(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = std::acos(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Stl::atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
  const auto y = std::atan(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Zisc::pow(const Float base, const Float /* exponent */) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = base;
  static_assert(Config::isStlMathPowUsed(),
                "The zisc pow isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Zisc::sqrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = n;
  static_assert(Config::isStlMathSqrtUsed(),
                "The zisc sqrt isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Zisc::cbrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = n;
  static_assert(Config::isStlMathCbrtUsed(),
                "The zisc cbrt isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::Zisc::exp(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = n;
  static_assert(Config::isStlMathExpUsed(),
                "The zisc exp isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::log(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = x;
  static_assert(Config::isStlMathLogUsed(),
                "The zisc log isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = x;
  static_assert(Config::isStlMathLogUsed(),
                "The zisc log2 isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = x;
  static_assert(Config::isStlMathLogUsed(),
                "The zisc log10 isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::sin(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = theta;
  static_assert(Config::isStlMathTrigonometricUsed(),
                "The zisc sin isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::cos(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = theta;
  static_assert(Config::isStlMathTrigonometricUsed(),
                "The zisc cos isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::tan(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = theta;
  static_assert(Config::isStlMathTrigonometricUsed(),
                "The zisc tan isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = x;
  static_assert(Config::isStlMathInvTrigonometricUsed(),
                "The zisc asin isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = x;
  static_assert(Config::isStlMathInvTrigonometricUsed(),
                "The zisc acos isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float Math::Zisc::atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
  const auto y = x;
  static_assert(Config::isStlMathInvTrigonometricUsed(),
                "The zisc atan isn't implemented yet.");
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
std::tuple<std::array<Float, 2>, uint> Math::solveQuadratic(
    const Float a, 
    const Float b, 
    const Float c) noexcept
{
  constexpr Float min_x = std::numeric_limits<Float>::lowest();
  std::array<Float, 2> x = {{min_x, min_x}};
  uint n = 0;

  const Float discriminant = b * b - 4.0 * a * c;
  if (constant::Math::isAlmostEqual(discriminant, cast<Float>(0.0))) {
    x[0] = -b / (2.0 * a);
    n = 1;
  }
  else if (0.0 < discriminant) {
    const Float root_d = sqrt(discriminant);
    x[0] = (-b + root_d) / (2.0 * a);
    x[1] = (-b - root_d) / (2.0 * a);
    n = 2;
  }
  return std::make_tuple(x, n);
}

/*!
  \details
  Solve a cubic equation using "cubic reduction" method.
  */
template <typename Float> inline
Float Math::solveCubicOne(const Float b, const Float c, const Float d) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  const Float k = invert(-3.0) * b;

  const Float p = (b * k + c);
  const Float q = (((2.0 / 3.0) * b * k + c) * k + d);
  const Float y = solveCubicOneY(p, q);
  const Float x = y  +  k;
  return x;
}

/*!
  \details
  Solve a cubic equation using "cubic reduction" method.
  */
template <typename Float> inline
Float Math::solveCubicOne(const Float a,
                          const Float b,
                          const Float c,
                          const Float d) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  ZISC_ASSERT(a != 0.0, "The a is zero.");
  const Float inv_a = invert(a);
  const Float k = invert(-3.0) * b * inv_a;

  const Float p = (b * k + c) * inv_a;
  const Float q = (((2.0 / 3.0) * b * k + c) * k + d) * inv_a;
  const Float y = solveCubicOneY(p, q);
  const Float x = y  +  k;
  return x;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
std::tuple<std::array<Float, 4>, uint> Math::solveQuartic(
    const Float a, 
    const Float b, 
    const Float c, 
    const Float d, 
    const Float e) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  ZISC_ASSERT(a != 0.0, "The a is zero.");
  const Float inv_a = 1.0 / a;
  const Float k = invert(-4.0) * b * inv_a;

  const Float p = ((3.0 / 2.0) * b * k + c) * inv_a;
  const Float q = ((b * k + c) * 2.0 * k + d) * inv_a;
  const Float r = ((((3.0 / 4.0) * b * k + c) * k + d) * k + e) * inv_a;
  const Float z = solveCubicOne(-0.5 * p, -r, invert(8.0) * (4 * p * r - q * q));

  uint n = 0;
  std::array<Float, 4> x;

  const Float f2 = 2.0 * z - p;
  if (0.0 <= f2) {
    constexpr Float min_x = std::numeric_limits<Float>::lowest();
    const Float f = sqrt(f2);
    std::array<Float, 2> x1 = {{min_x, min_x}};
    const Float d1 = -(2.0 * (z + (q / f)) + p);
    if (0.0 <= d1) {
      const Float root_d1 = sqrt(d1);
      x1[0] = 0.5 * (f + root_d1) + k;
      x1[1] = 0.5 * (f - root_d1) + k;
      n += 2;
    }
    std::array<Float, 2> x2 = {{min_x, min_x}};
    const Float d2 = -(2.0 * (z - (q / f)) + p);
    if (0.0 <= d2) {
      const Float root_d2 = sqrt(d2);
      x2[0] = 0.5 * (-f + root_d2) + k;
      x2[1] = 0.5 * (-f - root_d2) + k;
      n += 2;
    }
    sortQuarticResults(x1, x2, x);
  }
  return std::make_tuple(x, n);
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float Math::solveCubicOneY(const Float p, const Float q) noexcept
{
  Float y;
  if (constant::Math::isAlmostEqual(p, cast<Float>(0.0))) {
    y = cbrt(-q);
  }
  else if (constant::Math::isAlmostEqual(q, cast<Float>(0.0))) {
    y = sqrt(-p);
  }
  else {
    const Float discriminant = power<2>(q) + (4.0 / 27.0) * power<3>(p);
    if (0.0 <= discriminant) {
      const Float z = cbrt(0.5 * (sqrt(discriminant) - q));
      y = z - (p / (3.0 * z));
    }
    else {
      ZISC_ASSERT(p < 0.0, "The p is positive: ", p);
      const Float r = sqrt(invert(-3.0) * p);
      const Float cos_theta = clamp(q / (-2.0 * power<3>(r)), -1.0, 1.0);
      const Float phi = (invert(3.0)) * acos(cos_theta);
      y = 2.0 * r * cos(phi);
    }
  }
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
void Math::sortQuarticResults(const std::array<Float, 2>& x1,
                              const std::array<Float, 2>& x2,
                              std::array<Float, 4>& x) noexcept
{
  x[0] = max(x1[0], x2[0]);
  const Float tmp1 = min(x1[0], x2[0]);
  const Float tmp2 = max(x1[1], x2[1]);
  x[1] = max(tmp1, tmp2);
  x[2] = min(tmp1, tmp2);
  x[3] = min(x1[1], x2[1]);
}

/*!
 \details
 No detailed.
 */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> gcd(
    Integer1 m,
    Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto y = Math::gcd(m, n);
  return y;
}

/*!
 \details
 No detailed.
 */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> lcm(
    Integer1 m,
    Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto y = Math::lcm(m, n);
  return y;
}

/*!
  */
template <typename Integer> inline
constexpr Integer factorial(const Integer n) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  const auto y = Math::factorial(n);
  return y;
}

/*!
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> sequence(
    const Integer1 m,
    const Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto y = Math::sequence(m, n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <int kExponent, typename Arithmetic> inline
constexpr Arithmetic power(Arithmetic base) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>, 
                "Arithmetic isn't arithmetic type");
  const auto y = Math::power<kExponent>(base);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, typename SignedInteger>
constexpr Arithmetic power(const Arithmetic base,
                           const SignedInteger exponent) noexcept
{
  const auto y = Math::power(base, exponent);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float pow(const Float base, const Float exponent) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::pow(base, exponent);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float sqrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::sqrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float cbrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::cbrt(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float exp(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::exp(n);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log2(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log10(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float sin(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::sin(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float cos(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::cos(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float tan(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::tan(theta);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = Math::asin(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
  const auto y = Math::acos(x);
  return y;
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
  const auto y = Math::atan(x);
  return y;
}


} // namespace zisc

#endif // ZISC_MATH_INL_HPP
