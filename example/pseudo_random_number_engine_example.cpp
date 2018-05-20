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
#include "zisc/zisc_config.hpp"

namespace {

template <typename GeneratorClass, typename Seed, typename Result>
using PrnEngine = zisc::PseudoRandomNumberEngine<GeneratorClass, Seed, Result>;

template <typename GeneratorClass, typename Seed, typename Result>
void generateRandomNumbers(PrnEngine<GeneratorClass, Seed, Result>* engine)
{
  for (std::uint32_t sample = 0; sample < 1024; ++sample) {
    const float x = engine->template generateFloat<float>(0.0f, 1.0f);
    const float y = engine->template generateFloat<float>(0.0f, 1.0f);
    [[maybe_unused]] const float z = engine->template generate01Float<float>();
    [[maybe_unused]] const float w = engine->template generate01Float<float>();
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<float>::max_digits10)
              << sample << ", " << x << ", " << y << std::endl;
  }
}

template <typename Type> constexpr Type getSeed() {return 123456789;}
template <> constexpr zisc::uint8 getSeed<zisc::uint8>() {return 12u;}
template <> constexpr zisc::uint16 getSeed<zisc::uint16>() {return 12345u;}

template <typename GeneratorClass>
void runRngTest(const std::string_view&)
{
  using SeedType = typename GeneratorClass::SeedType;
  constexpr auto seed = getSeed<SeedType>();
  GeneratorClass engine{seed};
  generateRandomNumbers(&engine);
}

} // namespace

int main(int argc, char** argv)
{
  const std::string_view engine_name{(1 < argc) ? argv[1] : ""};

  const std::string_view pcg8_name{"pcg8"};
  const std::string_view pcg16_name{"pcg16"};
  const std::string_view pcg32_name{"pcg32"};
  const std::string_view pcg64_name{"pcg64"};

  if (engine_name == pcg8_name)
    runRngTest<zisc::PcgLcgRxsMXs8>(pcg8_name);
  if (engine_name == pcg16_name)
    runRngTest<zisc::PcgLcgRxsMXs16>(pcg16_name);
  if (engine_name == pcg32_name)
    runRngTest<zisc::PcgLcgRxsMXs32>(pcg32_name);
  else if (engine_name == pcg64_name)
    runRngTest<zisc::PcgLcgRxsMXs64>(pcg64_name);
  else {
    std::cerr << "Please select a PRN engine ["
        << pcg8_name << ", "
        << pcg16_name << ", "
        << pcg32_name << ", "
        << pcg64_name << "]." << std::endl;
  }

  return 0;
}
