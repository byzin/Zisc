/*!
  \file float_manipulation_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "float_manipulation_test.hpp"
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

template <zisc::FloatingPoint Float>
void testFrLdexp(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllXList<Float>();

  for (const auto x : x_list) {
    mp_exp_t expt{0};
    const mpfr::mpreal in{x};
    const auto mant = mpfr::frexp(in, &expt);
    {
      const Float y = cast<Float>(mant);
      zisc::BSerializer::write(&y, output);
      const zisc::int32b e = cast<zisc::int32b>(expt);
      zisc::BSerializer::write(&e, output);
    }
    const auto result = mpfr::ldexp(mant, expt);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    mp_exp_t expt{0};
    const auto mant = mpfr::frexp(mpfr::mpreal{x}, &expt);
    {
      const Float y = cast<Float>(mant);
      zisc::BSerializer::write(&y, output);
      const zisc::int32b e = cast<zisc::int32b>(expt);
      zisc::BSerializer::write(&e, output);
    }
    const auto result = mpfr::ldexp(mant, expt);
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
void testFrLdexpSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  for (const auto x : x_list) {
    mp_exp_t expt{0};
    const mpfr::mpreal in{x};
    const auto mant = mpfr::frexp(in, &expt);
    {
      const Float y = cast<Float>(mant);
      zisc::BSerializer::write(&y, output);
      const zisc::int32b e = cast<zisc::int32b>(expt);
      zisc::BSerializer::write(&e, output);
    }
    const auto result = mpfr::ldexp(mant, expt);
    {
      const Float y = cast<Float>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

} // namespace 

void testFrLdexpF(std::ostream* output) noexcept
{
  ::testFrLdexp<float>(output);
}

void testFrLdexpD(std::ostream* output) noexcept
{
  ::testFrLdexp<double>(output);
}

void testFrLdexpSubnormalF(std::ostream* output) noexcept
{
  ::testFrLdexpSubnormal<float>(output);
}

void testFrLdexpSubnormalD(std::ostream* output) noexcept
{
  ::testFrLdexpSubnormal<double>(output);
}

namespace {

template <zisc::FloatingPoint Float>
void testIlogb(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllXList<Float>();

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::ilogb(in);
    {
      const zisc::int32b y = cast<zisc::int32b>(result);
      zisc::BSerializer::write(&y, output);
    }
  }

  // Special cases
  auto test_special = [output](const Float x) noexcept
  {
    zisc::BSerializer::write(&x, output);
    const auto result = mpfr::ilogb(mpfr::mpreal{x});
    {
      const zisc::int32b y = cast<zisc::int32b>(result);
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
void testIlogbSubnormal(std::ostream* output) noexcept
{
  using zisc::cast;

  auto x_list = makeAllSubnormalXList<Float>();

  for (const auto x : x_list) {
    const mpfr::mpreal in{x};
    const auto result = mpfr::ilogb(in);
    {
      const zisc::int32b y = cast<zisc::int32b>(result);
      zisc::BSerializer::write(&y, output);
    }
  }
}

} // namespace 

void testIlogbF(std::ostream* output) noexcept
{
  ::testIlogb<float>(output);
}

void testIlogbD(std::ostream* output) noexcept
{
  ::testIlogb<double>(output);
}

void testIlogbSubnormalF(std::ostream* output) noexcept
{
  ::testIlogbSubnormal<float>(output);
}

void testIlogbSubnormalD(std::ostream* output) noexcept
{
  ::testIlogbSubnormal<double>(output);
}
