/*!
  \file pcg_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "zisc/concepts.hpp"
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

  \tparam ValueT No description.
  \tparam kBase No description.
  */
template <UnsignedInteger ValueT, PcgBase kBase>
class PcgEngine : public PseudoRandomNumberEngine<PcgEngine<ValueT, kBase>, ValueT>
{
 public:
  using BaseEngine = PseudoRandomNumberEngine<PcgEngine, ValueT>;
  using ValueType = typename BaseEngine::ValueType;


  //! Initialize
  PcgEngine() noexcept;

  //! Initialize
  PcgEngine(const ValueType seed) noexcept;


  //! Generate a random number
  ValueType generate() noexcept;

  //! Return the n which of the period 2^n
  static constexpr std::size_t getPeriodPow2() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <UnsignedInteger Integer>
  static constexpr bool isEndOfPeriod(const Integer sample) noexcept;

  //! Set seed
  void setSeed(const ValueType seed) noexcept;

 private:
  using BitCountType = uint8b;


  //! Bump
  ValueType bump(const ValueType state) const noexcept;

  //! Generate a base of a random number
  ValueType generateBase() noexcept;

  //! Return the default increment
  static constexpr ValueType increment() noexcept;

  //! Return the default multiplier
  static constexpr ValueType mcgMultiplier() noexcept;

  //! Return the default multiplier
  static constexpr ValueType multiplier() noexcept;

  //!
  static ValueType output(ValueType internal) noexcept;


  static constexpr bool kOutputPrevious = sizeof(ValueType) <= 8;


  ValueType state_;
};

// Type aliases

template <UnsignedInteger ValueT>
using PcgLcgEngine = PcgEngine<ValueT, PcgBase::Lcg>;

template <UnsignedInteger ValueT>
using PcgMcgEngine = PcgEngine<ValueT, PcgBase::Mcg>;

// Predefined PCG engine type
// RxsMXs
using PcgLcgRxsMXs8 = PcgLcgEngine<uint8b>;
using PcgLcgRxsMXs16 = PcgLcgEngine<uint16b>;
using PcgLcgRxsMXs32 = PcgLcgEngine<uint32b>;
using PcgLcgRxsMXs64 = PcgLcgEngine<uint64b>;

} // namespace zisc

#include "pcg_engine-inl.hpp"

#endif // ZISC_PCG_ENGINE_HPP
