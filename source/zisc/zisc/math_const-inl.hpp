/*!
  \file math_const-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_CONST_INL_HPP
#define ZISC_MATH_CONST_INL_HPP

#include "math_const.hpp"
// Standard C++ library
#include <cmath>
#include <limits>
#include <numeric>
#include <tuple>
#include <type_traits>
// Zisc
#include "algorithm.hpp"
#include "error.hpp"
#include "floating_point.hpp"
#include "utility.hpp"
#include "type_traits.hpp"
#include "zisc_config.hpp"

namespace zisc {

namespace constant {

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::pi() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr int64b n = std::numeric_limits<Float>::digits;
  const Float p = calcPi<Float>(n);
  return p;
}

/*!
  \details No detailed description

  \tparam kUlpScale No description.
  \tparam Float No description.
  \param [in] value No description.
  \return No description
  */
template <std::size_t kUlpScale, typename Float> inline
constexpr Float Math::getUlps(const Float value) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float e = cast<Float>(kUlpScale) *
                      std::numeric_limits<Float>::epsilon();
  const Float ulps = e * abs(value);
  return ulps;
}

/*!
  \details No detailed description

  \tparam kUlpScale No description.
  \tparam Arith No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::size_t kUlpScale, typename Arith> inline
constexpr bool Math::isAlmostEqual(const Arith lhs, const Arith rhs) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type");
  if constexpr (kIsFloat<Arith>) {
    const Arith v = abs(lhs - rhs);
    const bool result = (v <= getUlps<kUlpScale>(lhs + rhs)) || isSubnormal(v);
    return result;
  }
  else {
    const bool result = lhs == rhs;
    return result;
  }
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] a No description.
  \param [in] b No description.
  \param [in] c No description.
  \return No description
  */
