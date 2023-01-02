/*!
  \file pseudo_random_number_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP
#define ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP

// Standard C++ library
#include <concepts>
#include <cstdint>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Base class of pseudo random number algorithm

  No detailed description.

  \tparam GeneratorClass No description.
  \tparam ValueT No description.
  */
template <typename GeneratorClass, std::unsigned_integral T>
class PseudoRandomNumberEngine
{
 public:
  using GeneratorT = GeneratorClass;
  using ValueT = T;


  //! Generate a random number
  ValueT operator()() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <std::floating_point Float>
  Float operator()(const Float lower, const Float upper) noexcept;


  //! Return the default seed
  static constexpr ValueT defaultSeed() noexcept;

  //! Generate a random number
  ValueT generate() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <std::floating_point Float>
  Float generate(const Float lower, const Float upper) noexcept;

  //! Generate a [0, 1) float random number
  template <std::floating_point Float>
  Float generate01() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <std::unsigned_integral Integer>
  static constexpr bool isEndOfPeriod(const Integer sample) noexcept;

  //! Set a random seed
  void setSeed(const ValueT seed) noexcept;

 protected:
  //! Initialize the PRNE
  PseudoRandomNumberEngine() noexcept;

  //! Copy a PRNE state
  PseudoRandomNumberEngine(const PseudoRandomNumberEngine&) noexcept;

  // Delete function
  PseudoRandomNumberEngine& operator=(const PseudoRandomNumberEngine&) = delete;
};

} // namespace zisc

/*!
  \example pseudo_random_number_engine_example.cpp

  This is an example of how to use zisc pseudo random number generators.
  */

#include "pseudo_random_number_engine-inl.hpp"

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP
