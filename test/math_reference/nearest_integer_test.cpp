/*!
  \file nearest_integer_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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

namespace {

template <zisc::FloatingPoint Float, int kType>
void testNearestInteger(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeRintInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::rint(in) :
                        (kType == 1) ? mpfr::ceil(in) :
                        (kType == 2) ? mpfr::floor(in) :
                        (kType == 3) ? mpfr::trunc(in) :
                                       mpfr::round(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::rint(in) :
                        (kType == 1) ? mpfr::ceil(in) :
                        (kType == 2) ? mpfr::floor(in) :
                        (kType == 3) ? mpfr::trunc(in) :
                                       mpfr::round(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  };
  constexpr zisc::int32b n_specials = 3;
  zisc::BSerializer::write(&n_specials, output);
  test_special(cast<Float>(0.0));
  test_special(std::numeric_limits<Float>::infinity());
  test_special(-std::numeric_limits<Float>::infinity());
}

template <zisc::FloatingPoint Float, int kType>
void testNearestIntegerSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 200;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeRintSubnormalInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = (kType == 0) ? mpfr::rint(in) :
                        (kType == 1) ? mpfr::ceil(in) :
                        (kType == 2) ? mpfr::floor(in) :
                        (kType == 3) ? mpfr::trunc(in) :
                                       mpfr::round(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
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
