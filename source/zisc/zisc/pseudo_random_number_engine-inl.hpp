/*!
  \file pseudo_random_number_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
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
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

template <>
struct EngineFloatInfo<sizeof(float)>
{
  using FloatType = float;
  static constexpr int kFloatBitSize = sizeof(float) * 8;
  static constexpr int kMantissaBitSize = 23;
  static constexpr int kNonMantissaBitSize = kFloatBitSize - kMantissaBitSize;
  static constexpr uint32 kExponentBit = cast<uint32>(127u) << kMantissaBitSize;
};

template <>
struct EngineFloatInfo<sizeof(double)>
{
  using FloatType = double;
  static constexpr int kFloatBitSize = sizeof(double) * 8;
  static constexpr int kMantissaBitSize = 52;
  static constexpr int kNonMantissaBitSize = kFloatBitSize - kMantissaBitSize;
  static constexpr uint64 kExponentBit = cast<uint64>(1023ull) << kMantissaBitSize;
};

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
  return generate(lower, upper);
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
Float PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generate(
    const Float lower,
    const Float upper) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  const Float u = cast<Float>(generate01());
  const Float value = lower + (upper - lower) * u;
  ZISC_ASSERT(isInBounds(value, lower, upper),
              "The value is out of range [lower, upper).");
  return value;
}

/*!
  */
template <typename GeneratorClass, typename Seed, typename Result> inline
auto PseudoRandomNumberEngine<GeneratorClass, Seed, Result>::generate01() noexcept
    -> FloatType
{
  // Generate a integer random number
  const Result x = generate();
  // Convert to [0, 1) float value
  using FloatInfo = EngineFloatInfo<sizeof(Result)>;
  constexpr auto exponent = FloatInfo::kExponentBit;
  const FloatValue u{exponent | (x >> FloatInfo::kNonMantissaBitSize)};
  const FloatType value = u.float_ - cast<FloatType>(1.0);
  ZISC_ASSERT(isInBounds(value, cast<FloatType>(0.0), cast<FloatType>(1.0)),
              "The value is out of range [0, 1).");
  return value;
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
  static_assert(sizeof(FloatValue) == sizeof(ResultType), "The union is invalid.");
  static_assert(sizeof(FloatValue) == sizeof(FloatType), "The union is invalid.");
  static_assert(alignof(FloatValue) == alignof(ResultType), "The union is invalid.");
  static_assert(alignof(FloatValue) == alignof(FloatType), "The union is invalid.");
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
