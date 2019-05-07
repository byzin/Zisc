/*!
  \file xorshift_star_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XORSHIFT_STAR_ENGINE_HPP
#define ZISC_XORSHIFT_STAR_ENGINE_HPP

// Standard C++ library
#include <cstddef>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Seed, typename Result,
          Seed kMultiplier, uint8b kA, uint8b kB, uint8b kC>
class XorshiftStarEngine : public PseudoRandomNumberEngine<
    XorshiftStarEngine<Seed, Result, kMultiplier, kA, kB, kC>, Seed, Result>
{
  static_assert(kIsUnsignedInteger<Seed>, "Seed isn't unsigned integer type.");
  static_assert(kIsUnsignedInteger<Result>, "Result isn't unsigned integer type.");

 public:
  using SeedType = Seed;
  using ResultType = Result;


  //! Initialize
  XorshiftStarEngine() noexcept;

  //! Initialize
  XorshiftStarEngine(const SeedType seed) noexcept;


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
  //! Next an state
  void advance() noexcept;

  //! Return the multiplier value
  static constexpr SeedType multiplier() noexcept;


  SeedType state_;
};

// Predefined Xorshift* type
using XorshiftStar8 = XorshiftStarEngine<uint16b, uint8b, 0x58f5u, 5, 9, 8>;
using XorshiftStar16 = XorshiftStarEngine<uint32b, uint16b, 0xb2e1cb1du, 6, 17, 9>;
using XorshiftStar32 = XorshiftStarEngine<uint64b, uint32b, 0xd989bcacc137dcd5ull, 11, 31, 18>;

} // namespace zisc

#include "xorshift_star_engine-inl.hpp"

#endif // ZISC_XORSHIFT_STAR_ENGINE_HPP
