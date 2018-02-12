/*!
  \file pseudo_random_number_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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
  */
template <uint Byte> struct PrnEngineFloatInfo;

/*!
  \brief Base class of pseudo random number algorithm
  \details
  No detailed.
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
  using FloatType = typename PrnEngineFloatInfo<sizeof(Result)>::FloatType;


  //! Generate a random number
  ResultType operator()() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <typename Float>
  Float operator()(const Float lower, const Float upper) noexcept;


  //! Generate a random number
  ResultType generate() noexcept;

  //! Generate a float random number x satisfying [lower, upper)
  template <typename Float>
  Float generate(const Float lower, const Float upper) noexcept;

  //! Generate a [0, 1) float random number
  FloatType generate01() noexcept;

  //! Map an integer value to a [0, 1) floating point value
  static FloatType mapTo01Float(const ResultType x) noexcept;

  //! Set a random seed
  void setSeed(const SeedType seed) noexcept;

 protected:
  //! Initialize the PRNE
  PseudoRandomNumberEngine() noexcept;

  //! Copy a PRNE state
  PseudoRandomNumberEngine(const PseudoRandomNumberEngine&) noexcept;

  // Delete function
  PseudoRandomNumberEngine& operator=(const PseudoRandomNumberEngine&) = delete;

 private:
  //! Used for conversion from integer to float
  union FloatValue
  {
    ResultType integer_;
    FloatType float_;
    FloatValue(const ResultType integer) : integer_{integer} {}
  };
};

template <typename GeneratorClass>
using RandomNumberEngine = PseudoRandomNumberEngine<
                               GeneratorClass,
                               typename GeneratorClass::SeedType,
                               typename GeneratorClass::ResultType>;

} // namespace zisc

#include "pseudo_random_number_engine-inl.hpp"

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_HPP
