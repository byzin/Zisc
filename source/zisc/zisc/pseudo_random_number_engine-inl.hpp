/*!
  \file pseudo_random_number_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
#define ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP

#include "pseudo_random_number_engine.hpp"
// Standard C++ library
#include <cstdint>
#include <type_traits>
// Zisc
#include "error.hpp"
#include "floating_point.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
auto PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::operator()() noexcept
    -> ResultType
{
  return generate();
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::operator()(
    const Float lower,
    const Float upper) noexcept
{
  return generateFloat(lower, upper);
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
auto PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generate() noexcept
    -> ResultType
{
  return cast<GeneratorClass*>(this)->generate();
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generateFloat(
    const Float lower,
    const Float upper) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float u = generate01Float<Float>();
  const Float value = lower + (upper - lower) * u;
  ZISC_ASSERT(isInBounds(value, lower, upper),
              "The value is out of range [lower, upper).");
  return value;
}

/*!
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generate01Float()
    noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  // Generate a integer random number
  const ResultType x = generate();
  // Map to a [0, 1) float
  using FloatType = FloatingPointFromBytes<sizeof(Float)>;
  return FloatType::mapTo01(x);
}

/*!
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename UnsignedInteger> inline
constexpr bool PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::
    isEndOfPeriod(const UnsignedInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UnsignedInteger>,
                "UnsignedInteger isn't unsigned integer type.");
  const bool result = GeneratorClass::isEndOfPeriod(sample);
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
void PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::setSeed(
    const SeedType seed) noexcept
{
  cast<GeneratorClass*>(this)->setSeed(seed);
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::PseudoRandomNumberEngine()
    noexcept
{
}

/*!
  \details
  No detailed.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::PseudoRandomNumberEngine(
    const PseudoRandomNumberEngine&) noexcept
{
}

} // namespace zisc

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
