/*!
  \file pow_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "exponential_test.hpp"
// Standard C++ library
#include <limits>
#include <numbers>
#include <ostream>
#include <type_traits>
#include <vector>
// MPFR
#include "mpreal.h"
// Zisc
#include "zisc/binary_serializer.hpp"
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
// Test
#include "basic_test.hpp"

namespace {

template <zisc::FloatingPoint Float, int kType>
void testExp(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeXList<Float>(kPowScale<Float>);

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::exp(in) :
                        (kType == 1) ? mpfr::exp2(in) :
                                       mpfr::expm1(in);
    const Float y = cast<Float>(result);
    return y;
  };

  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                  cast<Float>(0),
                  Limits::infinity(),
                  -Limits::infinity(),
                },
                output);
}

template <zisc::FloatingPoint Float, int kType>
void testExpInt(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 16;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float x = cast<Float>(i);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const Float x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::exp(in) :
                        (kType == 1) ? mpfr::exp2(in) :
                                       mpfr::expm1(in);
    const Float y = cast<Float>(result);
    zisc::BSerializer::write(&y, output);
  }
}

} // namespace 

void testExpF(std::ostream* output) noexcept
{
  ::testExp<float, 0>(output);
}

void testExpD(std::ostream* output) noexcept
{
  ::testExp<double, 0>(output);
}

void testExpIntF(std::ostream* output) noexcept
{
  ::testExpInt<float, 0>(output);
}

void testExpIntD(std::ostream* output) noexcept
{
  ::testExpInt<double, 0>(output);
}

void testExp2F(std::ostream* output) noexcept
{
  ::testExp<float, 1>(output);
}

void testExp2D(std::ostream* output) noexcept
{
  ::testExp<double, 1>(output);
}

void testExp2IntF(std::ostream* output) noexcept
{
  ::testExpInt<float, 1>(output);
}

void testExp2IntD(std::ostream* output) noexcept
{
  ::testExpInt<double, 1>(output);
}

void testExpm1F(std::ostream* output) noexcept
{
  ::testExp<float, 2>(output);
}

void testExpm1D(std::ostream* output) noexcept
{
  ::testExp<double, 2>(output);
}

void testExpm1IntF(std::ostream* output) noexcept
{
  ::testExpInt<float, 2>(output);
}

void testExpm1IntD(std::ostream* output) noexcept
{
  ::testExpInt<double, 2>(output);
}

namespace {

template <zisc::FloatingPoint Float, int kType>
void testLog(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::log(in) :
                        (kType == 1) ? mpfr::log10(in) :
                        (kType == 2) ? mpfr::log2(in) :
                                       mpfr::log1p(in);
    const Float y = cast<Float>(result);
    return y;
  };

  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                  cast<Float>(0),
                  cast<Float>(1),
                  Limits::infinity(),
                },
                output);
}

template <zisc::FloatingPoint Float, int kType>
void testLogSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::log(in) :
                        (kType == 1) ? mpfr::log10(in) :
                        (kType == 2) ? mpfr::log2(in) :
                                       mpfr::log1p(in);
    const Float y = cast<Float>(result);
    return y;
  };

  // using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                },
                output);
}

} // namespace

void testLogF(std::ostream* output) noexcept
{
  ::testLog<float, 0>(output);
}

void testLogD(std::ostream* output) noexcept
{
  ::testLog<double, 0>(output);
}

void testLogSubnormalF(std::ostream* output) noexcept
{
  ::testLogSubnormal<float, 0>(output);
}

void testLogSubnormalD(std::ostream* output) noexcept
{
  ::testLogSubnormal<double, 0>(output);
}

void testLog10F(std::ostream* output) noexcept
{
  ::testLog<float, 1>(output);
}

void testLog10D(std::ostream* output) noexcept
{
  ::testLog<double, 1>(output);
}

void testLog10SubnormalF(std::ostream* output) noexcept
{
  ::testLogSubnormal<float, 1>(output);
}

void testLog10SubnormalD(std::ostream* output) noexcept
{
  ::testLogSubnormal<double, 1>(output);
}

void testLog2F(std::ostream* output) noexcept
{
  ::testLog<float, 2>(output);
}

void testLog2D(std::ostream* output) noexcept
{
  ::testLog<double, 2>(output);
}

void testLog2SubnormalF(std::ostream* output) noexcept
{
  ::testLogSubnormal<float, 2>(output);
}

void testLog2SubnormalD(std::ostream* output) noexcept
{
  ::testLogSubnormal<double, 2>(output);
}

void testLog1pF(std::ostream* output) noexcept
{
  ::testLog<float, 3>(output);
}

void testLog1pD(std::ostream* output) noexcept
{
  ::testLog<double, 3>(output);
}

void testLog1pSubnormalF(std::ostream* output) noexcept
{
  ::testLogSubnormal<float, 3>(output);
}

void testLog1pSubnormalD(std::ostream* output) noexcept
{
  ::testLogSubnormal<double, 3>(output);
}
