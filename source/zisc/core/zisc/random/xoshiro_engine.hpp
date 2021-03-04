/*!
  \file xoshiro_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_ENGINE_HPP
#define ZISC_XOSHIRO_ENGINE_HPP

// Standard C++ library
#include <array>
#include <cstddef>
// Zisc
#include "pseudo_random_number_engine.hpp"
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

enum class XoshiroMethod : int
{
  Plus,
  PlusPlus,
  StarStar
};

/*!
  \brief Xoshir** generator

  No detailed description.

  \tparam ValueT No description.
  \tparam kMethod No description.
  */
template <UnsignedInteger ValueT, XoshiroMethod kMethod>
class XoshiroEngine : public PseudoRandomNumberEngine<XoshiroEngine<ValueT, kMethod>, ValueT>
{
 public:
  using BaseEngine = PseudoRandomNumberEngine<XoshiroEngine, ValueT>;
  using ValueType = typename BaseEngine::ValueType;


  //! Initialize
  XoshiroEngine() noexcept;

  //! Initialize
  XoshiroEngine(const ValueType seed) noexcept;


  //! Generate a random number
  ValueType generate() noexcept;

  //! Return the n which of the period 2^n-1
  static constexpr std::size_t getPeriodPow2() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <UnsignedInteger Integer>
  static constexpr bool isEndOfPeriod(const Integer sample) noexcept;

  //! Set a seed
  void setSeed(const ValueType seed) noexcept;

 private:
  //!
  static constexpr ValueType a() noexcept;

  //!
  static constexpr ValueType b() noexcept;

  //!
  static constexpr ValueType c() noexcept;

  //! Generate a random number
  ValueType generateRandom() noexcept;

  //! Next seed state
  void nextState() noexcept;

  //!
  template <ValueT k>
  static ValueType rotateLeft(const ValueType x) noexcept;


  alignas(4 * sizeof(ValueType)) std::array<ValueType, 4> state_;
};

// Predefined Xoshiro** type
using XoshiroPlus128 = XoshiroEngine<uint32b, XoshiroMethod::Plus>;
using XoshiroPlus256 = XoshiroEngine<uint64b, XoshiroMethod::Plus>;
using Xoshiro2Plus128 = XoshiroEngine<uint32b, XoshiroMethod::PlusPlus>;
using Xoshiro2Plus256 = XoshiroEngine<uint64b, XoshiroMethod::PlusPlus>;
using Xoshiro2Star128 = XoshiroEngine<uint32b, XoshiroMethod::StarStar>;
using Xoshiro2Star256 = XoshiroEngine<uint64b, XoshiroMethod::StarStar>;

} // namespace zisc

#include "xoshiro_engine-inl.hpp"

#endif // ZISC_XOSHIRO_ENGINE_HPP
