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

enum class PcgAlgorithm : int
{
  XshRs,
  XshRr,
  RxsMXs
};

/*!
  \details
  No detailed.
  */
template <PcgBase Base, PcgAlgorithm Algorithm, typename Seed, typename Result>
class PcgEngine : public PseudoRandomNumberEngine<
    PcgEngine<Base, Algorithm, Seed, Result>, Seed, Result>
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

  //! Set seed
  void setSeed(const SeedType seed) noexcept;

 private:
  //! Bound
  template <typename Type>
  Type bound(const ResultType random, const Type lower, const Type upper) const noexcept;

  //! Bump
  SeedType bump(const SeedType state) const noexcept;

  //! Return the default increment
  static constexpr SeedType increment() noexcept;

  //! Return the default multiplier
  static constexpr SeedType multiplier() noexcept;

  //! Next state
  void next() noexcept;

  //! Output random
  ResultType output(SeedType internal) const noexcept;


  SeedType state_;
};

template <PcgAlgorithm Algorithm, typename Seed, typename Result>
using PcgLcgEngine = PcgEngine<PcgBase::Lcg, Algorithm, Seed, Result>;

template <PcgAlgorithm Algorithm, typename Seed, typename Result>
using PcgMcgEngine = PcgEngine<PcgBase::Mcg, Algorithm, Seed, Result>;

// Predefined PCG engine type
using PcgLcgXshRr = PcgLcgEngine<PcgAlgorithm::XshRr, uint64, uint32>;
using PcgMcgXshRr = PcgMcgEngine<PcgAlgorithm::XshRr, uint64, uint32>;
using PcgLcgRxsMXs = PcgLcgEngine<PcgAlgorithm::RxsMXs, uint64, uint64>;
using PcgMcgRxsMXs = PcgMcgEngine<PcgAlgorithm::RxsMXs, uint64, uint64>;
using PcgLcgRxsMXs32 = PcgLcgEngine<PcgAlgorithm::RxsMXs, uint32, uint32>;
using PcgMcgRxsMXs32 = PcgMcgEngine<PcgAlgorithm::RxsMXs, uint32, uint32>;

} // namespace zisc

#include "pcg_engine-inl.hpp"

#endif // ZISC_PCG_ENGINE_HPP
