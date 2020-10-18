/*!
  \file math-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_INL_HPP
#define ZISC_MATH_INL_HPP

#include "math.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <type_traits>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::pi() noexcept
{
  constexpr int64b n = std::numeric_limits<Float>::digits;
  Float x = cast<Float>(2.0);
  for (int64b k = n; 0 < k; --k) {
    const Float t = cast<Float>(2 * k + 1);
    x = (cast<Float>(k) * x + cast<Float>(2.0) * t) / t;
  }
  return x;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam Int No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <Arithmetic Arith, Integer Int> inline
constexpr Arith Math::pow(Arith base, Int exponent) noexcept
{
  constexpr Arith zero = cast<Arith>(0);
  constexpr Arith one = cast<Arith>(1);
  constexpr Arith inf = std::numeric_limits<Arith>::infinity();
  Arith x = (exponent == 0) ? one : isNegative(exponent) ? inf : zero;
  if (base != zero) {
    x = one;
    // Inverse pow
    if(isNegative(exponent))
      base = invert(base);
    // Compute pow
    for (exponent = abs(exponent); 0 < exponent; exponent = exponent >> 1) {
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
  \tparam Float No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <Arithmetic Arith, FloatingPoint Float> inline
constexpr Arith Math::pow(const Arith base, const Float exponent) noexcept
{
  using FLimit = std::numeric_limits<Float>;
  constexpr Float zero = cast<Float>(0.0);
  constexpr Float one = cast<Float>(1.0);

  const bool y_isint = Impl::isInt(exponent);
  const bool y_isodd = y_isint && Impl::isOdd(exponent);

  const Float x = cast<Float>(base);
  const Impl::F2<Float> d = Impl::mul(Impl::log(Impl::abs(x)), exponent);
  Float result = Impl::exp(d);
  if (d.x_ > cast<Float>(709.78271114955742909217217426))
    result = FLimit::infinity();

  result = Impl::isNan(result) ? FLimit::infinity() : result;
  result *= isNegative(x) ? (y_isodd ? -one : one) : one; //! \todo Nan check

  const Float efx = Impl::mulsign(Impl::abs(x) - one, exponent);
  if (Impl::isInf(exponent))
    result = isNegative(efx) ? zero : ((efx == zero) ? one : FLimit::infinity());
  if (Impl::isInf(x) || x == zero)
    result = (y_isodd ? Impl::sign(x) : one) * ((x == zero) ? -exponent : exponent) < zero ? zero : FLimit::infinity();
  //! \todo Nan check
  if (exponent == zero || x == one)
    result = one;

  return result;
}

///*!
//  \details No detailed description
//
//  \tparam kExponent No description.
//  \tparam Arithmetic No description.
//  \param [in] base No description.
//  \return No description
//  */
//template <int kExponent, typename Arithmetic> inline
//constexpr Arithmetic Math::power(Arithmetic base) noexcept
//{
//  static_assert(std::is_arithmetic_v<Arithmetic>, 
//                "Arithmetic isn't arithmetic type");
//  return constant::power<kExponent>(base);
//}
//
///*!
//  \details No detailed description
//
//  \tparam Arithmetic No description.
//  \tparam SignedInteger No description.
//  \param [in] base No description.
//  \param [in] exponent No description.
//  \return No description
//  */
//template <typename Arithmetic, typename SignedInteger>
//constexpr Arithmetic Math::power(const Arithmetic base,
//                                 const SignedInteger exponent) noexcept
//{
//  return constant::power(base, exponent);
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] base No description.
//  \param [in] exponent No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::pow(const Float base, const Float exponent) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathPowUsed()
//      ? Stl::pow(base, exponent)
//      : Zisc::pow(base, exponent);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::sqrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathSqrtUsed()
//      ? Stl::sqrt(n)
//      : Zisc::sqrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::cbrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathCbrtUsed()
//      ? Stl::cbrt(n)
//      : Zisc::cbrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::exp(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathExpUsed()
//      ? Stl::exp(n)
//      : Zisc::exp(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::log(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathLogUsed()
//      ? Stl::log(x)
//      : Zisc::log(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::log2(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathLogUsed()
//      ? Stl::log2(x)
//      : Zisc::log2(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::log10(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathLogUsed()
//      ? Stl::log10(x)
//      : Zisc::log10(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::sin(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathTrigonometricUsed()
//      ? Stl::sin(theta)
//      : Zisc::sin(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::cos(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathTrigonometricUsed()
//      ? Stl::cos(theta)
//      : Zisc::cos(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::tan(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Config::isStlMathTrigonometricUsed()
//      ? Stl::tan(theta)
//      : Zisc::tan(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::asin(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = Config::isStlMathInvTrigonometricUsed()
//      ? Stl::asin(x)
//      : Zisc::asin(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::acos(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = Config::isStlMathInvTrigonometricUsed()
//      ? Stl::acos(x)
//      : Zisc::acos(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::atan(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
//  const auto y = Config::isStlMathInvTrigonometricUsed()
//      ? Stl::atan(x)
//      : Zisc::atan(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] base No description.
//  \param [in] exponent No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Stl::pow(const Float base, const Float exponent) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::pow(base, exponent);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Stl::sqrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::sqrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Stl::cbrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::cbrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Stl::exp(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::exp(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::log(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::log(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::log2(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::log2(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::log10(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::log10(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::sin(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::sin(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::cos(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::cos(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::tan(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = std::tan(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::asin(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = std::asin(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::acos(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = std::acos(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Stl::atan(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
//  const auto y = std::atan(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] base No description.
//  \param [in] exponent No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Zisc::pow(const Float base, const Float exponent) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = base;
//  static_cast<void>(exponent);
//  static_assert(Config::isStlMathPowUsed(),
//                "The zisc pow isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Zisc::sqrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = n;
//  static_assert(Config::isStlMathSqrtUsed(),
//                "The zisc sqrt isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Zisc::cbrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = n;
//  static_assert(Config::isStlMathCbrtUsed(),
//                "The zisc cbrt isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::Zisc::exp(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = n;
//  static_assert(Config::isStlMathExpUsed(),
//                "The zisc exp isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::log(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = x;
//  static_assert(Config::isStlMathLogUsed(),
//                "The zisc log isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::log2(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = x;
//  static_assert(Config::isStlMathLogUsed(),
//                "The zisc log2 isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::log10(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = x;
//  static_assert(Config::isStlMathLogUsed(),
//                "The zisc log10 isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::sin(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = theta;
//  static_assert(Config::isStlMathTrigonometricUsed(),
//                "The zisc sin isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::cos(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = theta;
//  static_assert(Config::isStlMathTrigonometricUsed(),
//                "The zisc cos isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::tan(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = theta;
//  static_assert(Config::isStlMathTrigonometricUsed(),
//                "The zisc tan isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::asin(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = x;
//  static_assert(Config::isStlMathInvTrigonometricUsed(),
//                "The zisc asin isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::acos(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = x;
//  static_assert(Config::isStlMathInvTrigonometricUsed(),
//                "The zisc acos isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float Math::Zisc::atan(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
//  const auto y = x;
//  static_assert(Config::isStlMathInvTrigonometricUsed(),
//                "The zisc atan isn't implemented yet.");
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] a No description.
//  \param [in] b No description.
//  \param [in] c No description.
//  \return No description
//  */
//template <typename Float> inline
//std::tuple<std::array<Float, 2>, uint> Math::solveQuadratic(
//    const Float a, 
//    const Float b, 
//    const Float c) noexcept
//{
//  constexpr Float min_x = std::numeric_limits<Float>::lowest();
//  std::array<Float, 2> x = {{min_x, min_x}};
//  uint n = 0;
//
//  const Float discriminant = b * b - 4.0 * a * c;
//  if (constant::Math::isAlmostEqual(discriminant, cast<Float>(0.0))) {
//    x[0] = -b / (2.0 * a);
//    n = 1;
//  }
//  else if (0.0 < discriminant) {
//    const Float root_d = sqrt(discriminant);
//    x[0] = (-b + root_d) / (2.0 * a);
//    x[1] = (-b - root_d) / (2.0 * a);
//    n = 2;
//  }
//  return std::make_tuple(x, n);
//}
//
///*!
//  \details Solve a cubic equation using "cubic reduction" method.
//
//  \tparam Float No description.
//  \param [in] b No description.
//  \param [in] c No description.
//  \param [in] d No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::solveCubicOne(const Float b, const Float c, const Float d) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//
//  const Float k = invert(-3.0) * b;
//
//  const Float p = (b * k + c);
//  const Float q = (((2.0 / 3.0) * b * k + c) * k + d);
//  const Float y = solveCubicOneY(p, q);
//  const Float x = y  +  k;
//  return x;
//}
//
///*!
//  \details Solve a cubic equation using "cubic reduction" method.
//
//  \tparam Float No description.
//  \param [in] a No description.
//  \param [in] b No description.
//  \param [in] c No description.
//  \param [in] d No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::solveCubicOne(const Float a,
//                          const Float b,
//                          const Float c,
//                          const Float d) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//
//  ZISC_ASSERT(a != 0.0, "The a is zero.");
//  const Float inv_a = invert(a);
//  const Float k = invert(-3.0) * b * inv_a;
//
//  const Float p = (b * k + c) * inv_a;
//  const Float q = (((2.0 / 3.0) * b * k + c) * k + d) * inv_a;
//  const Float y = solveCubicOneY(p, q);
//  const Float x = y  +  k;
//  return x;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] a No description.
//  \param [in] b No description.
//  \param [in] c No description.
//  \param [in] d No description.
//  \param [in] e No description.
//  \return No description
//  */
//template <typename Float> inline
//std::tuple<std::array<Float, 4>, uint> Math::solveQuartic(
//    const Float a, 
//    const Float b, 
//    const Float c, 
//    const Float d, 
//    const Float e) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//
//  ZISC_ASSERT(a != 0.0, "The a is zero.");
//  const Float inv_a = 1.0 / a;
//  const Float k = invert(-4.0) * b * inv_a;
//
//  const Float p = ((3.0 / 2.0) * b * k + c) * inv_a;
//  const Float q = ((b * k + c) * 2.0 * k + d) * inv_a;
//  const Float r = ((((3.0 / 4.0) * b * k + c) * k + d) * k + e) * inv_a;
//  const Float z = solveCubicOne(-0.5 * p, -r, invert(8.0) * (4 * p * r - q * q));
//
//  uint n = 0;
//  std::array<Float, 4> x;
//
//  const Float f2 = 2.0 * z - p;
//  if (0.0 <= f2) {
//    constexpr Float min_x = std::numeric_limits<Float>::lowest();
//    const Float f = sqrt(f2);
//    std::array<Float, 2> x1 = {{min_x, min_x}};
//    const Float d1 = -(2.0 * (z + (q / f)) + p);
//    if (0.0 <= d1) {
//      const Float root_d1 = sqrt(d1);
//      x1[0] = 0.5 * (f + root_d1) + k;
//      x1[1] = 0.5 * (f - root_d1) + k;
//      n += 2;
//    }
//    std::array<Float, 2> x2 = {{min_x, min_x}};
//    const Float d2 = -(2.0 * (z - (q / f)) + p);
//    if (0.0 <= d2) {
//      const Float root_d2 = sqrt(d2);
//      x2[0] = 0.5 * (-f + root_d2) + k;
//      x2[1] = 0.5 * (-f - root_d2) + k;
//      n += 2;
//    }
//    sortQuarticResults(x1, x2, x);
//  }
//  return std::make_tuple(x, n);
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] p No description.
//  \param [in] q No description.
//  \return No description
//  */
//template <typename Float> inline
//Float Math::solveCubicOneY(const Float p, const Float q) noexcept
//{
//  Float y;
//  if (constant::Math::isAlmostEqual(p, cast<Float>(0.0))) {
//    y = cbrt(-q);
//  }
//  else if (constant::Math::isAlmostEqual(q, cast<Float>(0.0))) {
//    y = sqrt(-p);
//  }
//  else {
//    const Float discriminant = power<2>(q) + (4.0 / 27.0) * power<3>(p);
//    if (0.0 <= discriminant) {
//      const Float z = cbrt(0.5 * (sqrt(discriminant) - q));
//      y = z - (p / (3.0 * z));
//    }
//    else {
//      ZISC_ASSERT(p < 0.0, "The p is positive: ", p);
//      const Float r = sqrt(invert(-3.0) * p);
//      const Float cos_theta = clamp(q / (-2.0 * power<3>(r)), -1.0, 1.0);
//      const Float phi = (invert(3.0)) * acos(cos_theta);
//      y = 2.0 * r * cos(phi);
//    }
//  }
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x1 No description.
//  \param [in] x2 No description.
//  \param [out] x No description.
//  */
//template <typename Float> inline
//void Math::sortQuarticResults(const std::array<Float, 2>& x1,
//                              const std::array<Float, 2>& x2,
//                              std::array<Float, 4>& x) noexcept
//{
//  x[0] = max(x1[0], x2[0]);
//  const Float tmp1 = min(x1[0], x2[0]);
//  const Float tmp2 = max(x1[1], x2[1]);
//  x[1] = max(tmp1, tmp2);
//  x[2] = min(tmp1, tmp2);
//  x[3] = min(x1[1], x2[1]);
//}
//
///*!
//  \details No detailed description
//
//  \tparam Integer1 No description.
//  \tparam Integer2 No description.
//  \param [in] m No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Integer1, typename Integer2> inline
//constexpr std::common_type_t<Integer1, Integer2> gcd(
//    Integer1 m,
//    Integer2 n) noexcept
//{
//  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
//  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
//  const auto y = Math::gcd(m, n);
//  return y;
//}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::add(const Float x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x + y.x_};
  result.y_ = x - result.x_ + y.x_ + y.y_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::add(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x_ + y.x_};
  result.y_ = x.x_ - result.x_ + y.x_ + x.y_ + y.y_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::add2(const Float x, const Float y) noexcept -> F2<Float>
{
  F2<Float> result{x + y};
  const Float v = result.x_ - x;
  result.y_ = (x - (result.x_ - v)) + (y - v);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::add2(const F2<Float> x, const Float y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x_ + y};
  const Float v = result.x_ - x.x_;
  result.y_ = (x.x_ - (result.x_ - v)) + (y - v);
  result.y_ += x.y_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::add2(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x_ + y.x_};
  const Float v = result.x_ - x.x_;
  result.y_ = (x.x_ - (result.x_ - v)) + (y.x_ - v);
  result.y_ += x.y_ + y.y_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::div(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  const Float t = invert(y.x_);
  const Float yh = upper(y.x_);
  const Float yl = y.x_ - yh;
  const Float th = upper(t);
  const Float tl = t - th;
  const Float xhh = upper(x.x_);
  const Float xhl = x.x_ - xhh;

  F2<Float> q{t * x.x_};
  const Float u = -q.x_ + xhh * th + xhh * tl + xhl * th + xhl * tl +
                   q.x_ * (cast<Float>(1.0) - yh * th - yh * tl - yl * th - yl * tl);
  q.y_ = t * (x.y_ - q.x_ * y.y_) + u;
  return q;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::mul(const F2<Float> x, const Float y) noexcept
    -> F2<Float>
{
  const Float xh = upper(x.x_);
  const Float xl = x.x_ - xh;
  const Float yh = upper(y);
  const Float yl = y - yh;

  F2<Float> result{x.x_ * y};
  result.y_ = xh * yh - result.x_ + xl * yh + xh * yl + xl * yl + x.y_ * y;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::mul(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  const Float xh = upper(x.x_);
  const Float xl = x.x_ - xh;
  const Float yh = upper(y.x_);
  const Float yl = y.x_ - yh;

  F2<Float> result{x.x_ * y.x_};
  result.y_ = xh * yh - result.x_ + xl * yh + xh * yl + xl * yl + x.x_ * y.y_ + x.y_ * y.x_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::normalize(const F2<Float> x) noexcept -> F2<Float>
{
  F2<Float> result{x.x_ + x.y_};
  result.y_ = x.x_ - result.x_ + x.y_;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] s No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::scale(const F2<Float> x, const Float s) noexcept
    -> F2<Float>
{
  const F2<Float> result{s * x.x_, s * x.y_};
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename ReturnType, FloatingPoint Float> inline
constexpr ReturnType Math::Impl::squ(const F2<Float> x) noexcept
{
  const Float xh = upper(x.x_);
  const Float xl = x.x_ - xh;
  ReturnType y;
  if constexpr (std::is_scalar_v<ReturnType>) {
    y = xh * x.y_ + xh * x.y_ + xl * xl + (xh * xl + xh * xl) + xh * xh;
  }
  else {
    y.x_ = x.x_ * x.x_;
    y.y_ = xh * xh - y.x_ + (xh + xh) * xl + xl * xl + x.x_ * (x.y_ + x.y_);
  }
  return y;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
constexpr double Math::Impl::ExpDConstants::get(const std::size_t index) noexcept
{
  const double c = (index == 0) ? 0.500000000000000999200722 :
                   (index == 1) ? 0.166666666666666740681535 :
                   (index == 2) ? 0.0416666666665409524128449 :
                   (index == 3) ? 0.00833333333332371417601081 :
                   (index == 4) ? 0.0013888888939977128960529 :
                   (index == 5) ? 0.000198412698809069797676111 :
                   (index == 6) ? 2.48014973989819794114153e-05 :
                   (index == 7) ? 2.75572496725023574143864e-06 :
                   (index == 8) ? 2.76286166770270649116855e-07 :
                   (index == 9) ? 2.51069683420950419527139e-08 : 0.0;
  return c;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
constexpr double Math::Impl::LogDConstants::get(const std::size_t index) noexcept
{
  const double c = (index == 0) ? 0.400000000000000077715612 :
                   (index == 1) ? 0.285714285714249172087875 :
                   (index == 2) ? 0.222222222230083560345903 :
                   (index == 3) ? 0.181818180850050775676507 :
                   (index == 4) ? 0.153846227114512262845736 :
                   (index == 5) ? 0.13332981086846273921509 :
                   (index == 6) ? 0.117754809412463995466069 :
                   (index == 7) ? 0.103239680901072952701192 :
                   (index == 8) ? 0.116255524079935043668677 : 0.0;
  return c;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::ln2() noexcept
{
  const Float k = fvalue<Float>(
      1.442695040888963407359924681001892137426645954152985934135449406931f,
      1.442695040888963407359924681001892137426645954152985934135449406931);
  return k;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::l2l() noexcept
{
  const Float k = fvalue<Float>(
      1.428606765330187045e-06f,
      .28235290563031577122588448175013436025525412068e-12);
  return k;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::l2u() noexcept
{
  const Float k = fvalue<Float>(
      0.693145751953125f,
      .69314718055966295651160180568695068359375);
  return k;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly2(const Float x) noexcept
{
  constexpr Float y = Constants::get(coffset, 1);
  constexpr Float z = Constants::get(coffset, 0);
  const Float result = mla(x, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly3(const Float x, const Float x2) noexcept
{
  constexpr Float y = Constants::get(coffset + 2);
  const Float z = poly2<Constants, coffset>(x);
  const Float result = mla(x2, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly4(const Float x, const Float x2) noexcept
{
  const Float y = poly2<Constants, coffset + 2>(x);
  const Float z = poly2<Constants, coffset + 0>(x);
  const Float result = mla(x2, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly5(const Float x,
                                  const Float x2,
                                  const Float x4) noexcept
{
  constexpr Float y = Constants::get(coffset + 4);
  const Float z = poly4<Constants, coffset>(x, x2);
  const Float result = mla(x4, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly6(const Float x,
                                  const Float x2,
                                  const Float x4) noexcept
{
  const Float y = poly2<Constants, coffset + 4>(x);
  const Float z = poly4<Constants, coffset + 0>(x, x2);
  const Float result = mla(x4, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly7(const Float x,
                                  const Float x2,
                                  const Float x4) noexcept
{
  const Float y = poly3<Constants, coffset + 4>(x, x2);
  const Float z = poly4<Constants, coffset + 0>(x, x2);
  const Float result = mla(x4, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly8(const Float x,
                                  const Float x2,
                                  const Float x4) noexcept
{
  const Float y = poly4<Constants, coffset + 4>(x, x2);
  const Float z = poly4<Constants, coffset + 0>(x, x2);
  const Float result = mla(x4, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly9(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept
{
  constexpr Float y = Constants::get(coffset + 8);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly10(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly2<Constants, coffset + 8>(x);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly11(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly3<Constants, coffset + 8>(x, x2);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly12(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly4<Constants, coffset + 8>(x, x2);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly13(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly5<Constants, coffset + 8>(x, x2, x4);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly14(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly6<Constants, coffset + 8>(x, x2, x4);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly15(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly7<Constants, coffset + 8>(x, x2, x4);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly16(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8) noexcept
{
  const Float y = poly8<Constants, coffset + 8>(x, x2, x4);
  const Float z = poly8<Constants, coffset + 0>(x, x2, x4);
  const Float result = mla(x8, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \param [in] x16 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly17(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8,
                                   const Float x16) noexcept
{
  constexpr Float y = Constants::get(coffset + 16);
  const Float z = poly16<Constants, coffset + 0>(x, x2, x4, x8);
  const Float result = mla(x16, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \param [in] x16 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly18(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8,
                                   const Float x16) noexcept
{
  const Float y = poly2<Constants, coffset + 16>(x);
  const Float z = poly16<Constants, coffset + 0>(x, x2, x4, x8);
  const Float result = mla(x16, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Constants No description.
  \tparam Float No description.
  \param [in] x No description.
  \param [in] x2 No description.
  \param [in] x4 No description.
  \param [in] x8 No description.
  \param [in] x16 No description.
  \return No description
  */
template <typename Constants, std::size_t coffset, FloatingPoint Float> inline
constexpr Float Math::Impl::poly19(const Float x,
                                   const Float x2,
                                   const Float x4,
                                   const Float x8,
                                   const Float x16) noexcept
{
  const Float y = poly3<Constants, coffset + 16>(x, x2);
  const Float z = poly16<Constants, coffset + 0>(x, x2, x4, x8);
  const Float result = mla(x16, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::abs(const Float x) noexcept
{
  const Float y = isNegative(x) ? -x : x;
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::copysign(const Float x, const Float y) noexcept
{
  using BitType = BitT<Float>;
  const BitType xi = bit_cast<BitType>(x);
  const BitType yi = bit_cast<BitType>(y);
  constexpr BitType n_bits = std::numeric_limits<BitType>::digits;
  constexpr BitType mask = cast<BitType>(1ull << (n_bits - 1));
  const BitType zi = (xi & ~mask) ^ (yi & mask);
  const Float z = bit_cast<Float>(zi);
  return z;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::exp(const F2<Float> x) noexcept
{
  const int q = cast<int>(rint((x.x_ + x.y_) * ln2<Float>()));
  F2<Float> s = add2(x, cast<Float>(q) * -l2u<Float>());
  s = add2(s, cast<Float>(q) * -l2l<Float>());
  s = normalize(s);

  Float u = cast<Float>(0.0);
  F2<Float> t;
  if constexpr (sizeof(Float) == 4) {
    u = 0.00136324646882712841033936f;
    u = mla(u, s.x_, 0.00836596917361021041870117f);
    u = mla(u, s.x_, 0.0416710823774337768554688f);
    u = mla(u, s.x_, 0.166665524244308471679688f);
    u = mla(u, s.x_, 0.499999850988388061523438f);
    t = add(s, mul(squ<F2<Float>>(s), u));
    t = add(1.f, t);
  }
  else {
    const Float s2 = s.x_ * s.x_;
    const Float s4 = s2 * s2;
    const Float s8 = s4 * s4;
    u = poly10<ExpDConstants, 0>(s.x, s2, s4, s8);
    t = add(1.0, s);
    t = add(t, mul(squ<F2<Float>>(s), u));
  }
  u = ldexp(t.x_ + t.y_, q);
  if (x.x_ < fvalue<Float>(-104.0f, -1000.0))
    u = cast<Float>(0.0);
  return u;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam Arith1 No description.
  \tparam Arith2 No description.
  \param [in] v1 No description.
  \param [in] v2 No description.
  \return No description
  */
template <FloatingPoint Float, Arithmetic Arith1, Arithmetic Arith2> inline
constexpr auto Math::Impl::fvalue(const Arith1 v1, const Arith2 v2) noexcept
{
  if constexpr (sizeof(Float) == 4)
    return v1;
  else
    return v2;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::ilogb2(const Float x) noexcept -> IntT<Float>
{
  using IntType = IntT<Float>;
  using FLimit = std::numeric_limits<Float>;
  constexpr IntType mask = fvalue<Float>(0xff, 0x7ff);
  constexpr IntType k = fvalue<Float>(0x7f, 0x3ff);
  const IntType expt = bit_cast<IntType>(x) >> (FLimit::digits - 1);
  const IntType y = (expt & mask) - k;
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr bool Math::Impl::isInf(const Float x) noexcept
{
  using FLimit = std::numeric_limits<Float>;
  const bool result = (x == FLimit::infinity()) || (x == -FLimit::infinity());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr bool Math::Impl::isInt(const Float x) noexcept
{
  Float d = x;
  if constexpr (sizeof(Float) == 8)
  {
    constexpr Float t = cast<Float>(1ull << 31ull);
    d = x - t * cast<Float>(cast<int>(x * invert(t)));
  }
  constexpr Float k = cast<Float>(1ull << std::numeric_limits<Float>::digits);
  const bool result = (d == cast<Float>(cast<int>(d))) || (k <= abs(x));
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr bool Math::Impl::isNan(const Float x) noexcept
{
  const bool result = x != x;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr bool Math::Impl::isOdd(const Float x) noexcept
{
  Float d = x;
  if constexpr (sizeof(Float) == 8) {
    constexpr Float t = cast<Float>(1ull << 31ull);
    d = x - t * cast<Float>(cast<int>(x * invert(t)));
  }
  constexpr Float k = cast<Float>(1ull << std::numeric_limits<Float>::digits);
  const bool result = ((1 & cast<int>(d)) != 0) && (abs(x) < k);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] e No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::ldexp(Float x, int e) noexcept
{
  using IntType = IntT<Float>;
  int m = e >> 31;
  m = (((m + e) >> fvalue<Float>(6, 9)) - m) << fvalue<Float>(4, 7);
  e = e - (m << 2);
  m += fvalue<Float>(127, 0x3ff);
  m = (m < 0) ? 0 : m;
  m = (m > fvalue<Float>(255, 0x7ff)) ? fvalue<Float>(255, 0x7ff) : m;
  constexpr IntType mant_digits = std::numeric_limits<Float>::digits - 1;
  Float u = bit_cast<Float>(cast<IntType>(cast<IntType>(m) << mant_digits));
  x = x * u * u * u * u;
  e = e + fvalue<Float>(0x7f, 0x3ff);
  u = bit_cast<Float>(cast<IntType>(cast<IntType>(e) << mant_digits));
  const Float y = x * u;
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] e No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::ldexp3(const Float x, const IntT<Float> e) noexcept
{
  using BitType = BitT<Float>;
  using FLimit = std::numeric_limits<Float>;
  const auto bits = bit_cast<BitType>(x) + cast<BitType>(e << (FLimit::digits - 1));
  const Float y = bit_cast<Float>(bits);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr auto Math::Impl::log(Float x) noexcept -> F2<Float>
{
  const bool is_subnormal = x < (std::numeric_limits<Float>::min)();
  if (is_subnormal) {
    constexpr Float k = cast<Float>(1ull << 32ull);
    x *= k * k;
  }
  IntT<Float> e = ilogb2(x * invert(cast<Float>(0.75)));
  const Float m = ldexp3(x, -e);
  if (is_subnormal)
    e = e - 64;

  F2<Float> u = div(add2(cast<Float>(-1), m), add2(cast<Float>(1), m));
  const F2<Float> u2 = squ<F2<Float>>(u);

  Float t = cast<Float>(0.0);
  if constexpr (sizeof(Float) == 4) {
    t = 0.240320354700088500976562f;
    t = mla(t, u2.x_, 0.285112679004669189453125f);
    t = mla(t, u2.x_, 0.400007992982864379882812f);
  }
  else {
    const Float u4 = u2.x_ * u2.x_;
    const Float u8 = u4 * u4;
    const Float u16 = u8 * u8;
    t = poly9<LogDConstants, 0>(u2.x_, u4, u8, u16);
  }

  constexpr F2<Float> c{
    fvalue<Float>(0.66666662693023681640625f, 0.666666666666666629659233),
    fvalue<Float>(3.69183861259614332084311e-09f, 3.80554962542412056336616e-17)};

  F2<Float> s{
      fvalue<Float>(0.69314718246459960938f, 0.693147180559945286226764),
      fvalue<Float>(-1.904654323148236017e-09f, 2.319046813846299558417771e-17)};
  s = mul(s, cast<Float>(e));
  s = add(s, scale(u, cast<Float>(2.0)));
  if constexpr (sizeof(Float) == 4) {
    s = add(s, mul(mul(u2, u), add2(mul(u2, t), c)));
  }
  else {
    u = mul(u2, u);
    s = add(s, mul(u, c));
    u = mul(u2, u);
    s = add(s, mul(u, t));
  }

  return s;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \param [in] z No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::mla(const Float x, const Float y, const Float z) noexcept
{
  const Float result = x * y + z;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::mulsign(const Float x, const Float y) noexcept
{
  using BitType = BitT<Float>;
  const BitType xi = bit_cast<BitType>(x);
  const BitType yi = bit_cast<BitType>(y);
  constexpr BitType n_bits = std::numeric_limits<BitType>::digits;
  constexpr BitType mask = cast<BitType>(1ull << (n_bits - 1));
  const BitType zi = xi ^ (yi & mask);
  const Float z = bit_cast<Float>(zi);
  return z;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::rint(const Float x) noexcept
{
  constexpr Float half = cast<Float>(0.5);
  const int result = isNegative(x) ? cast<int>(x - half) : cast<int>(x + half);
  return cast<Float>(result);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::sign(const Float x) noexcept
{
  const Float y = mulsign(cast<Float>(1.0), x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <FloatingPoint Float> inline
constexpr Float Math::Impl::upper(const Float x) noexcept
{
  using BitType = BitT<Float>;
  using FLimit = std::numeric_limits<Float>;
  using BLimit = std::numeric_limits<BitType>;
  constexpr BitType mask = cast<BitType>((BLimit::max)() << FLimit::digits);
  const BitType u = cast<BitType>(bit_cast<BitType>(x) & mask);
  const Float y = bit_cast<Float>(u);
  return y;
}

///*!
//  \details No detailed description
//
//  \tparam Integer1 No description.
//  \tparam Integer2 No description.
//  \param [in] m No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Integer1, typename Integer2> inline
//constexpr std::common_type_t<Integer1, Integer2> lcm(
//    Integer1 m,
//    Integer2 n) noexcept
//{
//  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
//  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
//  const auto y = Math::lcm(m, n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Integer No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Integer> inline
//constexpr Integer factorial(const Integer n) noexcept
//{
//  static_assert(kIsInteger<Integer>, "Integer isn't integer type.");
//  const auto y = Math::factorial(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Integer1 No description.
//  \tparam Integer2 No description.
//  \param [in] m No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Integer1, typename Integer2> inline
//constexpr std::common_type_t<Integer1, Integer2> sequence(
//    const Integer1 m,
//    const Integer2 n) noexcept
//{
//  static_assert(kIsInteger<Integer1>, "Integer1 isn't integer type.");
//  static_assert(kIsInteger<Integer2>, "Integer2 isn't integer type.");
//  const auto y = Math::sequence(m, n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam kExponent No description.
//  \tparam Arithmetic No description.
//  \param [in] base No description.
//  \return No description
//  */
//template <int kExponent, typename Arithmetic> inline
//constexpr Arithmetic power(Arithmetic base) noexcept
//{
//  static_assert(std::is_arithmetic_v<Arithmetic>, 
//                "Arithmetic isn't arithmetic type");
//  const auto y = Math::power<kExponent>(base);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Arithmetic No description.
//  \tparam SignedInteger No description.
//  \param [in] base No description.
//  \param [in] exponent No description.
//  \return No description
//  */
//template <typename Arithmetic, typename SignedInteger>
//constexpr Arithmetic power(const Arithmetic base,
//                           const SignedInteger exponent) noexcept
//{
//  const auto y = Math::power(base, exponent);
//  return y;
//}

/*!
  \details No detailed description

  \tparam Arith1 No description.
  \tparam Arith2 No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <Arithmetic Arith1, Arithmetic Arith2> inline
constexpr Arith1 pow(const Arith1 base, const Arith2 exponent) noexcept
{
  const Arith1 y = Math::pow(base, exponent);
  return y;
}

///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float sqrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::sqrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float cbrt(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::cbrt(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] n No description.
//  \return No description
//  */
//template <typename Float> inline
//Float exp(const Float n) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::exp(n);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float log(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::log(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float log2(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::log2(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float log10(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::log10(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float sin(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::sin(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float cos(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::cos(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] theta No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float tan(const Float theta) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  const auto y = Math::tan(theta);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float asin(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = Math::asin(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float acos(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(isInClosedBounds(x, -1.0, 1.0), "The x is out of range.");
//  const auto y = Math::acos(x);
//  return y;
//}
//
///*!
//  \details No detailed description
//
//  \tparam Float No description.
//  \param [in] x No description.
//  \return No description
//  */
//template <typename Float> inline 
//Float atan(const Float x) noexcept
//{
//  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
//  ZISC_ASSERT(x != cast<Float>(0.0), "The x is 0.");
//  const auto y = Math::atan(x);
//  return y;
//}

} // namespace zisc

#endif // ZISC_MATH_INL_HPP
