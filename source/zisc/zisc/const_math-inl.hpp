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
#include "floating_point.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace constant {

namespace inner {

template <typename Float> inline
constexpr Float calcPi(const int64b n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float t = cast<Float>(2.0);
  Float pi = t;
  for (int64b k = n; 0 < k; --k) {
    const Float a = cast<Float>(k) / cast<Float>(2 * k + 1);
    pi = t + a * pi;
  }
  return pi;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float pi() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr int64b n = std::numeric_limits<Float>::digits;
  const Float p = inner::calcPi<Float>(n);
  return p;
}

/*!
  */
template <std::size_t kUlpScale, typename Float> inline
constexpr Float getUlps(const Float value) noexcept
{
  constexpr Float e = cast<Float>(kUlpScale) * std::numeric_limits<Float>::epsilon();
  const Float ulps = e * abs(value);
  return ulps;
}

/*!
  */
template <std::size_t kUlpScale, typename Arith> inline
constexpr bool isAlmostEqual(const Arith lhs, const Arith rhs) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type");
  bool result = false;
  if constexpr (kIsFloat<Arith>) {
    const Arith v = abs(lhs - rhs);
    result = (v <= getUlps<kUlpScale>(lhs + rhs)) || isSubnormal(v);
  }
  else {
    result = lhs == rhs;
  }
  return result;
}

/*!
  */
template <std::size_t kUlpScale, typename Arith> inline
constexpr bool isAlmostEqualToZero(const Arith value) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type");
  constexpr Arith zero = cast<Arith>(0);
  bool result = false;
  if constexpr (kIsFloat<Arith>) {
    const Arith v = abs(value);
    result = v <= getUlps<kUlpScale>(v);
  }
  else {
    result = value == zero;
  }
  return result;
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
  constexpr Float zero = cast<Float>(0.0);
  constexpr Float one = cast<Float>(1.0);
  constexpr Float m = std::numeric_limits<Float>::max();
  return (x != zero) ? one / x : m;
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
template <int kExponent, typename Arith> inline
constexpr Arith power(Arith base) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>,
                "Arith isn't arithmetic type");
  const auto zero = cast<Arith>(0);
  Arith x = zero;
  // Check if base is zero
  if (base != zero) {
    x = cast<Arith>(1);
    // Inverse pow
    if constexpr (kIsFloat<Arith> && isNegative(kExponent))
      base = invert(base);
    // Compute pow
    for (int exponent = abs(kExponent); 0 < exponent; exponent = exponent >> 1) {
      if (isOdd(exponent))
        x = x * base;
      if (1 < exponent)
        base = base * base;
    }
  }
  return x;
}

/*!
  */
template <typename Arith, typename SignedInteger> inline
constexpr Arith power(Arith base, SignedInteger exponent) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>,
                "Arith isn't arithmetic type");
  static_assert(kIsSignedInteger<SignedInteger>,
                "SignedInteger isn't signed integer type.");
  const auto zero = cast<Arith>(0);
  Arith x = zero;
  // Check if base is zero
  if (base != zero) {
    x = cast<Arith>(1);
    // Inverse pow
    if constexpr (kIsFloat<Arith>) {
      if(isNegative(exponent))
        base = invert(base);
    }
    // Compute pow
    for (exponent = abs(exponent); 0 < exponent; exponent = exponent >> 1) {
      if (isOdd(exponent))
        x = x * base;
      if (cast<SignedInteger>(1) < exponent)
        base = base * base;
    }
  }
  return x;
}

//! Calculate the value of base raised to the power exponent
template <typename Float> inline
constexpr Float pow(const Float base, const Float exponent) noexcept
{
  const Float e = exponent * log(base);
  const Float y = exp(e);
  return y;
}

