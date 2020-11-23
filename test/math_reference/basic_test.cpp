/*!
  \file basic_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "basic_test.hpp"
// Standard C++ library
#include <array>
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
void testFma(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<std::array<Float, 3>> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(3 * n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeFmaInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal x1{x[0]};
    const mpfr::mpreal x2{x[1]};
    const mpfr::mpreal x3{x[2]};
    const auto result = mpfr::fma(x1, x2, x3);
    const Float y = cast<Float>(result);
    zisc::BSerializer::write(&y, output);
  }
}

} // namespace 

void testFmaF(std::ostream* output) noexcept
{
  ::testFma<float>(output);
}

void testFmaD(std::ostream* output) noexcept
{
  ::testFma<double>(output);
}

namespace {

template <zisc::FloatingPoint Float>
void testFmod(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = 100'000;
  zisc::BSerializer::write(&n, output);

  std::vector<std::array<Float, 2>> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(2 * n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeFmodInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  for (const auto x : x_list) {
    const mpfr::mpreal x1{x[0]};
    const mpfr::mpreal x2{x[1]};
    const auto result = mpfr::fmod(x1, x2);
    const Float y = cast<Float>(result);
    zisc::BSerializer::write(&y, output);
  }

  // Special cases
  auto test_special = [output](const Float x1, const Float x2) noexcept
  {
    zisc::BSerializer::write(&x1, output);
    zisc::BSerializer::write(&x2, output);
    const auto result = mpfr::fmod(mpfr::mpreal{x1}, mpfr::mpreal{x2});
    const Float y = cast<Float>(result);
    zisc::BSerializer::write(&y, output);
  };
  constexpr zisc::int32b n_specials = 3;
  zisc::BSerializer::write(&n_specials, output);
  test_special(cast<Float>(0.0), cast<Float>(2.0));
  test_special(std::numeric_limits<Float>::infinity(), cast<Float>(2.0));
  test_special(-std::numeric_limits<Float>::infinity(), cast<Float>(2.0));
}

} // namespace 

void testFmodF(std::ostream* output) noexcept
{
  ::testFmod<float>(output);
}

void testFmodD(std::ostream* output) noexcept
{
  ::testFmod<double>(output);
}
