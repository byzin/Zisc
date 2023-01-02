/*!
  \file correlated_multi_jittered_engine_example.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include "zisc/utility.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"
#include "zisc/random/correlated_multi_jittered_engine.hpp"

namespace {

template <std::uint32_t kRootN>
void generateRandomNumbers(const std::string_view& seed_key)
{
  using CmjEngine = zisc::CorrelatedMultiJitteredEngine<kRootN>;

  std::uint32_t seed = zisc::Fnv1aHash32::hash(seed_key);
  auto update_engine = [&seed](std::uint32_t& s)
  {
    if (CmjEngine::isEndOfPeriod(s++)) {
      ++seed;
      s = 0;
    }
  };

  for (std::uint32_t sample = 0, s = 0; sample < 1024; ++sample) {
    const std::uint32_t p = zisc::Fnv1aHash32::hash(seed);
    const auto xy = CmjEngine::template generate2d<double>(s, p);
    update_engine(s);
    if (!zisc::isInBounds(xy[0], 0.0, 1.0))
      std::cerr << "sample[" << sample << "] x(" << xy[0] << ") is out of range [0, 1)";
    if (!zisc::isInBounds(xy[1], 0.0, 1.0))
      std::cerr << "sample[" << sample << "] y(" << xy[1] << ") is out of range [0, 1)";
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