namespace inner {

/*!
  */
template <int kN, typename Float> inline
constexpr Float estimateNRoot(const Float x) noexcept
{
  static_assert(0 < kN, "The kN isn't positive.");
  int exponent = 0;
  const Float m = frexp(x, &exponent);
  const Float y = ldexp(m, exponent / kN);
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float sqrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = inner::estimateNRoot<2>(x);
  for (Float pre_y = cast<Float>(0.0); !isAlmostEqual(y, pre_y);) {
    constexpr Float k = cast<Float>(0.5);
    pre_y = y;
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
  Float y = inner::estimateNRoot<3>(x);
  for (Float pre_y = cast<Float>(0.0); !isAlmostEqual(y, pre_y);) {
    constexpr Float k = cast<Float>(2.0);
    const Float y3 = power<3>(y);
    pre_y = y;
    y = y * ((y3 + k * x) / (k * y3 + x));
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
  Float y = x;
  Float pre_y = cast<Float>(0.0);
  Float t = x;
  for (int iteration = 2; !isAlmostEqual(y, pre_y); ++iteration) {
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
  constexpr int n = 4;
  constexpr Float k1 = log(cast<Float>(2.0));
  constexpr Float k2 = power<n>(cast<Float>(0.5));
  constexpr Float l2u = cast<Float>(0.69314718055966295651160180568695068359375);
  constexpr Float l2l = cast<Float>(0.28235290563031577122588448175013436025525412068e-12);

  const int q = cast<int>(invert(k1) * x);

  // Argument reduction
  x = x - cast<Float>(q) * l2u - cast<Float>(q) * l2l;
  x = x * k2;

  Float y = inner::exp(x);
  for (int iteration = 0; iteration < n; ++iteration)
    y = (cast<Float>(2.0) + y) * y;
  y = (y + cast<Float>(1.0)) * power(cast<Float>(2.0), q);

  return y;
}

namespace inner {

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
    constexpr Float b = cast<Float>(2.0);
    constexpr Float inv_b = invert(b);
    for (Float pre_y = b, e = inv_b; !isAlmostEqual(y, pre_y); e = inv_b * e) {
      x = power<2>(x);
      if (b < x) {
        pre_y = y;
        y = y + e;
        x = inv_b * x;
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

  int exponent = 0;
  const Float m = cast<Float>(2.0) * frexp(x, &exponent);
  const Float l = cast<Float>(exponent - 1) + inner::log2(m);
  return l;
}

/*!
  */
template <typename Float> inline
constexpr Float log(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(1.0) + inner::exp(cast<Float>(1.0));
  constexpr Float k = invert(log2(base));
  return k * log2(x);
}

/*!
  */
template <typename Float> inline
constexpr Float log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(10.0);
  constexpr Float k = invert(log2(base));
  return k * log2(x);
}

namespace inner {

//! x should be [0, pi/4)
template <typename Float> inline
constexpr std::tuple<Float, Float> sincosImpl(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  // Argument reduction 
  constexpr int n = 3;
  constexpr Float k = power<n>(cast<Float>(0.5));
  x = k * x;

  // Evaluate Taylor series
  Float y = cast<Float>(0.0);
  {
    Float t = cast<Float>(2.0);
    Float pre_y = t;
    x = power<2>(x);
    for (int i = 1; !isAlmostEqual(y, pre_y); ++i) {
      pre_y = y;
      t = t * x / cast<Float>((2 * i) * (2 * i - 1));
      y = isOdd(i) ? y + t : y - t;
    }
  }

  // Apply double-angle formula
  for (int i = 0; i < n; ++i)
    y = (cast<Float>(4.0) - y) * y;
  y = cast<Float>(0.5) * y;

  const Float s = sqrt((cast<Float>(2.0) - y) * y);
  const Float c = cast<Float>(1.0) - y;
  return std::make_tuple(s, c);
}

/*!
  */
template <typename Float> inline
constexpr std::tuple<Float, Float> sincos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  constexpr Float m4_pi = cast<Float>(4.0) / pi<Float>();
  Float a = abs(x);
  const int q = cast<int>(a * m4_pi);
  const Float r = cast<Float>(q + (q & 1));

  // Split pi/4 into three parts
  constexpr Float pi_4_a =
      cast<Float>(0.7853981554508209228515625);
  constexpr Float pi_4_b =
      cast<Float>(0.794662735614792836713604629039764404296875e-8);
  constexpr Float pi_4_c =
      cast<Float>(0.306161699786838294306516483068750264552437361480769e-16);
  a = a - r * pi_4_a - r * pi_4_b - r * pi_4_c;

  auto y = sincosImpl(a);
  auto& s = std::get<0>(y);
  auto& c = std::get<1>(y);

  if (((q + 1) & 2) != 0)
    swap(s, c);
  if (((q & 4) != 0) != (x < 0))
    s = -s;
  if (((q + 2) & 4) != 0)
    c = -c;

  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = inner::sincos(x);
  const auto s = std::get<0>(y);
  return s;
}

/*!
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = inner::sincos(x);
  const Float c = std::get<1>(y);
  return c;
}

/*!
  */
template <typename Float> inline
constexpr Float tan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = inner::sincos(x);
  const Float s = std::get<0>(y);
  const Float c = std::get<1>(y);
  return s / c;
}

namespace inner {

//! Evaluate a series
template <typename Float> inline
constexpr Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float k = x / (cast<Float>(1.0) + power<2>(x));
  Float t = k;
  Float y = k;
  Float pre_y = cast<Float>(0.0);
  for (int i = 1; !isAlmostEqual(y, pre_y); ++i) {
    pre_y = y;
    const Float s = cast<Float>(2.0) * power<2>(cast<Float>(i)) /
                    cast<Float>((2 * i + 1) * i);
    t = t * s * x * k;
    y = y + t;
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
  if (x != y) {
    constexpr Float one = cast<Float>(1.0);
    const Float z = abs(x);
    Float a = (z < one) ? invert(z) : z;

    constexpr Float v1 = sqrt(std::numeric_limits<Float>::max());
    constexpr Float v2 = sqrt(v1);
    const int n = (abs(a) < v2) ? 2 : (abs(a) < v1) ? 1 : 0;
    // Apply cotangent half angle formula
    for (int i = 0; i < n; ++i)
      a = a + sqrt(one + power(a, 2));

    a = invert(a);
    // Evaluate a series
    y = inner::atan(a);

    const Float e = power(cast<Float>(2.0), n);
    y = y * e;

    if (one < z) {
      constexpr Float pi_2 = pi<Float>() / cast<Float>(2.0);
      y = pi_2 - y;
    }
  }
  return isNegative(x) ? -y : y;
}

/*!
  */
template <typename Float> inline
constexpr Float asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);
  constexpr Float pi_2 = pi<Float>() / cast<Float>(2.0);
  Float y = isNegative(x) ? -pi_2 : pi_2;
  if (abs(x) < one) {
    const Float z = x / sqrt((one + x) * (one - x));
    y = atan(z);
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
  constexpr Float pi_2 = pi<Float>() / cast<Float>(2.0);
  Float y = pi_2;
  if (x != cast<Float>(0.0)) {
    const Float z = sqrt((one + x) * (one - x)) / x;
    y = atan(z) + (isNegative(x) ? pi<Float>() : cast<Float>(0.0));
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float frexp(const Float x, int* e) noexcept
{
  using FType = FloatingPointFromBytes<sizeof(Float)>;
  Float y = x;
  constexpr Float zero = cast<Float>(0.0);
  if ((x == zero) || isInf(x) || isNan(x)) {
    *e = 0;
  }
  else {
    *e = cast<int>(FType::makeExponentBits(x) >> FType::significandBitSize()) -
         cast<int>(FType::exponentBias() - 1);
    constexpr Float base = cast<Float>(0.5);
    const Float normalizer = power(base, *e);
    y = normalizer * x;
  }
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float ldexp(const Float x, const int e) noexcept
{
  constexpr Float base = cast<Float>(2.0);
  const Float y = (isInf(x) || isNan(x)) ? x : x * power(base, e);
  return y;
}

} // namespace constant

} // namespace zisc

#endif // ZISC_CONST_MATH_INL_HPP
