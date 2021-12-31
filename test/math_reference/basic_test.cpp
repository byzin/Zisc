/*!
  \file basic_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "basic_test.hpp"
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

template <zisc::FloatingPoint Float, zisc::int32b kN = 100'000>
void testFma(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = kN;
  zisc::BSerializer::write(&n, output);

  std::vector<F3<Float>> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeFmaInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  auto func = [](const F3<Float> x) noexcept
  {
    const mpfr::mpreal x1{x.x_};
    const mpfr::mpreal x2{x.y_};
    const mpfr::mpreal x3{x.z_};
    const auto result = mpfr::fma(x1, x2, x3);
    const Float y = cast<Float>(result);
    return y;
  };
  testF3<Float>(func,
                x_list,
                {},
                output);
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

template <zisc::FloatingPoint Float, zisc::int32b kN = 100'000>
void testFmod(std::ostream* output) noexcept
{
  using zisc::cast;

  constexpr zisc::int32b n = kN;
  zisc::BSerializer::write(&n, output);

  std::vector<F2<Float>> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const auto x = makeFmodInput(u);
    zisc::BSerializer::write(&x, output);
    x_list.push_back(x);
  }

  auto func = [](const F2<Float> x) noexcept
  {
    const mpfr::mpreal x1{x.x_};
    const mpfr::mpreal x2{x.y_};
    const auto result = mpfr::fmod(x1, x2);
    const Float y = cast<Float>(result);
    return y;
  };
  using Limits = std::numeric_limits<Float>;
  testF2<Float>(func,
                x_list,
                {
                  {cast<Float>(0), cast<Float>(2)},
                  {Limits::infinity(), cast<Float>(2)},
                  {-Limits::infinity(), cast<Float>(2)}
                },
                output);
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
