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
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
inline
auto PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::operator()() noexcept
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
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
template <FloatingPoint Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::operator()(
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
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
inline
auto PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::generate() noexcept
    -> ResultType
{
  const ResultType result = cast<GeneratorType*>(this)->generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
template <FloatingPoint Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::generateFloat(
    const Float lower,
    const Float upper) noexcept
{
  const Float u = generate01Float<Float>();
  const Float result = lower + (upper - lower) * u;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
template <FloatingPoint Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::generate01Float() noexcept
{
  // Generate a integer random number
  const ResultType x = generate();
  // Map to a [0, 1) float
  const auto y = mapTo01<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
template <UnsignedInteger Integer> inline
constexpr bool PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::
    isEndOfPeriod(const Integer sample) noexcept
{
  const bool result = GeneratorType::isEndOfPeriod(sample);
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
void PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::setSeed(
    const SeedType seed) noexcept
{
  cast<GeneratorType*>(this)->setSeed(seed);
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::PseudoRandomNumberEngine() noexcept
{
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, UnsignedInteger SeedT, UnsignedInteger ResultT>
PseudoRandomNumberEngine<GeneratorClass, SeedT, ResultT>::PseudoRandomNumberEngine(
    const PseudoRandomNumberEngine&) noexcept
{
}

} // namespace zisc

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
