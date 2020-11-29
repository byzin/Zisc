/*!
  \file error_function_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "error_function_test.hpp"
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
void testErf(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeXList<Float>(kPowScale<Float>);

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::erf(in) :
                                       mpfr::erfc(in);
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
void testErfSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::erf(in) :
                                       mpfr::erfc(in);
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

void testErfF(std::ostream* output) noexcept
{
  ::testErf<float, 0>(output);
}

void testErfD(std::ostream* output) noexcept
{
  ::testErf<double, 0>(output);
}

void testErfSubnormalF(std::ostream* output) noexcept
{
  ::testErfSubnormal<float, 0>(output);
}

void testErfSubnormalD(std::ostream* output) noexcept
{
  ::testErfSubnormal<double, 0>(output);
}

void testErfcF(std::ostream* output) noexcept
{
  ::testErf<float, 1>(output);
}

void testErfcD(std::ostream* output) noexcept
{
  ::testErf<double, 1>(output);
}

void testErfcSubnormalF(std::ostream* output) noexcept
{
  ::testErfSubnormal<float, 1>(output);
}

void testErfcSubnormalD(std::ostream* output) noexcept
{
  ::testErfSubnormal<double, 1>(output);
}

namespace {

template <zisc::FloatingPoint Float, int kType>
void testTgamma(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::tgamma(in) :
                                       mpfr::lgamma(in);
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
void testTgammaSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::tgamma(in) :
                                       mpfr::lgamma(in);
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

void testTgammaF(std::ostream* output) noexcept
{
  ::testTgamma<float, 0>(output);
}

void testTgammaD(std::ostream* output) noexcept
{
  ::testTgamma<double, 0>(output);
}

void testTgammaSubnormalF(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<float, 0>(output);
}

void testTgammaSubnormalD(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<double, 0>(output);
}

void testLgammaF(std::ostream* output) noexcept
{
  ::testTgamma<float, 1>(output);
}

void testLgammaD(std::ostream* output) noexcept
{
  ::testTgamma<double, 1>(output);
}

void testLgammaSubnormalF(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<float, 1>(output);
}

void testLgammaSubnormalD(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<double, 1>(output);
}