template <typename Arith> inline
constexpr Arith Math::fma(const Arith a, const Arith b, const Arith c) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type");
  const Arith result = a * b + c;
  return result;
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::gcd(Integer1 m,
                                                           Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return std::gcd(m, n);
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::lcm(Integer1 m,
                                                           Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  return std::lcm(m, n);
}

/*!
  \details No detailed description

  \tparam y No description.
  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <long y, typename Integer> inline
constexpr Integer Math::mod(Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  static_assert(y != 0, "The y is zero.");
  constexpr bool is_power_of_2 = Algorithm::isPowerOf2(y);
  const Integer result = (is_power_of_2)
      ? x & (cast<Integer>(y) - 1)
      : x % cast<Integer>(y);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> Math::sequence(
    const Integer1 m,
    const Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  std::common_type_t<Integer1, Integer2> x = 1;
  for (auto i = m; i < n; ++i)
    x = fma(i, x, x);
  return x;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr Integer Math::factorial(const Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  constexpr Integer start = 1;
  return sequence(start, x);
}

/*!
  \details No detailed description

  \tparam kExponent No description.
  \tparam Arith No description.
  \param [in] base No description.
  \return No description
  */
template <int kExponent, typename Arith> inline
constexpr Arith Math::power(Arith base) noexcept
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
  \details No detailed description

  \tparam Arith No description.
  \tparam SignedInteger No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <typename Arith, typename SignedInteger> inline
constexpr Arith Math::power(Arith base, SignedInteger exponent) noexcept
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

/*!
  \details No detailed description

  \tparam Float description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::pow(const Float base, const Float exponent) noexcept
{
  const Float e = exponent * log(base);
  const Float y = exp(e);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::sqrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = estimateNRoot<2>(x);
  for (Float pre_y = cast<Float>(0.0); !isAlmostEqual(y, pre_y);) {
    constexpr Float k = cast<Float>(0.5);
    pre_y = y;
    y = k * (y + x / y);
  }
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::cbrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float y = estimateNRoot<3>(x);
  for (Float pre_y = cast<Float>(0.0); !isAlmostEqual(y, pre_y);) {
    constexpr Float k = cast<Float>(2.0);
    const Float y3 = power<3>(y);
    pre_y = y;
    y = y * (fma(k, x, y3) / fma(k, y3, x));
  }
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::exp(Float x) noexcept
{
  constexpr Float lon2 = log(cast<Float>(2.0));
  constexpr Float l2u = getL2u<Float>();
  constexpr Float l2l = getL2l<Float>();

  const int q = cast<int>(rint(x * invert(lon2)));
  // Argument reduction
  x = fma(cast<Float>(q), -l2u, x);
  x = fma(cast<Float>(q), -l2l, x);

  // Evaluate a series
  Float y = expImpl(x);

  y = ldexp(y + cast<Float>(1.0), q);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  int exponent = 0;
  const Float m = cast<Float>(2.0) * frexp(x, &exponent);
  const Float l = cast<Float>(exponent - 1) + log2Impl(m);
  return l;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::log(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(1.0) + expImpl(cast<Float>(1.0));
  constexpr Float k = invert(log2(base));
  return k * log2(x);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(10.0);
  constexpr Float k = invert(log2(base));
  return k * log2(x);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = sincos(x);
  const auto s = std::get<0>(y);
  return s;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = sincos(x);
  const Float c = std::get<1>(y);
  return c;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::tan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = sincos(x);
  const Float s = std::get<0>(y);
  const Float c = std::get<1>(y);
  return s / c;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::atan(const Float x) noexcept
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
      a = a + sqrt(fma(a, a, one));

    a = invert(a);
    // Evaluate a series
    y = atanImpl(a);

    const Float e = power(cast<Float>(2.0), n);
    y = y * e;

    if (one < z) {
      constexpr Float pi_2 = cast<Float>(0.5) * pi<Float>();
      y = pi_2 - y;
    }
  }
  return isNegative(x) ? -y : y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::asin(const Float x) noexcept
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
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::acos(const Float x) noexcept
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
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [out] e No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::frexp(const Float x, int* e) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
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
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] e No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::ldexp(const Float x, const int e) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float base = cast<Float>(2.0);
  const Float y = (isInf(x) || isNan(x)) ? x : x * power(base, e);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::rint(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float half = cast<Float>(0.5);
  const int64b y = isNegative(x) ? cast<int64b>(x - half) : cast<int64b>(x + half);
  return cast<Float>(y);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] n No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::calcPi(const int64b n) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  Float x = cast<Float>(2.0);
  for (int64b k = n; 0 < k; --k) {
    const Float t = cast<Float>(2 * k + 1);
    x = fma(cast<Float>(k), x, cast<Float>(2.0) * t) / t;
  }
  return x;
}

/*!
  \details No detailed description

  \tparam kN No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <int kN, typename Float> inline
constexpr Float Math::estimateNRoot(const Float x) noexcept
{
  static_assert(0 < kN, "The kN isn't positive.");
  int exponent = 0;
  const Float m = frexp(x, &exponent);
  const Float y = ldexp(m, exponent / kN);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::expImpl(const Float x) noexcept
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

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getL2u() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float u = 0.693145751953125f;
    return u;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float u = 0.69314718055966295651160180568695068359375;
    return u;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported result type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getL2l() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float l = 1.428606765330187045e-06f;
    return l;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float l = 0.28235290563031577122588448175013436025525412068e-12;
    return l;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported result type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x \a x must be satisfied [1, 2).
  \return No description
  */
template <typename Float> inline
constexpr Float Math::log2Impl(Float x) noexcept
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

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x \a x should be [0, pi/4)
  \return No description
  */
template <typename Float> inline
constexpr std::tuple<Float, Float> Math::sincosImpl(Float x) noexcept
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
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getPiA() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float a = 3.140625f;
    return a;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float a = 3.1415926218032836914;
    return a;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getPiB() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float a = 0.0009670257568359375f;
    return a;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float a = 3.1786509424591713469e-08;
    return a;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getPiC() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float a = 6.2771141529083251953e-07f;
    return a;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float a = 1.2246467864107188502e-16;
    return a;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::getPiD() noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  if constexpr (sizeof(Float) == 4) {
    const Float a = 1.2154201256553420762e-10f;
    return a;
  }
  else if constexpr (sizeof(Float) == 8) {
    const Float a = 1.2736634327021899816e-24;
    return a;
  }
  else {
    static_assert(sizeof(Float) == 0, "Unsupported type is specified.");
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr std::tuple<Float, Float> Math::sincos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");

  constexpr Float m4_pi = cast<Float>(4.0) / pi<Float>();
  Float a = abs(x);
  const int q = cast<int>(a * m4_pi);

  {
    const Float r = cast<Float>(q + (q & 1));
    // Split pi/4 into four parts
    constexpr Float k = cast<Float>(0.25);
    constexpr Float pi_4_a = k * getPiA<Float>();
    constexpr Float pi_4_b = k * getPiB<Float>();
    constexpr Float pi_4_c = k * getPiC<Float>();
    constexpr Float pi_4_d = k * getPiD<Float>();
    a = fma(r, -pi_4_a, a);
    a = fma(r, -pi_4_b, a);
    a = fma(r, -pi_4_c, a);
    a = fma(r, -pi_4_d, a);
  }

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

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float Math::atanImpl(const Float x) noexcept
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

/*!
  \details No detailed description

  \tparam Arith No description.
  \param [in] a No description.
  \param [in] b No description.
  \param [in] c No description.
  \return No description
  */
template <typename Arith> inline
constexpr Arith fma(const Arith a, const Arith b, const Arith c) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>, "Arith isn't arithmetic type");
  const Arith result = Math::fma(a, b, c);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
                                                     Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto result = Math::gcd(m, n);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
                                                     Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto result = Math::lcm(m, n);
  return result;
}

