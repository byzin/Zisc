/*!
  \file pseudo_random_number_engine.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
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
#include "zisc/algorithm.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/random/pcg_engine.hpp"
#include "zisc/random/pseudo_random_number_engine.hpp"
#include "zisc/random/xoshiro_engine.hpp"

namespace {

template <typename GeneratorClass, typename ValueT>
using PrnEngine = zisc::PseudoRandomNumberEngine<GeneratorClass, ValueT>;

template <typename Type> constexpr Type getSeed() {return 123456789;}
template <> constexpr zisc::uint8b getSeed<zisc::uint8b>() {return 12u;}
template <> constexpr zisc::uint16b getSeed<zisc::uint16b>() {return 12345u;}

template <typename GeneratorClass, typename ValueT>
void generateRandomNumbers(PrnEngine<GeneratorClass, ValueT>* engine)
{
  using ValueType = typename GeneratorClass::ValueType;
  auto seed = getSeed<ValueType>();
  auto update_engine = [engine, &seed](std::uint32_t& s)
  {
    if (engine->isEndOfPeriod(s++)) {
      ++seed;
      engine->setSeed(seed);
      s = 0;
    }
  };

  for (std::uint32_t sample = 0, s = 0; sample < 1024; ++sample) {
    const float x = engine->template generate<float>(0.0f, 1.0f);
    update_engine(s);
    const float y = engine->template generate<float>(0.0f, 1.0f);
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
void runRngTest([[maybe_unused]] const std::string_view& name)
{
  using ValueType = typename GeneratorClass::ValueType;
  constexpr auto seed = getSeed<ValueType>();
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
  const std::string_view xoshiro_2star128_name{"xoshiro**128"};
  const std::string_view xoshiro_2star256_name{"xoshiro**256"};

  if (engine_name == pcg8_name)
    runRngTest<zisc::PcgLcgRxsMXs8>(pcg8_name);
  else if (engine_name == pcg16_name)
    runRngTest<zisc::PcgLcgRxsMXs16>(pcg16_name);
  else if (engine_name == pcg32_name)
    runRngTest<zisc::PcgLcgRxsMXs32>(pcg32_name);
  else if (engine_name == pcg64_name)
    runRngTest<zisc::PcgLcgRxsMXs64>(pcg64_name);
  else if (engine_name == xoshiro_2star128_name)
    runRngTest<zisc::Xoshiro2Star128>(xoshiro_2star128_name);
  else if (engine_name == xoshiro_2star256_name)
    runRngTest<zisc::Xoshiro2Star256>(xoshiro_2star256_name);
  else {
    std::cerr << "Please select a PRN engine ["
        << pcg8_name << ", "
        << pcg16_name << ", "
        << pcg32_name << ", "
        << pcg64_name << ", "
        << xoshiro_2star128_name << ", "
        << xoshiro_2star256_name << "]." << std::endl;
  }

  return 0;
}
