/*!
  \file pcg_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  explicit PcgEngine(const ValueT seed) noexcept;


  //! Generate a random number
  auto generate() noexcept -> ValueT;

  //! Return the n which of the period 2^n
  static constexpr auto getPeriodPow2() noexcept -> std::size_t;

  //! Check if a specified sample (0 base count) is the end of period
  template <std::unsigned_integral Integer>
  static constexpr auto isEndOfPeriod(const Integer sample) noexcept -> bool;

  //! Set seed
  void setSeed(const ValueT seed) noexcept;

 private:
  using BitCountType = uint8b;


  //! Bump
  auto bump(const ValueT state) const noexcept -> ValueT;

  //! Generate a base of a random number
  auto generateBase() noexcept -> ValueT;

  //! Return the default increment
  static constexpr auto increment() noexcept -> ValueT;

  //! Return the default multiplier
  static constexpr auto mcgMultiplier() noexcept -> ValueT;

  //! Return the default multiplier
  static constexpr auto multiplier() noexcept -> ValueT;

  //!
  static auto output(ValueT internal) noexcept -> ValueT;


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
