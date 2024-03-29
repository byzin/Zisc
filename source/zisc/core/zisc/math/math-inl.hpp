/*!
  \file math-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_INL_HPP
#define ZISC_MATH_INL_HPP

#include "math.hpp"
// Standard C++ library
#include <concepts>
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
template <std::floating_point Float> inline
constexpr auto Math::pi() noexcept -> Float
{
  constexpr int64b n = std::numeric_limits<Float>::digits;
  auto x = cast<Float>(2.0);
  for (int64b k = n; 0 < k; --k) {
    const auto t = cast<Float>(2 * k + 1);
    x = (cast<Float>(k) * x + cast<Float>(2.0) * t) / t;
  }
  return x;
}

/*!
  \details Please read 
  <a href="https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.582.288&rep=rep1&type=pdf">A generalized Kahan-Babuˇska-Summation-Algorithm</a>
  for more details.

  \tparam Float No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \param [in,out] compensation No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::add(const Float lhs,
                          const Float rhs,
                          Float* compensation) noexcept -> Float
{
  const Float t = lhs + rhs;
  if (compensation != nullptr)
    (*compensation) += (abs(rhs) <= abs(lhs)) ? (lhs - t) + rhs : (rhs - t) + lhs;
  return t;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \param [in] z No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::fma(const Float x, const Float y, const Float z) noexcept -> Float
{
  static_cast<void>(y);
  static_cast<void>(z);
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::fmod(const Float x, const Float y) noexcept -> Float
{
  static_cast<void>(y);
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
constexpr auto Math::pow(Arith base, Int exponent) noexcept -> Arith
{
  constexpr auto zero = cast<Arith>(0);
  constexpr auto one = cast<Arith>(1);
  constexpr Arith inf = std::numeric_limits<Arith>::infinity();
  Arith x = (exponent == 0) ? one : signbit(exponent) ? inf : zero;
  if (base != zero) {
    x = one;
    // Inverse pow
    if constexpr (std::floating_point<Arith>) {
      if(signbit(exponent))
        base = invert(base);
    }
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
template <Arithmetic Arith, std::floating_point Float> inline
constexpr auto Math::pow(const Arith base, const Float exponent) noexcept -> Arith
{
  using FLimits = std::numeric_limits<Float>;
  constexpr auto zero = cast<Float>(0.0);
  constexpr auto one = cast<Float>(1.0);

  const bool y_isint = Impl::isInt(exponent);
  const bool y_isodd = y_isint && Impl::isOdd(exponent);

  const auto x = cast<Float>(base);
  const Impl::F2<Float> d = Impl::mul(Impl::log(Impl::abs(x)), exponent);
  Float result = Impl::exp(d);
  if (d.x() > cast<Float>(709.78271114955742909217217426))
    result = FLimits::infinity();

  result = Impl::isNan(result) ? FLimits::infinity() : result;
  result *= signbit(x) ? (y_isodd ? -one : one) : one; //! \todo Nan check

  const Float efx = Impl::mulsign(Impl::abs(x) - one, exponent);
  if (Impl::isInf(exponent))
    result = signbit(efx) ? zero : ((efx == zero) ? one : FLimits::infinity());
  if (Impl::isInf(x) || x == zero)
    result = (y_isodd ? Impl::sign(x) : one) * ((x == zero) ? -exponent : exponent) < zero ? zero : FLimits::infinity();
  //! \todo Nan check
  if (exponent == zero || x == one)
    result = one;

  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::sqrt(Float x) noexcept -> Float
{
  constexpr auto half = cast<Float>(0.5);
  Float q = half;
  if (x < Impl::fvalue<Float>(5.2939559203393770e-23F, 8.636168555094445E-78)) {
    x *= Impl::fvalue<Float>(1.8889465931478580e+22F, 1.157920892373162E77);
    q = half * Impl::fvalue<Float>(7.2759576141834260e-12F, 2.9387358770557188E-39);
  }
  if (x > Impl::fvalue<Float>(1.8446744073709552e+19F, 1.3407807929942597e+154)) {
    x *= Impl::fvalue<Float>(5.4210108624275220e-20F, 7.4583407312002070e-155);
    q = half * Impl::fvalue<Float>(4294967296.0F, 1.1579208923731620e+77);
  }

  // http://en.wikipedia.org/wiki/Fast_inverse_square_root
  auto v = cast<Float>(0);
  {
    using Int = IntT<Float>;
    constexpr Int k = Impl::fvalue<Float>(0x5f375a86, 0x5fe6ec85e7de30daLL);
    const Int u = bit_cast<Int>(x + Impl::fvalue<Float>(1e-45F, 1e-320)) >> 1;
    v = bit_cast<Float>(k - u);
  }

  for (int i = 0; i < 3; ++i) {
    constexpr Float k = cast<Float>(1.5);
    v = v * (k - half * x * v * v);
  }
  v *= x;
  auto x2 = Impl::mul<Impl::F2<Float>>(Impl::add2(x, Impl::mul(v, v)), Impl::rec(v));

  using FLimits = std::numeric_limits<Float>;
  Float result = q * (x2.x() + x2.y());
  result = (x == FLimits::infinity()) ? FLimits::infinity() : result;
  result = (x == cast<Float>(0)) ? x : result;
  return result;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::sqrt(const Int x) noexcept -> Float
{
  const Float y = sqrt(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::cbrt(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::cbrt(const Int x) noexcept -> Float
{
  const Float y = cbrt(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::exp(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::exp(const Int x) noexcept -> Float
{
  const Float y = exp(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::exp2(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::exp2(const Int x) noexcept -> Float
{
  const Float y = exp2(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::log(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::log(const Int x) noexcept -> Float
{
  const Float y = log(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::log2(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::log2(const Int x) noexcept -> Float
{
  const Float y = log2(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::log10(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto Math::log10(const Int x) noexcept -> Float
{
  const Float y = log10(cast<Float>(x));
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::sin(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::cos(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::tan(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::asin(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::acos(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::atan(const Float x) noexcept -> Float
{
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] y No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::atan2(const Float y, const Float x) noexcept -> Float
{
  static_cast<void>(y);
  return x;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add(const Float x, const Float y) noexcept
    -> F2<Float>
{
  F2<Float> result{x + y};
  result.y() = x - result.x() + y;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add(const Float x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x + y.x()};
  result.y() = x - result.x() + y.x() + y.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add(const F2<Float> x, const Float y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x() + y};
  result.y() = x.x() - result.x() + y + x.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x() + y.x()};
  result.y() = x.x() - result.x() + y.x() + x.y() + y.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add2(const Float x, const Float y) noexcept -> F2<Float>
{
  F2<Float> result{x + y};
  const Float v = result.x() - x;
  result.y() = (x - (result.x() - v)) + (y - v);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add2(const Float x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x + y.x()};
  const Float v = result.x() - x;
  result.y() = (x - (result.x() - v)) + (y.x() - v) + y.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add2(const F2<Float> x, const Float y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x() + y};
  const Float v = result.x() - x.x();
  result.y() = (x.x() - (result.x() - v)) + (y - v) + x.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::add2(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  F2<Float> result{x.x() + y.x()};
  const Float v = result.x() - x.x();
  result.y() = (x.x() - (result.x() - v)) + (y.x() - v) + (x.y() + y.y());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::div(const F2<Float> x, const F2<Float> y) noexcept
    -> F2<Float>
{
  const Float t = invert(y.x());
  const Float yh = upper(y.x());
  const Float yl = y.x() - yh;
  const Float th = upper(t);
  const Float tl = t - th;
  const Float xhh = upper(x.x());
  const Float xhl = x.x() - xhh;

  F2<Float> q{t * x.x()};
  const Float u = -q.x() + xhh * th + xhh * tl + xhl * th + xhl * tl +
                   q.x() * (cast<Float>(1.0) - yh * th - yh * tl - yl * th - yl * tl);
  q.y() = t * (x.y() - q.x() * y.y()) + u;
  return q;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::mul(const Float x, const Float y) noexcept -> F2<Float>
{
  const Float xh = upper(x);
  const Float xl = x - xh;
  const Float yh = upper(y);
  const Float yl = y - yh;

  F2<Float> result{x * y};
  result.y() = xh * yh - result.x() + xl * yh + xh * yl + xl * yl;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::mul(const F2<Float> x, const Float y) noexcept
    -> F2<Float>
{
  const Float xh = upper(x.x());
  const Float xl = x.x() - xh;
  const Float yh = upper(y);
  const Float yl = y - yh;

  F2<Float> result{x.x() * y};
  result.y() = xh * yh - result.x() + xl * yh + xh * yl + xl * yl + x.y() * y;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <typename ReturnT, std::floating_point Float> inline
constexpr auto Math::Impl::mul(const F2<Float> x, const F2<Float> y) noexcept -> ReturnT
{
  const Float xh = upper(x.x());
  const Float xl = x.x() - xh;
  const Float yh = upper(y.x());
  const Float yl = y.x() - yh;

  if constexpr (std::same_as<Float, ReturnT>) {
    const Float result = x.y() * yh + xh * y.y() + xl * yl + xh * yl + xl * yh + xh * yh;
    return result;
  }
  else if constexpr (std::same_as<F2<Float>, ReturnT>) {
    F2<Float> result{x.x() * y.x()};
    result.y() = xh * yh - result.x() + xl * yh + xh * yl + xl * yl + x.x() * y.y() + x.y() * y.x();
    return result;
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::normalize(const F2<Float> x) noexcept -> F2<Float>
{
  F2<Float> result{x.x() + x.y()};
  result.y() = x.x() - result.x() + x.y();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::rec(const Float x) noexcept -> F2<Float>
{
  const Float xh = upper(x);
  const Float xl = x - xh;
  const Float t = invert(x);
  const Float th = upper(t);
  const Float tl = t - th;

  constexpr Float one = cast<Float>(1);
  F2<Float> result{t, t * (one - xh * th - xh * tl - xl * th - xl * tl)};
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::rec(const F2<Float> x) noexcept -> F2<Float>
{
  const Float xh = upper(x.x());
  const Float xl = x.x() - xh;
  const Float t = invert(x.x());
  const Float th = upper(t);
  const Float tl = t - th;

  constexpr Float one = cast<Float>(1);
  F2<Float> result{t, t * (one - xh * th - xh * tl - xl * th - xl * tl - x.y() * t)};
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] s No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::scale(const F2<Float> x, const Float s) noexcept
    -> F2<Float>
{
  const F2<Float> result{s * x.x(), s * x.y()};
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <typename ReturnT, std::floating_point Float> inline
constexpr auto Math::Impl::squ(const F2<Float> x) noexcept -> ReturnT
{
  const Float xh = upper(x.x());
  const Float xl = x.x() - xh;
  ReturnT y;
  if constexpr (std::is_scalar_v<ReturnT>) {
    y = xh * x.y() + xh * x.y() + xl * xl + (xh * xl + xh * xl) + xh * xh;
  }
  else {
    y.x() = x.x() * x.x();
    y.y() = xh * xh - y.x() + (xh + xh) * xl + xl * xl + x.x() * (x.y() + x.y());
  }
  return y;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
constexpr auto Math::Impl::ExpDConstants::get(const std::size_t index) noexcept -> double
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
constexpr auto Math::Impl::LogDConstants::get(const std::size_t index) noexcept -> double
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::ln2() noexcept -> Float
{
  const Float k = fvalue<Float>(
      1.442695040888963407359924681001892137426645954152985934135449406931F,
      1.442695040888963407359924681001892137426645954152985934135449406931);
  return k;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::l2l() noexcept -> Float
{
  const Float k = fvalue<Float>(
      1.428606765330187045e-06F,
      .28235290563031577122588448175013436025525412068e-12);
  return k;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::l2u() noexcept -> Float
{
  const Float k = fvalue<Float>(
      0.693145751953125F,
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly2(const Float x) noexcept -> Float
{
  constexpr Float y = Constants::get(coffset + 1);
  constexpr Float z = Constants::get(coffset + 0);
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly3(const Float x, const Float x2) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly4(const Float x, const Float x2) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly5(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly6(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly7(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly8(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly9(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly10(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly11(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly12(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly13(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly14(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly15(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly16(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly17(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly18(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept -> Float
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
template <typename Constants, std::size_t coffset, std::floating_point Float> inline
constexpr auto Math::Impl::poly19(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept -> Float
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::abs(const Float x) noexcept -> Float
{
  const Float y = signbit(x) ? -x : x;
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::copysign(const Float x, const Float y) noexcept -> Float
{
  using IntType = IntT<Float>;
  const auto xi = bit_cast<IntType>(x);
  const auto yi = bit_cast<IntType>(y);
  constexpr IntType mask = (std::numeric_limits<IntType>::min)();
  const IntType zi = (xi & ~mask) ^ (yi & mask);
  const auto z = bit_cast<Float>(zi);
  return z;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::exp(const F2<Float> x) noexcept -> Float
{
  const auto q = cast<int>(rint((x.x() + x.y()) * ln2<Float>()));
  F2<Float> s = add2(x, cast<Float>(q) * -l2u<Float>());
  s = add2(s, cast<Float>(q) * -l2l<Float>());
  s = normalize(s);

  Float u = cast<Float>(0.0);
  F2<Float> t;
  if constexpr (sizeof(Float) == 4) {
    u = 0.00136324646882712841033936F;
    u = mla(u, s.x(), 0.00836596917361021041870117F);
    u = mla(u, s.x(), 0.0416710823774337768554688F);
    u = mla(u, s.x(), 0.166665524244308471679688F);
    u = mla(u, s.x(), 0.499999850988388061523438F);
    t = add(s, mul(squ<F2<Float>>(s), u));
    t = add(1.0F, t);
  }
  else {
    const Float s2 = s.x() * s.x();
    const Float s4 = s2 * s2;
    const Float s8 = s4 * s4;
    u = poly10<ExpDConstants, 0>(s.x(), s2, s4, s8);
    t = add(1.0, s);
    t = add(t, mul(squ<F2<Float>>(s), u));
  }
  u = ldexp(t.x() + t.y(), q);
  if (x.x() < fvalue<Float>(-104.0F, -1000.0))
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
template <std::floating_point Float, Arithmetic Arith1, Arithmetic Arith2> inline
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::ilogb2(const Float x) noexcept -> IntT<Float>
{
  using IntType = IntT<Float>;
  using FLimits = std::numeric_limits<Float>;
  constexpr IntType mask = fvalue<Float>(0xfF, 0x7ff);
  constexpr IntType k = fvalue<Float>(0x7F, 0x3ff);
  const IntType expt = bit_cast<IntType>(x) >> (FLimits::digits - 1);
  const IntType y = (expt & mask) - k;
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::isInf(const Float x) noexcept -> bool
{
  using FLimits = std::numeric_limits<Float>;
  const bool result = (x == FLimits::infinity()) || (x == -FLimits::infinity());
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::isInt(const Float x) noexcept -> bool
{
  Float d = x;
  if constexpr (sizeof(Float) == 8)
  {
    constexpr auto t = cast<Float>(1ULL << 31ULL);
    d = x - t * cast<Float>(cast<int>(x * invert(t)));
  }
  constexpr auto k = cast<Float>(1ULL << std::numeric_limits<Float>::digits);
  const bool result = (d == cast<Float>(cast<int>(d))) || (k <= abs(x));
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::isNan(const Float x) noexcept -> bool
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::isOdd(const Float x) noexcept -> bool
{
  Float d = x;
  if constexpr (sizeof(Float) == 8) {
    constexpr auto t = cast<Float>(1ULL << 31ULL);
    d = x - t * cast<Float>(cast<int>(x * invert(t)));
  }
  constexpr auto k = cast<Float>(1ULL << std::numeric_limits<Float>::digits);
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::ldexp(Float x, int e) noexcept -> Float
{
  using IntType = IntT<Float>;
  int m = e >> 31;
  m = (((m + e) >> fvalue<Float>(6, 9)) - m) << fvalue<Float>(4, 7);
  e = e - (m << 2);
  m += fvalue<Float>(127, 0x3ff);
  m = (m < 0) ? 0 : m;
  m = (m > fvalue<Float>(255, 0x7ff)) ? fvalue<Float>(255, 0x7ff) : m;
  constexpr IntType mant_digits = std::numeric_limits<Float>::digits - 1;
  auto u = bit_cast<Float>(cast<IntType>(cast<IntType>(m) << mant_digits));
  x = x * u * u * u * u;
  e = e + fvalue<Float>(0x7F, 0x3ff);
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::ldexp3(const Float x, const IntT<Float> e) noexcept -> Float
{
  using IntType = IntT<Float>;
  using FLimits = std::numeric_limits<Float>;
  const auto bits = bit_cast<IntType>(x) + cast<IntType>(e << (FLimits::digits - 1));
  const auto y = bit_cast<Float>(bits);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::log(Float x) noexcept -> F2<Float>
{
  const bool is_subnormal = x < (std::numeric_limits<Float>::min)();
  if (is_subnormal) {
    constexpr auto k = cast<Float>(1ULL << 32ULL);
    x *= k * k;
  }
  IntT<Float> e = ilogb2(x * invert(cast<Float>(0.75)));
  const Float m = ldexp3(x, -e);
  if (is_subnormal)
    e = e - 64;

  F2<Float> u = div(add2(cast<Float>(-1), m), add2(cast<Float>(1), m));
  const F2<Float> u2 = squ<F2<Float>>(u);

  auto t = cast<Float>(0.0);
  if constexpr (sizeof(Float) == 4) {
    t = 0.240320354700088500976562F;
    t = mla(t, u2.x(), 0.285112679004669189453125F);
    t = mla(t, u2.x(), 0.400007992982864379882812F);
  }
  else {
    const Float u4 = u2.x() * u2.x();
    const Float u8 = u4 * u4;
    const Float u16 = u8 * u8;
    t = poly9<LogDConstants, 0>(u2.x(), u4, u8, u16);
  }

  constexpr F2<Float> c{
    fvalue<Float>(0.66666662693023681640625F, 0.666666666666666629659233),
    fvalue<Float>(3.69183861259614332084311e-09F, 3.80554962542412056336616e-17)};

  F2<Float> s{
      fvalue<Float>(0.69314718246459960938F, 0.693147180559945286226764),
      fvalue<Float>(-1.904654323148236017e-09F, 2.319046813846299558417771e-17)};
  s = mul(s, cast<Float>(e));
  s = add(s, scale(u, cast<Float>(2.0)));
  if constexpr (sizeof(Float) == 4) {
    s = add(s, mul<F2<Float>>(mul<F2<Float>>(u2, u), add2(mul(u2, t), c)));
  }
  else {
    u = mul<F2<Float>>(u2, u);
    s = add(s, mul<F2<Float>>(u, c));
    u = mul<F2<Float>>(u2, u);
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::mla(const Float x, const Float y, const Float z) noexcept -> Float
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::mulsign(const Float x, const Float y) noexcept -> Float
{
  using IntType = IntT<Float>;
  const IntType xi = bit_cast<IntType>(x);
  const IntType yi = bit_cast<IntType>(y);
  constexpr IntType mask = (std::numeric_limits<IntType>::min)();
  const IntType zi = xi ^ (yi & mask);
  const Float z = bit_cast<Float>(zi);
  return z;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::rint(const Float x) noexcept -> Float
{
  constexpr auto half = cast<Float>(0.5);
  const int result = signbit(x) ? cast<int>(x - half) : cast<int>(x + half);
  return cast<Float>(result);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto Math::Impl::sign(const Float x) noexcept -> Float
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
template <std::floating_point Float> inline
constexpr auto Math::Impl::upper(const Float x) noexcept -> Float
{
  using IntType = IntT<Float>;
  constexpr IntType mask = fvalue<Float>(0xfffff000, 0xfffffffff8000000LL);
  const IntType u = cast<IntType>(bit_cast<IntType>(x) & mask);
  const Float y = bit_cast<Float>(u);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \param [in] z No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto fma(const Float x, const Float y, const Float z) noexcept -> Float
{
  const Float result = Math::fma(x, y, z);
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \param [in] y No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto fmod(const Float x, const Float y) noexcept -> Float
{
  const Float z = Math::fmod(x, y);
  return z;
}

/*!
  \details No detailed description

  \tparam Arith1 No description.
  \tparam Arith2 No description.
  \param [in] base No description.
  \param [in] exponent No description.
  \return No description
  */
