/*!
  \file correlated_multi_jittered_engine_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>
// Zisc
#include "zisc/sip_hash_engine.hpp"
#include "zisc/math.hpp"
#include "zisc/correlated_multi_jittered_engine.hpp"

int main()
{
  constexpr std::uint32_t root_n = 16;
  constexpr std::uint32_t n = zisc::power<2>(root_n);

  using CmjEngine = zisc::CorrelatedMultiJitteredEngine<root_n>;

  constexpr char seed_key[] = "CorrelatedMultiJitteredEngine example";
  constexpr std::uint32_t seed32 = zisc::SipHash32::hash(seed_key);

  for (std::uint32_t sample = 0; sample < 1024; ++sample) {
    const std::uint32_t s = sample % n;
    const std::uint32_t p = seed32 + 64 * (sample / (n));
    const auto xy = CmjEngine::generate2D<double>(s, p);
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<double>::max_digits10)
              << sample << ", " << xy[0] << ", " << xy[1] << std::endl;
  }

  return 0;
}
