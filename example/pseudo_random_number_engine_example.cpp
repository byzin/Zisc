/*!
  \file pseudo_random_number_engine_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */ 

// Standard C++ library
#include <cstdint>
#include <iostream>
#include <iomanip>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/pcg_engine.hpp"
#include "zisc/xsadd_engine.hpp"
#include "zisc/xorshift_engine.hpp"

template <typename EngineType>
void tryPrneExample(EngineType& engine, const char* engine_name)
{
  constexpr int n = 10;
  std::cout << std::setw(12) << engine_name << ": ";
  for (int i = 0; i < n; ++i)
    std::cout << std::fixed << std::setprecision(4) << engine(0.0, 1.0) << " ";
  std::cout << std::endl;
}

int main()
{
  // PseudoRandomNumber engine example
  std::cout << "PseudoRandomNumber engine example" << std::endl;
  constexpr char seed_key[] = "PseudoRandomNumberEngine example";
  constexpr std::uint32_t seed32 = zisc::toHash32(seed_key);
  constexpr std::uint64_t seed64 = zisc::toHash64(seed_key);
  std::cout << "Seed32: " << seed32 << std::endl;
  std::cout << "Seed64: " << seed64 << std::endl;
  // Xorshift32 engine
  zisc::Xorshift32 xorshift_engine{seed32};
  tryPrneExample(xorshift_engine, "Xorshift32");
  // XSadd engine
  zisc::XsaddEngine xsadd_engine{seed32};
  tryPrneExample(xsadd_engine, "XSadd");
  // Xorshift128 engine
  zisc::Xorshift128Plus xorshift128_engine{seed64};
  tryPrneExample(xorshift128_engine, "Xorshift128");
  // PCG engine
  zisc::PcgMcgXshRr pcg_engine{seed64};
  tryPrneExample(pcg_engine, "PCG");

  return 0;
}