template <Arithmetic Arith1, Arithmetic Arith2> inline
constexpr auto pow(const Arith1 base, const Arith2 exponent) noexcept -> Arith1
{
  const Arith1 y = Math::pow(base, exponent);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto sqrt(const Float x) noexcept -> Float
{
  const Float y = Math::sqrt(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto sqrt(const Int x) noexcept -> Float
{
  const Float y = Math::sqrt<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto cbrt(const Float x) noexcept -> Float
{
  const Float y = Math::cbrt(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto cbrt(const Int x) noexcept -> Float
{
  const Float y = Math::cbrt<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto exp(const Float x) noexcept -> Float
{
  const Float y = Math::exp(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto exp(const Int x) noexcept -> Float
{
  const Float y = Math::exp<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto exp2(const Float x) noexcept -> Float
{
  const Float y = Math::exp2(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto exp2(const Int x) noexcept -> Float
{
  const Float y = Math::exp2<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto log(const Float x) noexcept -> Float
{
  const Float y = Math::log(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto log(const Int x) noexcept -> Float
{
  const Float y = Math::log<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto log2(const Float x) noexcept -> Float
{
  const Float y = Math::log2(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto log2(const Int x) noexcept -> Float
{
  const Float y = Math::log2<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto log10(const Float x) noexcept -> Float
{
  const Float y = Math::log10(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Int No description.
  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float, Integer Int> inline
constexpr auto log10(const Int x) noexcept -> Float
{
  const Float y = Math::log10<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto sin(const Float x) noexcept -> Float
{
  const Float y = Math::sin(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto cos(const Float x) noexcept -> Float
{
  const Float y = Math::cos(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto tan(const Float x) noexcept -> Float
{
  const Float y = Math::tan(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto asin(const Float x) noexcept -> Float
{
  const Float y = Math::asin(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto acos(const Float x) noexcept -> Float
{
  const Float y = Math::acos(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto atan(const Float x) noexcept -> Float
{
  const Float y = Math::atan(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] y No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr auto atan2(const Float y, const Float x) noexcept -> Float
{
  const Float z = Math::atan2(y, x);
  return z;
}

} // namespace zisc

#endif // ZISC_MATH_INL_HPP
