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

#include "power_test.hpp"
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

template <zisc::FloatingPoint Float>
void testPow(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);
  constexpr zisc::int32b n_base = 3;
  zisc::BSerializer::write(&n_base, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = makePowInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  // Base 2
  {
    const Float base = cast<Float>(2.0);
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base pi
  {
    const Float base = std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base 1 / pi
  {
    const Float base = cast<Float>(1.0) / std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float base, const Float expt) noexcept
  {
    zisc::BSerializer::write(&base, output);
    zisc::BSerializer::write(&expt, output);
    const auto result = mpfr::pow(mpfr::mpreal{base}, mpfr::mpreal{expt});
    const Float y = cast<Float>(result);
    zisc::BSerializer::write(&y, output);
  };
  constexpr zisc::int32b n_specials = 18;
  zisc::BSerializer::write(&n_specials, output);
  // Base 0
  test_special(cast<Float>(0.0), cast<Float>(0.0));
  test_special(cast<Float>(0.0), cast<Float>(2.0));
  test_special(cast<Float>(0.0), cast<Float>(-2.0));
  test_special(cast<Float>(0.0), -std::numeric_limits<Float>::infinity());
  // Base 1
  test_special(cast<Float>(1.0), cast<Float>(0.0));
  test_special(cast<Float>(1.0), cast<Float>(2.0));
  test_special(cast<Float>(1.0), std::numeric_limits<Float>::infinity());
  test_special(cast<Float>(1.0), cast<Float>(-2.0));
  test_special(cast<Float>(1.0), -std::numeric_limits<Float>::infinity());
  // Base -1
  test_special(cast<Float>(-1.0), std::numeric_limits<Float>::infinity());
  test_special(cast<Float>(-1.0), -std::numeric_limits<Float>::infinity());
  // Base infinity
  test_special(std::numeric_limits<Float>::infinity(), cast<Float>(-2.0));
  test_special(std::numeric_limits<Float>::infinity(), cast<Float>(2.0));
  // Base any
  test_special(cast<Float>(2.0), cast<Float>(0.0));
  test_special(cast<Float>(0.5), std::numeric_limits<Float>::infinity());
  test_special(cast<Float>(2.0), std::numeric_limits<Float>::infinity());
  test_special(cast<Float>(0.5), -std::numeric_limits<Float>::infinity());
  test_special(cast<Float>(2.0), -std::numeric_limits<Float>::infinity());
}

template <zisc::FloatingPoint Float>
void testPowInt(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 16;
  zisc::BSerializer::write(&n, output);
  constexpr zisc::int32b n_base = 5;
  zisc::BSerializer::write(&n_base, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float x = cast<Float>(i);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  // Base 2
  {
    const Float base = cast<Float>(2.0);
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base pi
  {
    const Float base = std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base 1 / pi
  {
    const Float base = cast<Float>(1.0) / std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base -pi
  {
    const Float base = -std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
  // Base -1 / pi
  {
    const Float base = -cast<Float>(1.0) / std::numbers::pi_v<Float>;
    zisc::BSerializer::write(&base, output);
    for (const Float x : x_list) {
      const mpfr::mpreal b{base};
      const mpfr::mpreal e{x};
      const auto result = mpfr::pow(b, e);
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

} // namespace 

void testPowF(std::ostream* output) noexcept
{
  ::testPow<float>(output);
}

void testPowD(std::ostream* output) noexcept
{
  ::testPow<double>(output);
}

void testPowIntF(std::ostream* output) noexcept
{
  ::testPowInt<float>(output);
}

void testPowIntD(std::ostream* output) noexcept
{
  ::testPowInt<double>(output);
}

namespace {

template <zisc::FloatingPoint Float>
void testSqrt(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = mpfr::sqrt(in);
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

template <zisc::FloatingPoint Float>
void testSqrtSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makePositiveSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = mpfr::sqrt(in);
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

void testSqrtF(std::ostream* output) noexcept
{
  ::testSqrt<float>(output);
}

void testSqrtD(std::ostream* output) noexcept
{
  ::testSqrt<double>(output);
}

void testSqrtSubnormalF(std::ostream* output) noexcept
{
  ::testSqrtSubnormal<float>(output);
}

void testSqrtSubnormalD(std::ostream* output) noexcept
{
  ::testSqrtSubnormal<double>(output);
}

namespace {

template <zisc::FloatingPoint Float>
void testCbrt(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = mpfr::cbrt(in);
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

template <zisc::FloatingPoint Float>
void testCbrtSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  auto func = [](const Float x) noexcept
  {
    const mpfr::mpreal in{x};
    const auto result = mpfr::cbrt(in);
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

void testCbrtF(std::ostream* output) noexcept
{
  ::testCbrt<float>(output);
}

void testCbrtD(std::ostream* output) noexcept
{
  ::testCbrt<double>(output);
}

void testCbrtSubnormalF(std::ostream* output) noexcept
{
  ::testCbrtSubnormal<float>(output);
}

void testCbrtSubnormalD(std::ostream* output) noexcept
{
  ::testCbrtSubnormal<double>(output);
}
