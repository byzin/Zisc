/*!
  \file math-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
#include "const_math.hpp"
#include "error.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

#if defined(ZISC_MATH_EFFICIENT_POWER)
//#define ZISC_MATH_POW
//#define ZISC_MATH_INVERSE_SQRT
//#define ZISC_MATH_SQRT
#endif // ZISC_MATH_EFFICIENT_POWER

#if defined(ZISC_MATH_EFFICIENT_EXPONENTIAL)
//#define ZISC_MATH_EXP
//#define ZISC_MATH_LOG
//#define ZISC_MATH_LOG2
//#define ZISC_MATH_LOG10
#endif // ZISC_MATH_EFFICIENT_EXPONENTIAL

#if defined(ZISC_MATH_EFFICIENT_TRIGONOMETRIC)
//#define ZISC_MATH_SIN
//#define ZISC_MATH_COS
//#define ZISC_MATH_TAN
//#define ZISC_MATH_ASIN
//#define ZISC_MATH_ACOS
//#define ZISC_MATH_ATAN
//#define ZISC_MATH_SINH
//#define ZISC_MATH_COSH
//#define ZISC_MATH_TANH
//#define ZISC_MATH_ASINH
//#define ZISC_MATH_ACOSH
//#define ZISC_MATH_ATANH
#endif // ZISC_MATH_EFFICIENT_TRIGONOMETRIC

/*!
 \details
 No detailed.
 */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
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
constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                     Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return constant::lcm(m, n);
}

/*!
  */
template <typename Integer> inline
constexpr Integer factorial(const Integer n) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  return constant::factorial(n);
}

/*!
  */
template <typename Float> inline
constexpr Float invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point.");
  return constant::invert(x);
}

/*!
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> sequence(const Integer1 m,
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
constexpr Arithmetic power(Arithmetic base) noexcept
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
constexpr Arithmetic power(const Arithmetic base,
                           const SignedInteger exponent) noexcept
{
  return constant::power(base, exponent);
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float pow(const Float base, const Float exponent) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_POW
  return std::pow(base, exponent);
#else // ZISC_MATH_POW
  static_assert(false, "Optimized pow() is not implemented.");
#endif // ZISC_MATH_POW
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float sqrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_SQRT
  return std::sqrt(n);
#else // ZISC_MATH_SQRT 
  static_assert(false, "Optimized sqrt() is not implemented.");
#endif // ZISC_MATH_SQRT 
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float cbrt(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_CBRT
  return std::cbrt(n);
#else // ZISC_MATH_CBRT
  static_assert(false, "Optimized cbrt() is not implemented.");
#endif // ZISC_MATH_CBRT
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float exp(const Float n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_EXP
  return std::exp(n);
#else // ZISC_MATH_EXP
  static_assert(false, "Optimized exp() is not implemented.");
#endif // ZISC_MATH_EXP
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_LOG
  return std::log(x);
#else // ZISC_MATH_LOG
  static_assert(false, "Optimized log() is not implemented.");
#endif // ZISC_MATH_LOG
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_LOG2
  return std::log2(x);
#else // ZISC_MATH_LOG2
  static_assert(false, "Optimized log2() is not implemented.");
#endif // ZISC_MATH_LOG2
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_LOG10
  return std::log10(x);
#else // ZISC_MATH_LOG10
  static_assert(false, "Optimized log10() is not implemented.");
#endif // ZISC_MATH_LOG10
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float sin(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_SIN
  return std::sin(theta);
#else // ZISC_MATH_SIN
  static_assert(false, "Optimized sin() is not implemented.");
#endif // ZISC_MATH_SIN
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float cos(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_COS
  return std::cos(theta);
#else // ZISC_MATH_COS
  static_assert(false, "Optimized cos() is not implemented.");
#endif // ZISC_MATH_COS
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline 
Float tan(const Float theta) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_TAN
  return std::tan(theta);
#else // ZISC_MATH_TAN
  static_assert(false, "Optimized tan() is not implemented.");
#endif // ZISC_MATH_TAN
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
#ifndef ZISC_MATH_ASIN
  return std::asin(x);
#else // ZISC_MATH_ASIN
  static_assert(false, "Optimized asin() is not implemented.");
#endif // ZISC_MATH_ASIN
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
#ifndef ZISC_MATH_ACOS
  return std::acos(x);
#else // ZISC_MATH_ACOS
  static_assert(false, "Optimized acos() is not implemented.");
#endif // ZISC_MATH_ACOS
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
#ifndef ZISC_MATH_ATAN
  return std::atan(x);
#else // ZISC_MATH_ATAN
  static_assert(false, "Optimized atan() is not implemented.");
#endif // ZISC_MATH_ATAN
}

/*!
  */
