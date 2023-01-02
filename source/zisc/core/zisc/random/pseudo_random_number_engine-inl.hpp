/*!
  \file pseudo_random_number_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
#define ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP

#include "pseudo_random_number_engine.hpp"
// Standard C++ library
#include <concepts>
#include <cstdint>
#include <type_traits>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
auto PseudoRandomNumberEngine<GeneratorClass, T>::operator()() noexcept -> ValueT 
{
  const ValueT result = generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, T>::operator()(
    const Float lower,
    const Float upper) noexcept
{
  const Float result = generate(lower, upper);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
constexpr auto PseudoRandomNumberEngine<GeneratorClass, T>::defaultSeed() noexcept
    -> ValueT
{
  ValueT seed = 0;
  if constexpr (sizeof(ValueT) == 4)
    seed = 0xa01755e8u;
  else if constexpr (sizeof(ValueT) == 8)
    seed = 0x698d965fcddf1a9dull;
  return seed;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
auto PseudoRandomNumberEngine<GeneratorClass, T>::generate() noexcept
    -> ValueT
{
  const ValueT result = static_cast<GeneratorT*>(this)->generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, T>::generate(
    const Float lower,
    const Float upper) noexcept
{
  const Float u = generate01<Float>();
  const Float result = lower + (upper - lower) * u;
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, T>::generate01() noexcept
{
  // Generate a integer random number
  const ValueT x = generate();
  // Map to a [0, 1) float
  const Float y = mapTo01<Float>(x);
  return y;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] sample No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral T>
template <std::unsigned_integral Integer> inline
constexpr bool PseudoRandomNumberEngine<GeneratorClass, T>::
    isEndOfPeriod(const Integer sample) noexcept
{
  const bool result = GeneratorT::isEndOfPeriod(sample);
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
void PseudoRandomNumberEngine<GeneratorClass, T>::setSeed(
    const ValueT seed) noexcept
{
  static_cast<GeneratorT*>(this)->setSeed(seed);
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
PseudoRandomNumberEngine<GeneratorClass, T>::PseudoRandomNumberEngine() noexcept
{
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, std::unsigned_integral T> inline
PseudoRandomNumberEngine<GeneratorClass, T>::PseudoRandomNumberEngine(
    const PseudoRandomNumberEngine&) noexcept
{
}

} // namespace zisc

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
