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
#include <string_view>
// Zisc
#include "zisc/sip_hash_engine.hpp"
#include "zisc/pcg_engine.hpp"
#include "zisc/pseudo_random_number_engine.hpp"
#include "zisc/split_mix64_engine.hpp"
#include "zisc/xoroshiro128_plus_engine.hpp"
#include "zisc/xorshift_engine.hpp"
#include "zisc/xsadd_engine.hpp"

namespace {

template <typename GeneratorClass, typename Seed, typename Result>
using PrnEngine = zisc::PseudoRandomNumberEngine<GeneratorClass, Seed, Result>;

template <typename GeneratorClass, typename Seed, typename Result>
void generateRandomNumbers(PrnEngine<GeneratorClass, Seed, Result>* engine)
{
  using Float = typename PrnEngine<GeneratorClass, Seed, Result>::FloatType;
  for (std::uint32_t sample = 0; sample < 1024; ++sample) {
    const Float x = engine->generate01();
    const Float y = engine->generate01();
    [[maybe_unused]] const Float z = engine->generate01();
    [[maybe_unused]] const Float w = engine->generate01();
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<Float>::max_digits10)
              << sample << ", " << x << ", " << y << std::endl;
  }
}

} // namespace

int main(int argc, char** argv)
{
  const std::string_view engine_name{(1 < argc) ? argv[1] : ""};
  const std::string_view seed_key{"PseudoRandomNumberEngine example"};

  auto run_xorshift32 = [](const std::string_view& key)
  {
    const std::uint32_t seed = zisc::SipHash32::hash(key);
    zisc::Xorshift32 engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_xorshift128_plus = [](const std::string_view& key)
  {
    const std::uint64_t seed = zisc::SipHash64::hash(key);
    zisc::Xorshift128Plus engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_xsadd = [](const std::string_view& key)
  {
    const std::uint32_t seed = zisc::SipHash32::hash(key);
    zisc::XsaddEngine engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_pcg32 = [](const std::string_view& key)
  {
    const std::uint32_t seed = zisc::SipHash32::hash(key);
    zisc::PcgLcgRxsMXs32 engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_pcg64 = [](const std::string_view& key)
  {
    const std::uint64_t seed = zisc::SipHash64::hash(key);
    zisc::PcgMcgRxsMXs engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_split_mix64 = [](const std::string_view& key)
  {
    const std::uint64_t seed = zisc::SipHash64::hash(key);
    zisc::SplitMix64Engine engine{seed};
    generateRandomNumbers(&engine);
  };

  auto run_xoroshiro128_plus = [](const std::string_view& key)
  {
    const std::uint64_t seed = zisc::SipHash64::hash(key);
    zisc::Xoroshiro128PlusEngine engine{seed};
    generateRandomNumbers(&engine);
  };

  const std::string_view xorshift32_name{"xorshift32"};
  const std::string_view xorshift128_plus_name{"xorshift128+"};
  const std::string_view xsadd_name{"xsadd"};
  const std::string_view pcg32_name{"pcg32"};
  const std::string_view pcg64_name{"pcg64"};
  const std::string_view split_mix64_name{"splitmix64"};
  const std::string_view xoroshiro128_plus_name{"xoroshiro128+"};

  if (engine_name == xorshift32_name) {
    run_xorshift32(seed_key);
  }
  else if (engine_name == xorshift128_plus_name) {
    run_xorshift128_plus(seed_key);
  }
  else if (engine_name == xsadd_name) {
    run_xsadd(seed_key);
  }
  else if (engine_name == pcg32_name) {
    run_pcg32(seed_key);
  }
  else if (engine_name == pcg64_name) {
    run_pcg64(seed_key);
  }
  else if (engine_name == split_mix64_name) {
    run_split_mix64(seed_key);
  }
  else if (engine_name == xoroshiro128_plus_name) {
    run_xoroshiro128_plus(seed_key);
  }
  else {
    std::cerr << "Please select a PRN engine ["
        << xorshift32_name << ", "
        << xorshift128_plus_name << ", "
        << xsadd_name << ", "
        << pcg32_name << ", "
        << pcg64_name << ", "
        << split_mix64_name << ", "
        << xoroshiro128_plus_name << "]." << std::endl;
  }

  return 0;
}
