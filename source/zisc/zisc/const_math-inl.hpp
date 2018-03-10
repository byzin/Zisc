/*!
  \file const_math-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONST_MATH_INL_HPP
#define ZISC_CONST_MATH_INL_HPP

#include "const_math.hpp"
// Standard C++ library
#include <cmath>
#include <limits>
#include <numeric>
#include <tuple>
#include <type_traits>
// Zisc
#include "error.hpp"
#include "fraction.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace constant {

namespace inner {

template <typename Float>
constexpr Float calcPi(const int64 n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float t = cast<Float>(2.0);
  Float pi = t;
  for (int64 k = n; 0 < k; --k) {
    const Float a = cast<Float>(k) / cast<Float>(2 * k + 1);
    pi = t + a * pi;
  }
  return pi;
}

} // namespace inner

/*!
  */
template <typename Float>
constexpr Float pi() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float p = cast<Float>(1.0);
  Float prev_p = cast<Float>(0.0);
  for (auto n = cast<int64>(2 * sizeof(Float)); p != prev_p; n = n << 1) {
    prev_p = p;
    p = inner::calcPi<Float>(n);
  }
  return p;
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic> inline
constexpr Arithmetic abs(const Arithmetic x) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type.");
  if constexpr (std::is_signed_v<Arithmetic>)
    return isNegative(x) ? -x : x;
  else
    return x;
}

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
  return std::gcd(m, n);
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
  return std::lcm(m, n);
}

/*!
  */
template <typename Float> inline
constexpr Float invert(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);
  return one / x;
}

/*!
 \details
 No detailed.
 */
template <long y, typename Integer> inline
constexpr Integer mod(Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  static_assert(y != 0, "The y is zero.");
  constexpr bool is_power_of_2 = isPowerOf2(y);
  const Integer result = (is_power_of_2)
      ? x & (cast<Integer>(y) - 1)
      : x % cast<Integer>(y);
  return result;
}

/*!
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> sequence(const Integer1 m,
                                                          const Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  std::common_type_t<Integer1, Integer2> x = 1;
  for (auto i = m; i < n; ++i)
    x = x * (i + 1);
  return x;
}

/*!
  */
template <typename Integer> inline
constexpr Integer factorial(const Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  constexpr Integer start = 1;
  return sequence(start, x);
}

/*!
  */
template <int kExponent, typename Arithmetic> inline
constexpr Arithmetic power(Arithmetic base) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type");
  Arithmetic x = cast<Arithmetic>(1);
  // Check if base is zero
  if (base != cast<Arithmetic>(0)) {
    // Inverse pow
    if constexpr (isNegative(kExponent)) {
      x = power<-kExponent>(cast<Arithmetic>(1) / base);
    }
    // pow
    else {
      for (int exponent = kExponent; 0 < exponent; exponent = exponent >> 1) {
        if (isOdd(exponent))
          x = x * base;
        base = base * base;
      }
    }
  }
  return x;
}

/*!
  */
template <typename Arithmetic, typename SignedInteger> inline
constexpr Arithmetic pow(Arithmetic base, SignedInteger exponent) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type");
  static_assert(kIsSignedInteger<SignedInteger>,
                "SignedInteger isn't signed integer type.");
  Arithmetic x = cast<Arithmetic>(1);
  // Check if base is zero
  if (base != cast<Arithmetic>(0)) {
    // Inverse pow
    if (isNegative(exponent)) {
      x = pow(cast<Arithmetic>(1) / base, -exponent);
    }
    // pow
    else {
      constexpr SignedInteger zero = cast<SignedInteger>(0);
      for (; zero < exponent; exponent = exponent >> 1) {
        if (isOdd(exponent))
          x = x * base;
        base = base * base;
      }
    }
  }
  return x;
}

/*!
  */
template <typename Float> inline
constexpr Float sqrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = x,
        pre_y = cast<Float>(0.0);
  while (y != pre_y) {
    pre_y = y;
    constexpr Float k = cast<Float>(1.0 / 2.0);
    y = k * (y + x / y);
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cbrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = x,
        pre_y = cast<Float>(0.0);
  while (y != pre_y) {
    pre_y = y;
    constexpr Float t = cast<Float>(2.0);
    constexpr Float k = cast<Float>(1.0 / 3.0);
    y = k * (t * y + x / (y * y));
  }
  return y;
}

