/*!
  \file correlated_multi_jittered_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP
#define ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP

// Standard C++ library
#include <array>
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
  //! Generate a [0, 1) float random number
  template <typename Float>
  static Float generate1D(const uint32b s, const uint32b p) noexcept;

  //! Generate two [0, 1) float random numbers
  template <typename Float>
  static std::array<Float, 2> generate2D(uint32b s, const uint32b p) noexcept;

  //! Return the period
  static constexpr std::size_t getPeriod() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <typename UnsignedInteger>
  static constexpr bool isEndOfPeriod(const UnsignedInteger sample) noexcept;

 private:
  //! Hash the i value
  static uint32b hashInteger(uint32b i, const uint32b p) noexcept;

  //! Make a w mask
  static constexpr uint32b makeWMask(const uint32b w) noexcept;

  //! Map an integer value to a [0, 1) floating point value 
  template <typename Float>
  static Float mapTo01Float(const uint32b x) noexcept;

  //!
  template <uint32b l>
  static uint32b permute(uint32b i, const uint32b p) noexcept;

  //! The implementation of permutation
  template <uint32b w>
  static uint32b permuteImpl(uint32b i, const uint32b p) noexcept;
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
