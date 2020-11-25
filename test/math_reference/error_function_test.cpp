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

namespace {

template <zisc::FloatingPoint Float>
void testErf(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeErfInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::erf(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const auto result = mpfr::erf(mpfr::mpreal{x});
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

template <zisc::FloatingPoint Float>
void testErfSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 200;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeErfSubnormalInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::erf(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

template <zisc::FloatingPoint Float>
void testErfc(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeErfInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::erfc(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const auto result = mpfr::erfc(mpfr::mpreal{x});
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

template <zisc::FloatingPoint Float>
void testErfcSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 200;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeErfSubnormalInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::erfc(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

} // namespace 

void testErfF(std::ostream* output) noexcept
{
  ::testErf<float>(output);
}

void testErfD(std::ostream* output) noexcept
{
  ::testErf<double>(output);
}

void testErfSubnormalF(std::ostream* output) noexcept
{
  ::testErfSubnormal<float>(output);
}

void testErfSubnormalD(std::ostream* output) noexcept
{
  ::testErfSubnormal<double>(output);
}

void testErfcF(std::ostream* output) noexcept
{
  ::testErfc<float>(output);
}

void testErfcD(std::ostream* output) noexcept
{
  ::testErfc<double>(output);
}

void testErfcSubnormalF(std::ostream* output) noexcept
{
  ::testErfcSubnormal<float>(output);
}

void testErfcSubnormalD(std::ostream* output) noexcept
{
  ::testErfcSubnormal<double>(output);
}

namespace {

template <zisc::FloatingPoint Float>
void testTgamma(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeTgammaInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::tgamma(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const auto result = mpfr::tgamma(mpfr::mpreal{x});
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

template <zisc::FloatingPoint Float>
void testTgammaSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 200;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeTgammaSubnormalInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::tgamma(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

template <zisc::FloatingPoint Float>
void testLgamma(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeTgammaInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::lgamma(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const auto result = mpfr::lgamma(mpfr::mpreal{x});
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

template <zisc::FloatingPoint Float>
void testLgammaSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 200;
  zisc::BSerializer::write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeTgammaSubnormalInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::lgamma(in);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

} // namespace 

void testTgammaF(std::ostream* output) noexcept
{
  ::testTgamma<float>(output);
}

void testTgammaD(std::ostream* output) noexcept
{
  ::testTgamma<double>(output);
}

void testTgammaSubnormalF(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<float>(output);
}

void testTgammaSubnormalD(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<double>(output);
}

void testLgammaF(std::ostream* output) noexcept
{
  ::testLgamma<float>(output);
}

void testLgammaD(std::ostream* output) noexcept
{
  ::testLgamma<double>(output);
}

void testLgammaSubnormalF(std::ostream* output) noexcept
{
  ::testLgammaSubnormal<float>(output);
}

void testLgammaSubnormalD(std::ostream* output) noexcept
{
  ::testTgammaSubnormal<double>(output);
}