namespace inner {

/*!
  */
template <typename Float> inline
constexpr Float exp(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(1.0) + x,
        pre_y = std::numeric_limits<Float>::infinity();
  Float t = x;
  for (int iteration = 2; y != pre_y; ++iteration) {
    pre_y = y;
    t = t * (x / cast<Float>(iteration));
    y = y + t;
  }
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float exp(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  int exponent = 1;
  while (cast<Float>(1.0) < abs(x)) {
    x = x / cast<Float>(2.0);
    exponent = exponent << 1;
  }
  Float y = inner::exp(x);
  y = pow(y, exponent);
  return y;
}

namespace inner {

/*!
  */
template <typename Float> inline
constexpr std::tuple<Float, int> extractFloat(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  bool is_minus = x < cast<Float>(0.0);
  x = abs(x);
  int exponent = 0;
  if (x != cast<Float>(0.0)) {
    if (cast<Float>(2.0) < x) {
      constexpr Float base = cast<Float>(2.0);
      while (base < x) {
        x = x / base;
        ++exponent;
      }
    }
    else {
      constexpr Float base = cast<Float>(0.5);
      while (x < cast<Float>(1.0)) {
        x = x / base;
        --exponent;
      }
    }
  }
  return std::make_tuple((is_minus) ? -x : x, exponent);
}

/*!
  \details
  x must be satisfied [1, 2).
  */
template <typename Float> inline
constexpr Float log2(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(0.0);
  if (x != cast<Float>(1.0)) {
    constexpr Float base = cast<Float>(2.0);
    Float pre_y = std::numeric_limits<Float>::infinity();
    for (Float element = invert(base); pre_y != y; element = element / base) {
      x = x * x;
      if (base < x) {
        pre_y = y;
        y = y + element;
        x = x / base;
      }
    }
  }
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  // Extract the float x
  const auto f = inner::extractFloat(x);
  const Float mantissa = std::get<0>(f);
  const int exponent = std::get<1>(f);
  // Calculate the log2(x)
  return inner::log2(mantissa) + cast<Float>(exponent);
}

/*!
  */
template <typename Float> inline
constexpr Float log(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = exp(cast<Float>(1.0));
  return log2(x) / log2(base);
}

/*!
  */
template <typename Float> inline
constexpr Float log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(10.0);
  return log2(x) / log2(base);
}

namespace inner {

/*!
  */
template <typename Float> inline
constexpr Float sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = x,
        pre_y = std::numeric_limits<Float>::infinity();
  Float t = y;
  for (int iteration = 1; y != pre_y; ++iteration) {
    pre_y = y;
    t = t * ((x * x) / cast<Float>((2 * iteration + 1) * (2 * iteration)));
    y = y + (isOdd(iteration) ? -t : t);
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(1.0),
        pre_y = std::numeric_limits<Float>::infinity();
  Float t = y;
  for (int iteration = 1; y != pre_y; ++iteration) {
    pre_y = y;
    t = t * ((x * x) / cast<Float>((2 * iteration) * (2 * iteration - 1)));
    y = y + (isOdd(iteration) ? -t : t);
  }
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(0.0);
  constexpr Float half_pi = cast<Float>(0.5) * pi<Float>();
  constexpr Float quarter_pi = cast<Float>(0.25) * pi<Float>();
  // x < 0
  if (isNegative(x)) {
    y = -sin(-x);
  }
  // pi < x
  else if (pi<Float>() < x) {
    const int n = cast<int>(x / pi<Float>());
    y = sin(x - cast<Float>(n) * pi<Float>());
    y = (isOdd(n)) ? -y : y;
  }
  // pi/2 < x <= pi
  else if (half_pi < x) {
    y = sin(pi<Float>() - x);
  }
  // pi/4 < x <= pi/2
  else if (quarter_pi < x) {
    y = inner::cos(half_pi - x);
  }
  // 0 <= x <= pi/4
  else {
    y = inner::sin(x);
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float half_pi = cast<Float>(0.5) * pi<Float>();
  return sin(half_pi - x);
}

/*!
  */
template <typename Float> inline
constexpr Float tan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  return sin(x) / cos(x);
}

namespace inner {

/*!
  */
template <typename Float> inline
constexpr Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = x / (cast<Float>(1.0) + (x * x)),
        pre_y = std::numeric_limits<Float>::infinity();
  Float t1 = cast<Float>(1.0),
        t2 = y;
  for (int iteration = 1; y != pre_y; ++iteration) {
    pre_y = y;
    t1 = t1 * (pow(cast<Float>(2.0) * cast<Float>(iteration), 2) /
               cast<Float>((2 * iteration + 1) * (2 * iteration)));
    t2 = t2 * ((x * x) / (cast<Float>(1.0) + (x * x)));
    y = y + (t1 * t2);
  }
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(0.0);
  if (isNegative(x)) {
    y = -atan(-x);
  }
  else if (cast<Float>(1.0) < x) {
    constexpr Float half_pi = cast<Float>(0.5) * pi<Float>();
    y = half_pi - atan(cast<Float>(1.0) / x);
  }
  else {
    y = inner::atan(x);
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);
  const Float y =
      (x == -one) ? pi<Float>() :
      (x == one)  ? cast<Float>(0.0) :
                    cast<Float>(2.0) * atan(sqrt(one - x * x) / (one + x));
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);
  const Float y =
      (x == -one) ? -cast<Float>(0.5) * pi<Float>() :
      (x == one)  ? cast<Float>(0.5) * pi<Float>() :
                    cast<Float>(2.0) * atan(x / (one + sqrt(one - x * x)));
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float sinh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float y = (exp(x) - exp(-x)) / cast<Float>(2.0);
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cosh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float y = (exp(x) + exp(-x)) / cast<Float>(2.0);
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float tanh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float o = cast<Float>(1.0);
  constexpr Float t = cast<Float>(2.0);
  const Float y = (o - exp(-t * x)) / (o + exp(-t * x));
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float asinh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float y = log(x + sqrt(x * x + cast<Float>(1.0)));
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float acosh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float y = log(x + sqrt(x * x - cast<Float>(1.0)));
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float atanh(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float o = cast<Float>(1.0);
  constexpr Float t = cast<Float>(2.0);
  const Float y = log((x + o) / (x - o)) / t;
  return y;
}

} // namespace constant

} // namespace zisc

#endif // ZISC_CONST_MATH_INL_HPP
