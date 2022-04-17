/*!
  \file pcg_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PCG_ENGINE_HPP
#define ZISC_PCG_ENGINE_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
enum class PcgBase : int
{
  Lcg,
  Mcg
};

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  \tparam kBase No description.
  */
template <std::unsigned_integral T, PcgBase kBase>
class PcgEngine : public PseudoRandomNumberEngine<PcgEngine<T, kBase>, T>
{
 public:
  using BaseEngineT = PseudoRandomNumberEngine<PcgEngine, T>;
  using ValueT = typename BaseEngineT::ValueT;


  //! Initialize
  PcgEngine() noexcept;

  //! Initialize
  PcgEngine(const ValueT seed) noexcept;


  //! Generate a random number
  ValueT generate() noexcept;

  //! Return the n which of the period 2^n
  static constexpr std::size_t getPeriodPow2() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <std::unsigned_integral Integer>
  static constexpr bool isEndOfPeriod(const Integer sample) noexcept;

  //! Set seed
  void setSeed(const ValueT seed) noexcept;

 private:
  using BitCountType = uint8b;


  //! Bump
  ValueT bump(const ValueT state) const noexcept;

  //! Generate a base of a random number
  ValueT generateBase() noexcept;

  //! Return the default increment
  static constexpr ValueT increment() noexcept;

  //! Return the default multiplier
  static constexpr ValueT mcgMultiplier() noexcept;

  //! Return the default multiplier
  static constexpr ValueT multiplier() noexcept;

  //!
  static ValueT output(ValueT internal) noexcept;


  static constexpr bool kOutputPrevious = sizeof(ValueT) <= 8;


  ValueT state_;
};

// Type aliases

template <UnsignedInteger T>
using PcgLcgEngine = PcgEngine<T, PcgBase::Lcg>;

template <UnsignedInteger T>
using PcgMcgEngine = PcgEngine<T, PcgBase::Mcg>;

// Predefined PCG engine type
// RxsMXs
using PcgLcgRxsMXs8 = PcgLcgEngine<uint8b>;
using PcgLcgRxsMXs16 = PcgLcgEngine<uint16b>;
using PcgLcgRxsMXs32 = PcgLcgEngine<uint32b>;
using PcgLcgRxsMXs64 = PcgLcgEngine<uint64b>;

} // namespace zisc

#include "pcg_engine-inl.hpp"

#endif // ZISC_PCG_ENGINE_HPP
