/*!
  \file split_mix64_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_SPLIT_MIX64_ENGINE_HPP
#define ZISC_SPLIT_MIX64_ENGINE_HPP

// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \brief SplitMix64 random number generator
 \details
 http://xoroshiro.di.unimi.it/splitmix64.c
 */
class SplitMix64Engine :
    public PseudoRandomNumberEngine<SplitMix64Engine, uint64, uint64>
{
 public:
  using SeedType = uint64;
  using ResultType = uint64;


  //! Initialize generator using default random seed
  SplitMix64Engine() noexcept;

  //! Initialize generator
  SplitMix64Engine(const SeedType seed) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Set random seed
  void setSeed(const SeedType seed) noexcept;

 private:
  SeedType seed_;
};

} // namespace zisc

#include "split_mix64_engine-inl.hpp"

#endif // ZISC_SPLIT_MIX64_ENGINE_HPP
