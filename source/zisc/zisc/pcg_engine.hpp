/*!
  \file pcg_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PCG_ENGINE_HPP
#define ZISC_PCG_ENGINE_HPP

// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

enum class PcgBase : int
{
  Lcg,
  Mcg
};

enum class PcgMethod : int
{
  XshRs,
  XshRr,
  RxsMXs
};

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgMethod Method, typename Seed, typename Result>
class PcgEngine : public PseudoRandomNumberEngine<
    PcgEngine<Base, Method, Seed, Result>, Seed, Result>
{
  static_assert(kIsUnsignedInteger<Seed>, "Seed isn't unsigned integer type.");
  static_assert(kIsUnsignedInteger<Result>, "Result isn't unsigned integer type.");

 public:
  using SeedType = Seed;
  using ResultType = Result;


  //! Initialize
  PcgEngine() noexcept;

  //! Initialize
  PcgEngine(const SeedType seed) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Return the n which of the period 2^n
  static constexpr std::size_t getPeriodPow2() noexcept;

  //! Set seed
  void setSeed(const SeedType seed) noexcept;

 private:
  using BitCountType = uint8;


  //! Bump
  SeedType bump(const SeedType state) const noexcept;

  //! Generate a base of a random number
  SeedType generateBase() noexcept;

  //! Return the default increment
  static constexpr SeedType increment() noexcept;

  //! Return the default multiplier
  static constexpr SeedType multiplier() noexcept;


  static constexpr bool kOutputPrevious = sizeof(SeedType) <= 8;


  SeedType state_;
};

template <PcgMethod Method, typename Seed, typename Result>
using PcgLcgEngine = PcgEngine<PcgBase::Lcg, Method, Seed, Result>;

template <PcgMethod Method, typename Seed, typename Result>
using PcgMcgEngine = PcgEngine<PcgBase::Mcg, Method, Seed, Result>;

// Predefined PCG engine type
// XshRs
using PcgLcgXshRs8 = PcgLcgEngine<PcgMethod::XshRs, uint16, uint8>;
using PcgLcgXshRs16 = PcgLcgEngine<PcgMethod::XshRs, uint32, uint16>;
using PcgLcgXshRs32 = PcgLcgEngine<PcgMethod::XshRs, uint64, uint32>;
using PcgMcgXshRs8 = PcgMcgEngine<PcgMethod::XshRs, uint16, uint8>;
using PcgMcgXshRs16 = PcgMcgEngine<PcgMethod::XshRs, uint32, uint16>;
using PcgMcgXshRs32 = PcgMcgEngine<PcgMethod::XshRs, uint64, uint32>;
// XshRr
using PcgLcgXshRr8 = PcgLcgEngine<PcgMethod::XshRr, uint16, uint8>;
using PcgLcgXshRr16 = PcgLcgEngine<PcgMethod::XshRr, uint32, uint16>;
using PcgLcgXshRr32 = PcgLcgEngine<PcgMethod::XshRr, uint64, uint32>;
using PcgMcgXshRr8 = PcgMcgEngine<PcgMethod::XshRr, uint16, uint8>;
using PcgMcgXshRr16 = PcgMcgEngine<PcgMethod::XshRr, uint32, uint16>;
using PcgMcgXshRr32 = PcgMcgEngine<PcgMethod::XshRr, uint64, uint32>;
// RxsMXs
using PcgLcgRxsMXs8 = PcgLcgEngine<PcgMethod::RxsMXs, uint8, uint8>;
using PcgLcgRxsMXs16 = PcgLcgEngine<PcgMethod::RxsMXs, uint16, uint16>;
using PcgLcgRxsMXs32 = PcgLcgEngine<PcgMethod::RxsMXs, uint32, uint32>;
using PcgLcgRxsMXs64 = PcgLcgEngine<PcgMethod::RxsMXs, uint64, uint64>;

} // namespace zisc

#include "pcg_engine-inl.hpp"

#endif // ZISC_PCG_ENGINE_HPP
