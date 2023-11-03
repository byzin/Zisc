/*!
  \file correlated_multi_jittered_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP
#define ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP

// Standard C++ library
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Correlated Multi-Jittered Sampler

  For more detail, please see the following paper:
  <a href="https://graphics.pixar.com/library/MultiJitteredSampling/">Correlated Multi-Jittered Sampling</a>.

  \tparam kRootN No description.
  */
template <uint32b kRootN>
class CorrelatedMultiJitteredEngine
{
 public:
  using ValueT = uint32b;


  //! Generate a [0, 1) float random number
  template <std::floating_point Float>
  static auto generate1d(const ValueT s, const ValueT p) noexcept -> Float;

  //! Generate two [0, 1) float random numbers
  template <std::floating_point Float>
  static auto generate2d(ValueT s, const ValueT p) noexcept -> std::array<Float, 2>;

  //! Return the period
  static constexpr auto getPeriod() noexcept -> std::size_t;

  //! Check if a specified sample (0 base count) is the end of period
  template <std::unsigned_integral Integer>
  static constexpr auto isEndOfPeriod(const Integer sample) noexcept -> bool;

 private:
  //! Hash the i value
  static auto hashInteger(ValueT i, const ValueT p) noexcept -> ValueT;

  //! Make a w mask
  static constexpr auto makeWMask(const ValueT w) noexcept -> ValueT;

  //!
  template <uint32b l>
  static auto permute(ValueT i, const ValueT p) noexcept -> ValueT;

  //! The implementation of permutation
  template <uint32b w>
  static auto permuteImpl(ValueT i, const ValueT p) noexcept -> ValueT;
};

// Type aliases
using CmjN16 = CorrelatedMultiJitteredEngine<4>;
using CmjN64 = CorrelatedMultiJitteredEngine<8>;
using CmjN81 = CorrelatedMultiJitteredEngine<9>;
using CmjN256 = CorrelatedMultiJitteredEngine<16>;

} // namespace zisc

/*!
  \example correlated_multi_jittered_engine_example.cpp

  This is an example of how to use zisc::CorrelatedMultiJitteredEngine.
  */

#include "correlated_multi_jittered_engine-inl.hpp"

#endif // ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP
