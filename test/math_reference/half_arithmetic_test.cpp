/*!
  \file half_arithmetic_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \note No notation.
  \attention No attention.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */


// Standard C++ library
#include <bit>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <random>
#include <iostream>
#include <iomanip>
#include <limits>
#include <ostream>
#include <vector>
// Half float
#include "./half-2.2.0/include/half.hpp"

using half_float::half;
static_assert(sizeof(half) == 2);
using uint16b = std::int16_t;
static_assert(sizeof(uint16b) == 2);

namespace {

void print(std::ostream& out, const float f) noexcept
{
  out << std::scientific
      << std::setprecision(std::numeric_limits<float>::max_digits10 + 2)
      << f
      << std::endl;
}

void print(std::ostream& out, const uint16b u) noexcept
{
  out << u;
}

void print(std::ostream& out, const half h) noexcept
{
  const uint16b v = __builtin_bit_cast(uint16b, h);
  print(out, v);
//  const auto f = static_cast<float>(h);
//  print(std::cout, f);
}

} /* namespace */

int main(int argc, char** argv)
{
  std::ofstream output{"half_arithmetic_reference.bin", std::ios_base::binary};

  const half hmin = std::numeric_limits<half>::min();
  const half hmax = std::numeric_limits<half>::max();
  const half hdmin = std::numeric_limits<half>::denorm_min();
  std::cout << "max       : ";
  ::print(std::cout, static_cast<float>(hmin));
  std::cout << "min       : ";
  ::print(std::cout, static_cast<float>(hmax));
  std::cout << "denorm min: ";
  ::print(std::cout, static_cast<float>(hdmin));
  const uint16b umin = __builtin_bit_cast(uint16b, hmin);
  const uint16b umax = __builtin_bit_cast(uint16b, hmax);
  const uint16b udmin = __builtin_bit_cast(uint16b, hdmin);
  const uint16b udmax = umin - 1;

  constexpr std::size_t n = 50000;
  static_assert(sizeof(n) == 8);
  output << n;

  std::vector<uint16b> normal_list;
  normal_list.resize(2 * n, 0);
  std::vector<uint16b> subnormal_list;
  subnormal_list.resize(2 * n, 0);

  std::mt19937_64 engine{123'456'789};
  {
    std::uniform_int_distribution<uint16b> sampler{umin, umax};
    for (std::size_t i = 0; i < n; ++i) {
      const uint16b x = sampler(engine);
      const uint16b y = sampler(engine);
      ::print(output, x);
      ::print(output, y);
      normal_list[2 * i + 0] = x;
      normal_list[2 * i + 1] = y;
    }
  }
  {
    std::uniform_int_distribution<uint16b> sampler{udmin, udmax};
    for (std::size_t i = 0; i < n; ++i) {
      const uint16b x = sampler(engine);
      const uint16b y = sampler(engine);
      ::print(output, x);
      ::print(output, y);
      subnormal_list[2 * i + 0] = x;
      subnormal_list[2 * i + 1] = y;
    }
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, normal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, normal_list[2 * i + 1]);
    const half result = x + y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, normal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, normal_list[2 * i + 1]);
    const half result = x - y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, normal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, normal_list[2 * i + 1]);
    const half result = x * y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, normal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, normal_list[2 * i + 1]);
    const half result = x / y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, subnormal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, subnormal_list[2 * i + 1]);
    const half result = x + y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, subnormal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, subnormal_list[2 * i + 1]);
    const half result = x - y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, subnormal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, subnormal_list[2 * i + 1]);
    const half result = x * y;
    ::print(output, result);
  }

  for (std::size_t i = 0; i < n; ++i) {
    const half x = __builtin_bit_cast(half, subnormal_list[2 * i + 0]);
    const half y = __builtin_bit_cast(half, subnormal_list[2 * i + 1]);
    const half result = x / y;
    ::print(output, result);
  }

  return 0;
}
