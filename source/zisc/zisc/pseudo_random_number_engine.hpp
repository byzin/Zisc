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
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Base class of pseudo random number algorithm

  No detailed description.

  \tparam GeneratorClass No description.
  \tparam Seed No description.
  \tparam Result No description.
  */
template <typename GeneratorClass, typename Seed, typename Result>
class PseudoRandomNumberEngine
{
  static_assert(kIsUnsignedInteger<Seed>, "Seed isn't unsigned integer type.");
  static_assert(kIsUnsignedInteger<Result>, "Result isn't unsigned integer type.");

 public:
  using GeneratorType = GeneratorClass;
  using SeedType = Seed;
  using ResultType = Result;


  //! Generate a random number
  ResultType operator()() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <typename Float>
  Float operator()(const Float lower, const Float upper) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <typename Float>
  Float generateFloat(const Float lower, const Float upper) noexcept;

  //! Generate a [0, 1) float random number
  template <typename Float>
  Float generate01Float() noexcept;

  //! Check if a specified sample (0 base count) is the end of period
  template <typename UInteger>
  static constexpr bool isEndOfPeriod(const UInteger sample) noexcept;

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

template <typename GeneratorClass>
using RandomNumberEngine = PseudoRandomNumberEngine<
                               GeneratorClass,
                               typename GeneratorClass::SeedType,
                               typename GeneratorClass::ResultType>;

} // namespace zisc

/*!
  \example pseudo_random_number_engine_example.cpp

  This is an example of how to use zisc pseudo random number generators.
  */

#include "pseudo_random_number_engine-inl.hpp"

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP
