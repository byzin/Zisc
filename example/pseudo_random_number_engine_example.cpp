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
#include "zisc/utility.hpp"
#include "zisc/xorshift_star_engine.hpp"
#include "zisc/xoshiro_2star_engine.hpp"
#include "zisc/zisc_config.hpp"

namespace {

template <typename GeneratorClass, typename Seed, typename Result>
using PrnEngine = zisc::PseudoRandomNumberEngine<GeneratorClass, Seed, Result>;

template <typename Type> constexpr Type getSeed() {return 123456789;}
template <> constexpr zisc::uint8 getSeed<zisc::uint8>() {return 12u;}
template <> constexpr zisc::uint16 getSeed<zisc::uint16>() {return 12345u;}

template <typename GeneratorClass, typename Seed, typename Result>
void generateRandomNumbers(PrnEngine<GeneratorClass, Seed, Result>* engine)
{
  using SeedType = typename GeneratorClass::SeedType;
  auto seed = getSeed<SeedType>();
  auto update_engine = [engine, &seed](std::uint32_t& s)
  {
    if (engine->isEndOfPeriod(s++)) {
      ++seed;
      engine->setSeed(seed);
      s = 0;
    }
  };

  for (std::uint32_t sample = 0, s = 0; sample < 1024; ++sample) {
    const float x = engine->template generateFloat<float>(0.0f, 1.0f);
    update_engine(s);
    const float y = engine->template generateFloat<float>(0.0f, 1.0f);
    update_engine(s);
    if (!zisc::isInBounds(x, 0.0f, 1.0f))
      std::cerr << "sample[" << sample << "] x(" << x << ") is out of range [0, 1)";
    if (!zisc::isInBounds(y, 0.0f, 1.0f))
      std::cerr << "sample[" << sample << "] y(" << y << ") is out of range [0, 1)";
    std::cout << std::fixed
              << std::setprecision(std::numeric_limits<float>::max_digits10)
              << sample << ", " << x << ", " << y << std::endl;
  }
}

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
  const std::string_view xorshift_star8_name{"xorshift*8"};
  const std::string_view xorshift_star16_name{"xorshift*16"};
  const std::string_view xorshift_star32_name{"xorshift*32"};
  const std::string_view xoshiro_2star32_name{"xoshiro**32"};
  const std::string_view xoshiro_2star64_name{"xoshiro**64"};

  if (engine_name == pcg8_name)
    runRngTest<zisc::PcgLcgRxsMXs8>(pcg8_name);
  else if (engine_name == pcg16_name)
    runRngTest<zisc::PcgLcgRxsMXs16>(pcg16_name);
  else if (engine_name == pcg32_name)
    runRngTest<zisc::PcgLcgRxsMXs32>(pcg32_name);
  else if (engine_name == pcg64_name)
    runRngTest<zisc::PcgLcgRxsMXs64>(pcg64_name);
  else if (engine_name == xorshift_star8_name)
    runRngTest<zisc::XorshiftStar8>(xorshift_star8_name);
  else if (engine_name == xorshift_star16_name)
    runRngTest<zisc::XorshiftStar16>(xorshift_star16_name);
  else if (engine_name == xorshift_star32_name)
    runRngTest<zisc::XorshiftStar32>(xorshift_star32_name);
  else if (engine_name == xoshiro_2star32_name)
    runRngTest<zisc::Xoshiro2Star32>(xoshiro_2star32_name);
  else if (engine_name == xoshiro_2star64_name)
    runRngTest<zisc::Xoshiro2Star64>(xoshiro_2star64_name);
  else {
    std::cerr << "Please select a PRN engine ["
        << pcg8_name << ", "
        << pcg16_name << ", "
        << pcg32_name << ", "
        << pcg64_name << ", "
        << xorshift_star8_name << ", "
        << xorshift_star16_name << ", "
        << xorshift_star32_name << ", "
        << xoshiro_2star32_name << ", "
        << xoshiro_2star64_name << "]." << std::endl;
  }

  return 0;
}