/*!
  \details No detailed description

  \tparam y No description.
  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <long y, typename Integer> inline
constexpr Integer mod(Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  const auto result = Math::mod<y>(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer1 No description.
  \tparam Integer2 No description.
  \param [in] m No description.
  \param [in] n No description.
  \return No description
  */
template <typename Integer1, typename Integer2> inline
constexpr std::common_type_t<Integer1, Integer2> sequence(
    const Integer1 m,
    const Integer2 n) noexcept
{
  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
  const auto result = Math::sequence(m, n);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <typename Integer> inline
constexpr Integer factorial(const Integer x) noexcept
{
  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
  const auto result = Math::factorial(x);
  return result;
}

/*!
  \details No detailed description

  \tparam kExponent No description.
  \tparam Arith No description.
  \param [in] base No description.
  \return No description
  */
template <int kExponent, typename Arith> inline
constexpr Arith power(Arith base) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>,
                "Arith isn't arithmetic type");
  const auto y = Math::power<kExponent>(base);
  return y;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam SignedInteger No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <typename Arith, typename SignedInteger> inline
constexpr Arith power(Arith base, SignedInteger exponent) noexcept
{
  static_assert(std::is_arithmetic_v<Arith>,
                "Arith isn't arithmetic type");
  static_assert(kIsSignedInteger<SignedInteger>,
                "SignedInteger isn't signed integer type.");
  const auto y = Math::power(base, exponent);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <typename Float> inline
constexpr Float pow(const Float base, const Float exponent) noexcept
{
  const auto y = Math::pow(base, exponent);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float sqrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::sqrt(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float cbrt(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::cbrt(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float exp(Float x) noexcept
{
  const auto y = Math::exp(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float log2(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log2(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float log(Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float log10(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::log10(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float sin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::sin(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float cos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::cos(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float tan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::tan(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float atan(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::atan(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float asin(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::asin(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float acos(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::acos(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [out] e No description.
  \return No description
  */
template <typename Float> inline
constexpr Float frexp(const Float x, int* e) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::frexp(x, e);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] e No description.
  \return No description
  */
template <typename Float> inline
constexpr Float ldexp(const Float x, const int e) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::ldexp(x, e);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Float> inline
constexpr Float rint(const Float x) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const auto y = Math::rint(x);
  return y;
}

} // namespace constant

} // namespace zisc

#endif // ZISC_MATH_CONST_INL_HPP
