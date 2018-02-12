/*!
  \file pseudo_random_number_engine_example.cpp
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
#include "zisc/pcg_engine.hpp"
#include "zisc/xoroshiro128_plus_engine.hpp"

int main()
{
//  using PrnEngine = zisc::PcgMcgRxsMXs;
  using PrnEngine = zisc::Xoroshiro128PlusEngine;

  constexpr char seed_key[] = "PseudoRandomNumberEngine example";
  constexpr std::uint64_t seed64 = zisc::SipHash64::hash(seed_key);

  PrnEngine engine{seed64};
  for (std::uint32_t sample = 0; sample < 1024; ++sample) {
    const double x = engine.generate01();
    const double y = engine.generate01();
    [[maybe_unused]] const double z = engine.generate01();
    [[maybe_unused]] const double w = engine.generate01();
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<double>::max_digits10)
              << sample << ", " << x << ", " << y << std::endl;
  }

  return 0;
}
