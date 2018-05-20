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
#include <string_view>
// Zisc
#include "zisc/sip_hash_engine.hpp"
#include "zisc/math.hpp"
#include "zisc/correlated_multi_jittered_engine.hpp"

namespace {

template <std::uint32_t kRootN>
void generateRandomNumbers(const std::string_view& seed_key)
{
  using CmjEngine = zisc::CorrelatedMultiJitteredEngine<kRootN>;
  constexpr std::uint32_t n = zisc::power<2>(kRootN);

  const std::uint32_t seed = zisc::SipHash32::hash(seed_key);
  for (std::uint32_t sample = 0; sample < 1024; ++sample) {
    const std::uint32_t s = sample % n;
    const std::uint32_t p = seed + n * (sample / (n));
    const auto xy = CmjEngine::template generate2D<double>(s, p);
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<double>::max_digits10)
              << sample << ", " << xy[0] << ", " << xy[1] << std::endl;
  }
}

} // namespace

int main(int argc, char** argv)
{
  const std::string_view engine_name{(1 < argc) ? argv[1] : ""};
  const std::string_view seed_key{"CorrelatedMultiJitteredEngine example"};

  const std::string_view cmj_n16_name{"cmj-n16"};
  const std::string_view cmj_n64_name{"cmj-n64"};
  const std::string_view cmj_n81_name{"cmj-n81"};
  const std::string_view cmj_n256_name{"cmj-n256"};
  const std::string_view cmj_n1024_name{"cmj-n1024"};
  const std::string_view cmj_n4096_name{"cmj-n4096"};

  if (engine_name == cmj_n16_name) {
    generateRandomNumbers<4>(seed_key);
  }
  else if (engine_name == cmj_n64_name) {
    generateRandomNumbers<8>(seed_key);
  }
  else if (engine_name == cmj_n81_name) {
    generateRandomNumbers<9>(seed_key);
  }
  else if (engine_name == cmj_n256_name) {
    generateRandomNumbers<16>(seed_key);
  }
  else if (engine_name == cmj_n1024_name) {
    generateRandomNumbers<32>(seed_key);
  }
  else if (engine_name == cmj_n4096_name) {
    generateRandomNumbers<64>(seed_key);
  }
  else {
    std::cerr << "Please select a CMJ engine ["
        << cmj_n16_name << ", "
        << cmj_n64_name << ", "
        << cmj_n81_name << ", "
        << cmj_n256_name << ", "
        << cmj_n1024_name << ", "
        << cmj_n4096_name << "]." << std::endl;
  }

  return 0;
}
