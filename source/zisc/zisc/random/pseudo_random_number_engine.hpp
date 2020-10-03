/*!
  \file pseudo_random_number_engine.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP
#define ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP

// Standard C++ library
#include <cstdint>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Base class of pseudo random number algorithm

  No detailed description.

  \tparam GeneratorClass No description.
  \tparam SeedT No description.
  \tparam ResultT No description.
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
class PseudoRandomNumberEngine
{
 public:
  using GeneratorType = GeneratorClass;
  using SeedType = SeedT;
  using ResultType = ResultT;


  //! Generate a random number
  ResultType operator()() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <FloatingPoint Float>
  Float operator()(const Float lower, const Float upper) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <FloatingPoint Float>
  Float generateFloat(const Float lower, const Float upper) noexcept;

  //! Generate a [0, 1) float random number
  template <FloatingPoint Float>
  Float generate01Float() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <UnsignedInteger Integer>
  static constexpr bool isEndOfPeriod(const Integer sample) noexcept;

  //! Set a random seed
  void setSeed(const SeedType seed) noexcept;

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
