/*!
  \file nearest_integer_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "nearest_integer_test.hpp"
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
void testNearestInteger(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::rint(in) :
                        (kType == 1) ? mpfr::ceil(in) :
                        (kType == 2) ? mpfr::floor(in) :
                        (kType == 3) ? mpfr::trunc(in) :
                                       mpfr::round(in);
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
void testNearestIntegerSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::rint(in) :
                        (kType == 1) ? mpfr::ceil(in) :
                        (kType == 2) ? mpfr::floor(in) :
                        (kType == 3) ? mpfr::trunc(in) :
                                       mpfr::round(in);
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

void testRintF(std::ostream* output) noexcept
{
  ::testNearestInteger<float, 0>(output);
}

void testRintD(std::ostream* output) noexcept
{
  ::testNearestInteger<double, 0>(output);
}

void testRintSubnormalF(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<float, 0>(output);
}

void testRintSubnormalD(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<double, 0>(output);
}

void testCeilF(std::ostream* output) noexcept
{
  ::testNearestInteger<float, 1>(output);
}

void testCeilD(std::ostream* output) noexcept
{
  ::testNearestInteger<double, 1>(output);
}

void testCeilSubnormalF(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<float, 1>(output);
}

void testCeilSubnormalD(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<double, 1>(output);
}

void testFloorF(std::ostream* output) noexcept
{
  ::testNearestInteger<float, 2>(output);
}

void testFloorD(std::ostream* output) noexcept
{
  ::testNearestInteger<double, 2>(output);
}

void testFloorSubnormalF(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<float, 2>(output);
}

void testFloorSubnormalD(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<double, 2>(output);
}

void testTruncF(std::ostream* output) noexcept
{
  ::testNearestInteger<float, 3>(output);
}

void testTruncD(std::ostream* output) noexcept
{
  ::testNearestInteger<double, 3>(output);
}

void testTruncSubnormalF(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<float, 3>(output);
}

void testTruncSubnormalD(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<double, 3>(output);
}

void testRoundF(std::ostream* output) noexcept
{
  ::testNearestInteger<float, 4>(output);
}

void testRoundD(std::ostream* output) noexcept
{
  ::testNearestInteger<double, 4>(output);
}

void testRoundSubnormalF(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<float, 4>(output);
}

void testRoundSubnormalD(std::ostream* output) noexcept
{
  ::testNearestIntegerSubnormal<double, 4>(output);
}
