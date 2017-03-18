/*!
  \file xoroshiro128_plus_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOROSHIRO128_PLUS_ENGINE_HPP
#define ZISC_XOROSHIRO128_PLUS_ENGINE_HPP

// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
 \brief Xoroshiro128+ random number generator
 \details
 http://xoroshiro.di.unimi.it/
 */
class Xoroshiro128PlusEngine :
    public PseudoRandomNumberEngine<Xoroshiro128PlusEngine, uint64, uint64>
{
 public:
  using SeedType = uint64;
  using ResultType = uint64;


  //! Initialize generator using default random seed
  Xoroshiro128PlusEngine() noexcept;

  //! Initialize generator
  Xoroshiro128PlusEngine(const SeedType seed) noexcept;

  //! Initialize generator
  Xoroshiro128PlusEngine(const SeedType seed1, const SeedType seed2) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Set random seed
  void setSeed(const SeedType seed) noexcept;

  //! Set random seed
  void setSeed(const SeedType seed1, const SeedType seed2) noexcept;

 private:
  template <int bit>
  static ResultType rotateLeft(const ResultType x) noexcept;


  SeedType seed_[2];
};

} // namespace zisc

#include "xoroshiro128_plus_engine-inl.hpp"

#endif // ZISC_XOROSHIRO128_PLUS_ENGINE_HPP
