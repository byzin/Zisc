/*!
  \file xoshiro_2star_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_2STAR_ENGINE_HPP
#define ZISC_XOSHIRO_2STAR_ENGINE_HPP

// Standard C++ library
#include <array>
#include <cstddef>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*! 
  \brief Xoshir** generator
  */
template <typename Seed, typename Result, uint8 kA, uint8 kB, uint8 kC>
class Xoshiro2StarEngine : public PseudoRandomNumberEngine<
    Xoshiro2StarEngine<Seed, Result, kA, kB, kC>, Seed, Result>
{
  static_assert(kIsUnsignedInteger<Seed>, "Seed isn't unsigned integer type.");
  static_assert(kIsUnsignedInteger<Result>, "Result isn't unsigned integer type.");

 public:
  using SeedType = Seed;
  using ResultType = Result;


  //! Initialize
  Xoshiro2StarEngine() noexcept;

  //! Initialize
  Xoshiro2StarEngine(const SeedType seed) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Return the n which of the period 2^n-1
  static constexpr std::size_t getPeriodPow2() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <typename UnsignedInteger>
  static constexpr bool isEndOfPeriod(const UnsignedInteger sample) noexcept;

  //! Set a seed
  void setSeed(const SeedType seed) noexcept;

 private:
  //!
  template <SeedType k>
  static ResultType rotateLeft(const SeedType x) noexcept;


  std::array<SeedType, 4> state_;
};

// Predefined Xoshiro** type
using Xoshiro2Star32 = Xoshiro2StarEngine<uint32, uint32, 0, 9, 11>;
using Xoshiro2Star64 = Xoshiro2StarEngine<uint64, uint64, 1, 17, 45>;

} // namespace zisc

#include "xoshiro_2star_engine-inl.hpp"

#endif // ZISC_XOSHIRO_2STAR_ENGINE_HPP
