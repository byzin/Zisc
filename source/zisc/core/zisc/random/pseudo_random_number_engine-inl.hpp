/*!
  \file pseudo_random_number_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
auto PseudoRandomNumberEngine<GeneratorClass, ValueT>::operator()() noexcept
    -> ValueType 
{
  const ValueType result = generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral ValueT>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, ValueT>::operator()(
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
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
constexpr auto PseudoRandomNumberEngine<GeneratorClass, ValueT>::defaultSeed() noexcept
    -> ValueType
{
  ValueType seed = 0;
  if constexpr (sizeof(ValueType) == 4)
    seed = 0xa01755e8u;
  else if constexpr (sizeof(ValueType) == 8)
    seed = 0x698d965fcddf1a9dull;
  return seed;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
auto PseudoRandomNumberEngine<GeneratorClass, ValueT>::generate() noexcept
    -> ValueType
{
  const ValueType result = static_cast<GeneratorType*>(this)->generate();
  return result;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lower No description.
  \param [in] upper No description.
  \return No description
  */
template <typename GeneratorClass, std::unsigned_integral ValueT>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, ValueT>::generate(
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
template <typename GeneratorClass, std::unsigned_integral ValueT>
template <std::floating_point Float> inline
Float PseudoRandomNumberEngine<GeneratorClass, ValueT>::generate01() noexcept
{
  // Generate a integer random number
  const ValueType x = generate();
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
template <typename GeneratorClass, std::unsigned_integral ValueT>
template <std::unsigned_integral Integer> inline
constexpr bool PseudoRandomNumberEngine<GeneratorClass, ValueT>::
    isEndOfPeriod(const Integer sample) noexcept
{
  const bool result = GeneratorType::isEndOfPeriod(sample);
  return result;
}

/*!
  \details No detailed description

  \param [in] seed No description.
  */
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
void PseudoRandomNumberEngine<GeneratorClass, ValueT>::setSeed(
    const ValueType seed) noexcept
{
  static_cast<GeneratorType*>(this)->setSeed(seed);
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
PseudoRandomNumberEngine<GeneratorClass, ValueT>::PseudoRandomNumberEngine() noexcept
{
}

/*!
  \details No detailed description
  */
template <typename GeneratorClass, std::unsigned_integral ValueT> inline
PseudoRandomNumberEngine<GeneratorClass, ValueT>::PseudoRandomNumberEngine(
    const PseudoRandomNumberEngine&) noexcept
{
}

} // namespace zisc

#endif // ZISC_PSEUDO_RANDOM_NUMBER_ENGINE_INL_HPP
