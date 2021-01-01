/*!
  \file trigonometric_test_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "trigonometric_test.hpp"
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
void testTrig(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeXList<Float>(kPowScale<Float>);

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::sin(in) :
                        (kType == 1) ? mpfr::cos(in) :
                                       mpfr::tan(in);
    const Float y = cast<Float>(result);
    return y;
  };

  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                  cast<Float>(0),
                  Limits::infinity(),
                  -Limits::infinity()
                },
                output);
}

template <zisc::FloatingPoint Float, int kType>
void testTrigSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::sin(in) :
                        (kType == 1) ? mpfr::cos(in) :
                                       mpfr::tan(in);
    const Float y = cast<Float>(result);
    return y;
  };

//  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                },
                output);
}

} // namespace

void testSinF(std::ostream* output) noexcept
{
  ::testTrig<float, 0>(output);
}

void testSinD(std::ostream* output) noexcept
{
  ::testTrig<double, 0>(output);
}

void testSinSubnormalF(std::ostream* output) noexcept
{
  ::testTrigSubnormal<float, 0>(output);
}

void testSinSubnormalD(std::ostream* output) noexcept
{
  ::testTrigSubnormal<double, 0>(output);
}

void testCosF(std::ostream* output) noexcept
{
  ::testTrig<float, 1>(output);
}

void testCosD(std::ostream* output) noexcept
{
  ::testTrig<double, 1>(output);
}

void testCosSubnormalF(std::ostream* output) noexcept
{
  ::testTrigSubnormal<float, 1>(output);
}

void testCosSubnormalD(std::ostream* output) noexcept
{
  ::testTrigSubnormal<double, 1>(output);
}

void testTanF(std::ostream* output) noexcept
{
  ::testTrig<float, 2>(output);
}

void testTanD(std::ostream* output) noexcept
{
  ::testTrig<double, 2>(output);
}

void testTanSubnormalF(std::ostream* output) noexcept
{
  ::testTrigSubnormal<float, 2>(output);
}

void testTanSubnormalD(std::ostream* output) noexcept
{
  ::testTrigSubnormal<double, 2>(output);
}

namespace {

template <zisc::FloatingPoint Float, int kType>
void testArcTrig(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeXList<Float>(cast<Float>(1));

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::asin(in) :
                        (kType == 1) ? mpfr::acos(in) :
                                       mpfr::atan(in);
    const Float y = cast<Float>(result);
    return y;
  };

  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                  cast<Float>(0),
                  Limits::infinity(),
                  -Limits::infinity()
                },
                output);
}

template <zisc::FloatingPoint Float, int kType>
void testArcTrigSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::asin(in) :
                        (kType == 1) ? mpfr::acos(in) :
                                       mpfr::atan(in);
    const Float y = cast<Float>(result);
    return y;
  };

//  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                },
                output);
}

} // namespace

void testAsinF(std::ostream* output) noexcept
{
  ::testArcTrig<float, 0>(output);
}

void testAsinD(std::ostream* output) noexcept
{
  ::testArcTrig<double, 0>(output);
}

void testAsinSubnormalF(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<float, 0>(output);
}

void testAsinSubnormalD(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<double, 0>(output);
}

void testAcosF(std::ostream* output) noexcept
{
  ::testArcTrig<float, 1>(output);
}

void testAcosD(std::ostream* output) noexcept
{
  ::testArcTrig<double, 1>(output);
}

void testAcosSubnormalF(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<float, 1>(output);
}

void testAcosSubnormalD(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<double, 1>(output);
}

void testAtanF(std::ostream* output) noexcept
{
  ::testArcTrig<float, 2>(output);
}

void testAtanD(std::ostream* output) noexcept
{
  ::testArcTrig<double, 2>(output);
}

void testAtanSubnormalF(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<float, 2>(output);
}

void testAtanSubnormalD(std::ostream* output) noexcept
{
  ::testArcTrigSubnormal<double, 2>(output);
}

namespace {

template <zisc::FloatingPoint Float, zisc::int32b kN = 100'000>
void testAtan2(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = kN;
  zisc::BSerializer::write(&n, output);

  std::vector<F2<Float>> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeAtan2Input(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  auto func = [](const F2<Float> x) noexcept
  {
    const mpfr::mpreal x1{x.x_};
    const mpfr::mpreal x2{x.y_};
    const auto result = mpfr::atan2(x1, x2);
    const Float y = cast<Float>(result);
    return y;
  };
  using Limits = std::numeric_limits<Float>;
  testF2<Float>(func,
                x_list,
                {
                  {cast<Float>(0), cast<Float>(-1)},
                  {cast<Float>(0), cast<Float>(+1)},
                  {Limits::infinity(), cast<Float>(1)},
                  {Limits::infinity(), cast<Float>(-1)},
                  {Limits::infinity(), -Limits::infinity()},
                  {-Limits::infinity(), Limits::infinity()},
                  {cast<Float>(1), cast<Float>(0)},
                  {cast<Float>(-1), cast<Float>(0)},
                  {cast<Float>(1), Limits::infinity()},
                  {cast<Float>(-1), Limits::infinity()},
                  {cast<Float>(1), -Limits::infinity()},
                  {cast<Float>(-1), -Limits::infinity()}
                },
                output);
}

} // namespace 

void testAtan2F(std::ostream* output) noexcept
{
  ::testAtan2<float>(output);
}

void testAtan2D(std::ostream* output) noexcept
{
  ::testAtan2<double>(output);
}

namespace {

template <zisc::FloatingPoint Float, int kType>
void testHyperbolic(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::sinh(in) :
                        (kType == 1) ? mpfr::cosh(in) :
                        (kType == 2) ? mpfr::tanh(in) :
                        (kType == 3) ? mpfr::asinh(in) :
                        (kType == 4) ? mpfr::acosh(in) :
                                       mpfr::atanh(in);
    const Float y = cast<Float>(result);
    return y;
  };

  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                  cast<Float>(0),
                  Limits::infinity(),
                  -Limits::infinity()
                },
                output);
}

template <zisc::FloatingPoint Float, int kType>
void testHyperbolicSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::sinh(in) :
                        (kType == 1) ? mpfr::cosh(in) :
                        (kType == 2) ? mpfr::tanh(in) :
                        (kType == 3) ? mpfr::asinh(in) :
                        (kType == 4) ? mpfr::acosh(in) :
                                       mpfr::atanh(in);
    const Float y = cast<Float>(result);
    return y;
  };

//  using Limits = std::numeric_limits<Float>;
  testF1<Float>(func,
                x_list,
                {
                },
                output);
}

} // namespace

void testSinhF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 0>(output);
}

void testSinhD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 0>(output);
}

void testSinhSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 0>(output);
}

void testSinhSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 0>(output);
}

void testCoshF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 1>(output);
}

void testCoshD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 1>(output);
}

void testCoshSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 1>(output);
}

void testCoshSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 1>(output);
}

void testTanhF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 2>(output);
}

void testTanhD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 2>(output);
}

void testTanhSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 2>(output);
}

void testTanhSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 2>(output);
}

void testAsinhF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 3>(output);
}

void testAsinhD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 3>(output);
}

void testAsinhSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 3>(output);
}

void testAsinhSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 3>(output);
}

void testAcoshF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 4>(output);
}

void testAcoshD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 4>(output);
}

void testAcoshSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 4>(output);
}

void testAcoshSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 4>(output);
}

void testAtanhF(std::ostream* output) noexcept
{
  ::testHyperbolic<float, 5>(output);
}

void testAtanhD(std::ostream* output) noexcept
{
  ::testHyperbolic<double, 5>(output);
}

void testAtanhSubnormalF(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<float, 5>(output);
}

void testAtanhSubnormalD(std::ostream* output) noexcept
{
  ::testHyperbolicSubnormal<double, 5>(output);
}
