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
#include "floating_point_bit.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

namespace constant {

namespace inner {

template <typename Float> inline
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
template <typename Float> inline
constexpr Float pi() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr int64 n = 4 * std::numeric_limits<Float>::digits10;
  const Float p = inner::calcPi<Float>(n);
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
template <std::size_t kUlpScale, typename Float> inline
constexpr bool isAlmostEqual(const Float lhs, const Float rhs) noexcept
{
  static_assert(std::is_arithmetic_v<Float>, "Float isn't arithmetic type");
  bool result = false;
  if constexpr (kIsFloat<Float>) {
    const Float v = abs(lhs - rhs);
    result = (v <= getUlps<kUlpScale>(lhs + rhs)) || isSubnormal(v);
  }
  else {
    result = lhs == rhs;
  }
  return result;
}

/*!
  */
template <std::size_t kUlpScale, typename Float> inline
constexpr bool isAlmostEqualToZero(const Float value) noexcept
{
  static_assert(std::is_arithmetic_v<Float>, "Float isn't arithmetic type");
  constexpr Float zero = cast<Float>(0);
  bool result = false;
  if constexpr (kIsFloat<Float>) {
    const Float v = abs(value);
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
  return (x != zero) ? one / x : zero;
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
  Arithmetic x = cast<Arithmetic>(0);
  // Check if base is zero
  if (x != base) {
    x = cast<Arithmetic>(1);
    // Inverse pow
    if constexpr (kIsFloat<Arithmetic> && isNegative(kExponent))
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
template <typename Arithmetic, typename SignedInteger> inline
constexpr Arithmetic pow(Arithmetic base, SignedInteger exponent) noexcept
{
  static_assert(std::is_arithmetic_v<Arithmetic>,
                "Arithmetic isn't arithmetic type");
  static_assert(kIsSignedInteger<SignedInteger>,
                "SignedInteger isn't signed integer type.");
  Arithmetic x = cast<Arithmetic>(0);
  // Check if base is zero
  if (x != base) {
    x = cast<Arithmetic>(1);
    // Inverse pow
    if constexpr (kIsFloat<Arithmetic>) {
      if (isNegative(exponent))
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

namespace inner {

/*!
  */
template <int kN, typename Float> inline
constexpr Float estimateNRoot(const Float x) noexcept
{
  static_assert(0 < kN, "The kN isn't positive.");

  using FloatBit = FloatingPointBit<Float>;

  int exponent = 
      cast<int>(FloatBit::getExponentBits(x) >> FloatBit::significandBitSize()) -
      cast<int>(FloatBit::exponentBias());
  exponent = exponent / kN;

  const Float y = pow(cast<Float>(2.0), exponent);
  return isNegative(x) ? -y : y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float sqrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = inner::estimateNRoot<2>(x);
  for (Float pre_y = x; !isAlmostEqual(y, pre_y);) {
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
  for (Float pre_y = x; !isAlmostEqual(y, pre_y);) {
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
  Float y = cast<Float>(1.0) + x;
  Float pre_y = cast<Float>(1.0);
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
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  int exponent = 1;
  while (cast<Float>(1.0) < abs(x)) {
    x = cast<Float>(0.5) * x;
    exponent = exponent << 1;
  }
  Float y = inner::exp(x);
  y = pow(y, exponent);
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
    constexpr Float base = cast<Float>(2.0);
    constexpr Float inv_base = invert(base);
    for (Float pre_y = base, element = inv_base;
         !isAlmostEqual(y, pre_y);
         element = inv_base * element) {
      x = power<2>(x);
      if (base < x) {
        pre_y = y;
        y = y + element;
        x = inv_base * x;
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

  using FloatBit = FloatingPointBit<Float>;
  const int exponent = 
      cast<int>(FloatBit::getExponentBits(x) >> FloatBit::significandBitSize()) -
      cast<int>(FloatBit::exponentBias());
  const Float f = pow(cast<Float>(0.5), exponent);

  const Float l = cast<Float>(exponent) + inner::log2(f * x);
  return l;
}

/*!
  */
template <typename Float> inline
constexpr Float log(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = exp(cast<Float>(1.0));
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

/*!
  */
template <typename Float> inline
constexpr Float sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(1.0),
        pre_y = cast<Float>(1.0) - x;
  Float t = y;
  for (int iteration = 1; !isAlmostEqual(y, pre_y); ++iteration) {
    pre_y = y;
    t = t * power<2>(x) / cast<Float>((2 * iteration) * (2 * iteration + 1));
    y = isOdd(iteration) ? y - t : y + t;
  }
  y = x * y;
  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = cast<Float>(1.0),
        pre_y = cast<Float>(1.0) - x;
  Float t = y;
  for (int iteration = 1; !isAlmostEqual(y, pre_y); ++iteration) {
    pre_y = y;
    t = t * power<2>(x) / cast<Float>((2 * iteration - 1) * (2 * iteration));
    y = isOdd(iteration) ? y - t : y + t;
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
  Float theta = abs(x);
  // Rotate theta
  constexpr Float double_pi = cast<Float>(2.0) * pi<Float>();
  constexpr Float half_pi = cast<Float>(0.5) * pi<Float>();
  if (double_pi <= theta) {
    constexpr Float k = invert(double_pi);
    const uint64 e = cast<uint64>(k * theta) - 1;
    theta = theta - cast<Float>(e) * double_pi;
  }
  if (pi<Float>() <= theta) {
    theta = theta - double_pi;
    theta = (theta <= -half_pi) ? -pi<Float>() - theta : theta;
  }
  else if (half_pi <= theta) {
    theta = pi<Float>() - theta;
  }

  // Compute sin
  Float y = inner::sin(theta);
  y = isNegative(x) ? -y : y;

  return y;
}

/*!
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float theta = abs(x);
  // Rotate theta
  constexpr Float double_pi = cast<Float>(2.0) * pi<Float>();
  if (double_pi <= theta) {
    constexpr Float k = invert(double_pi);
    const uint64 e = cast<uint64>(k * theta) - 1;
    theta = theta - cast<Float>(e) * double_pi;
  }
  if (pi<Float>() <= theta)
    theta = double_pi - theta;

  // Compute cos
  const Float y = inner::cos(theta);

  return y;
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
  const Float k = x / (cast<Float>(1.0) + power<2>(x));

  Float y = cast<Float>(1.0),
        pre_y = k;

  Float t = cast<Float>(1.0);
  for (int iteration = 1; !isAlmostEqual(y, pre_y); ++iteration) {
    pre_y = y;
    const Float s = x * (cast<Float>(2 * iteration) /
                         cast<Float>(2 * iteration + 1));
    t = t * k * s;
    y = y + t;
  }
  y = k * y;
  return y;
}

} // namespace inner

/*!
  */
template <typename Float> inline
constexpr Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = abs(x);

  constexpr Float half_pi = cast<Float>(0.5) * pi<Float>();
  const Float theta = (cast<Float>(1.0) < y)
      ? half_pi - inner::atan(invert(y))
      : inner::atan(y);

  return isNegative(x) ? -theta : theta;
}

/*!
  */
template <typename Float> inline
constexpr Float asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);

  const Float y = x / (one + sqrt(one - power<2>(x)));
  const Float theta = cast<Float>(2.0) * atan(y);

  return theta;
}

/*!
  */
template <typename Float> inline
constexpr Float acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float one = cast<Float>(1.0);

  Float theta = pi<Float>();
  if (-one < x) {
    const Float y = sqrt(one - power<2>(x)) / (one + x);
    theta = cast<Float>(2.0) * atan(y);
  }

  return theta;
}

/*!
  */
template <typename Float> inline
constexpr bool isFinite(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = !(isInf(x) || isNan(x));
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isInf(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = (x == std::numeric_limits<Float>::infinity()) ||
                      (x == -std::numeric_limits<Float>::infinity());
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isNan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const bool result = x != x;
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isNormal(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float d = abs(x);
  const bool result = (std::numeric_limits<Float>::min() <= d) &&
                      (d <= std::numeric_limits<Float>::max());
  return result;
}

/*!
  */
template <typename Float> inline
constexpr bool isSubnormal(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float zero = cast<Float>(0.0);
  const Float d = abs(x);
  const bool result = (zero < d) && (d < std::numeric_limits<Float>::min());
  return result;
}

} // namespace constant

} // namespace zisc

#endif // ZISC_CONST_MATH_INL_HPP
