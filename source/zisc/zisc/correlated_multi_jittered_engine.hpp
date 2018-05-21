/*!
  \file correlated_multi_jittered_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
  \brief Correlated Multi-Jittered Sampling
  \details
  "Correlated Multi-Jittered Sampling"
  Andrew Kensler, Pixar Technical Memo 13-01, 2013
  */
template <uint32 kRootN>
class CorrelatedMultiJitteredEngine
{
 public:
  //! Generate a [0, 1) float random number
  template <typename Float>
  static Float generate1D(const uint32 s, const uint32 p) noexcept;

  //! Generate two [0, 1) float random numbers
  template <typename Float>
  static std::array<Float, 2> generate2D(uint32 s, const uint32 p) noexcept;

  //! Return the period
  static constexpr std::size_t getPeriod() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <typename UnsignedInteger>
  static constexpr bool isEndOfPeriod(const UnsignedInteger sample) noexcept;

 private:
  //! Hash the i value
  static uint32 hashInteger(uint32 i, const uint32 p) noexcept;

  //! Make a w mask
  static constexpr uint32 makeWMask(const uint32 w) noexcept;

  //! Map an integer value to a [0, 1) floating point value 
  template <typename Float>
  static Float mapTo01Float(const uint32 x) noexcept;

  //!
  template <uint32 l>
  static uint32 permute(uint32 i, const uint32 p) noexcept;

  //! The implementation of permutation
  template <uint32 w>
  static uint32 permuteImpl(uint32 i, const uint32 p) noexcept;
};

using CmjN16 = CorrelatedMultiJitteredEngine<4>;
using CmjN64 = CorrelatedMultiJitteredEngine<8>;
using CmjN256 = CorrelatedMultiJitteredEngine<16>;

} // namespace zisc

#include "correlated_multi_jittered_engine-inl.hpp"

#endif // ZISC_CORRELATED_MULTI_JITTERED_ENGINE_HPP