template <typename Float> inline
Float sinh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_SINH
  return std::sinh(x);
#else // ZISC_MATH_SINH
  static_assert(false, "Optimized sinh() is not implemented.");
#endif // ZISC_MATH_SINH
}

/*!
  */
template <typename Float> inline
Float cosh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_COSH
  return std::cosh(x);
#else // ZISC_MATH_COSH
  static_assert(false, "Optimized cosh() is not implemented.");
#endif // ZISC_MATH_COSH

}

/*!
  */
template <typename Float> inline
Float tanh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_TANH
  return std::tanh(x);
#else // ZISC_MATH_TANH
  static_assert(false, "Optimized tanh() is not implemented.");
#endif // ZISC_MATH_TANH
}

/*!
  */
template <typename Float> inline
Float asinh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_ASINH
  return std::asinh(x);
#else // ZISC_MATH_ASINH
  static_assert(false, "Optimized asinh() is not implemented.");
#endif // ZISC_MATH_ASINH
}

/*!
  */
template <typename Float> inline
Float acosh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_ACOSH
  return std::acosh(x);
#else // ZISC_MATH_ACOSH
  static_assert(false, "Optimized acosh() is not implemented.");
#endif // ZISC_MATH_ACOSH
}

/*!
  */
template <typename Float> inline
Float atanh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
#ifndef ZISC_MATH_ATANH
  return std::atanh(x);
#else // ZISC_MATH_ATANH
  static_assert(false, "Optimized atanh() is not implemented.");
#endif // ZISC_MATH_ATANH
}

/*!
  \details
  No detailed.
  */
template <typename Float> inline
std::tuple<std::array<Float, 2>, uint> solveQuadratic(const Float a, 
                                                      const Float b, 
                                                      const Float c) noexcept
{
  constexpr Float min_x = std::numeric_limits<Float>::lowest();
  std::array<Float, 2> x = {{min_x, min_x}};
  uint n = 0;

  const Float discriminant = b * b - 4.0 * a * c;
  if (constant::isAlmostEqualToZero(discriminant)) {
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

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename Float> inline
Float solveCubicOneY(const Float p, const Float q) noexcept
{
  Float y;
  if (constant::isAlmostEqualToZero(p)) {
    y = cbrt(-q);
  }
  else if (constant::isAlmostEqualToZero(q)) {
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

} // namespace inner

/*!
  \details
  Solve a cubic equation using "cubic reduction" method.
  */
template <typename Float> inline
Float solveCubicOne(const Float b, const Float c, const Float d) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  const Float k = invert(-3.0) * b;

  const Float p = (b * k + c);
  const Float q = (((2.0 / 3.0) * b * k + c) * k + d);
  const Float y = inner::solveCubicOneY(p, q);
  const Float x = y  +  k;
  return x;
}

/*!
  \details
  Solve a cubic equation using "cubic reduction" method.
  */
template <typename Float> inline
Float solveCubicOne(const Float a,
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
  const Float y = inner::solveCubicOneY(p, q);
  const Float x = y  +  k;
  return x;
}

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename Float> inline
void sortQuarticResults(const std::array<Float, 2>& x1,
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

} // namespace inner

/*!
  \details
  No detailed.
  */
template <typename Float> inline
std::tuple<std::array<Float, 4>, uint> solveQuartic(const Float a, 
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
    inner::sortQuarticResults(x1, x2, x);
  }
  return std::make_tuple(x, n);
}

} // namespace zisc

#endif // ZISC_MATH_INL_HPP
