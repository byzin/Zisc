/*!
  \file pseudo_random_number_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
auto PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::operator()() noexcept
    -> ResultType
{
  const ResultType result = generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::operator()(
    const Float lower,
    const Float upper) noexcept
{
  const Float result = generateFloat(lower, upper);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
auto PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generate() noexcept
    -> ResultType
{
  const ResultType result = cast<GeneratorClass*>(this)->generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generateFloat(
    const Float lower,
    const Float upper) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float u = generate01Float<Float>();
  const Float result = lower + (upper - lower) * u;
  ZISC_ASSERT(isInBounds(result, lower, upper),
              "The result is out of range [lower, upper).");
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
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
  const Float result = FloatType::mapTo01(x);
  return result;
}

/*!
  \details No detailed description

  \tparam UInteger No description.
  \param [in] sample No description.
  \return No description
  */
template <typename GeneratorClass, typename Seed, typename Result>
template <typename UInteger> inline
constexpr bool PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::
    isEndOfPeriod(const UInteger sample) noexcept
{
  static_assert(kIsUnsignedInteger<UInteger>,
                "UInteger isn't unsigned integer type.");
  const bool result = GeneratorClass::isEndOfPeriod(sample);
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
void PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::setSeed(
    const SeedType seed) noexcept
{
  cast<GeneratorClass*>(this)->setSeed(seed);
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::PseudoRandomNumberEngine()
    noexcept
{
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::PseudoRandomNumberEngine(
    const PseudoRandomNumberEngine&) noexcept
{
}

} // namespace zisc

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
