/*!
  \file xoshiro_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_XOSHIRO_ENGINE_HPP
#define ZISC_XOSHIRO_ENGINE_HPP

// Standard C++ library
#include <array>
#include <concepts>
#include <cstddef>
// Zisc
#include "pseudo_random_number_engine.hpp"
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

  \tparam T No description.
  \tparam kMethod No description.
  */
template <std::unsigned_integral T, XoshiroMethod kMethod>
class XoshiroEngine : public PseudoRandomNumberEngine<XoshiroEngine<T, kMethod>, T>
{
 public:
  using BaseEngineT = PseudoRandomNumberEngine<XoshiroEngine, T>;
  using ValueT = typename BaseEngineT::ValueT;


  //! Initialize
  XoshiroEngine() noexcept;

  //! Initialize
  explicit XoshiroEngine(const ValueT seed) noexcept;


  //! Generate a random number
  auto generate() noexcept -> ValueT;

  //! Return the n which of the period 2^n-1
  static constexpr auto getPeriodPow2() noexcept -> std::size_t;

  //! Check if a specified sample (0 base count) is the end of period
  template <std::unsigned_integral Integer>
  static constexpr auto isEndOfPeriod(const Integer sample) noexcept -> bool;

  //! Set a seed
  void setSeed(const ValueT seed) noexcept;

 private:
  //!
  static constexpr auto a() noexcept -> ValueT;

  //!
  static constexpr auto b() noexcept -> ValueT;

  //!
  static constexpr auto c() noexcept -> ValueT;

  //! Generate a random number
  auto generateRandom() noexcept -> ValueT;

  //! Next seed state
  void nextState() noexcept;

  //!
  template <T k>
  static auto rotateLeft(const ValueT x) noexcept -> ValueT;


  alignas(4 * sizeof(ValueT)) std::array<ValueT, 4> state_;
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
