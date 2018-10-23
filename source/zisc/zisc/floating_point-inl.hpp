/*!
  \file floating_point-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOATING_POINT_INL_HPP
#define ZISC_FLOATING_POINT_INL_HPP

#include "floating_point.hpp"
// Standard C++ library
#include <cstdint>
#include <cstddef>
// Zisc
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

template <>
struct FloatRepresentation<2>
{
  using FloatType = void*;
  using RepresentationType = uint16b;
};

template <>
struct FloatRepresentation<4>
{
  using FloatType = float;
  using RepresentationType = uint32b;
};

template <>
struct FloatRepresentation<8>
{
  using FloatType = double;
  using RepresentationType = uint64b;
};

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<2>::exponentBias() noexcept
{
  return 15;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<4>::exponentBias() noexcept
{
  return 127;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<8>::exponentBias() noexcept
{
  return 1023;
}

/*!
  */
template <std::size_t kN> inline
constexpr auto FloatingPoint<kN>::exponentBitMask() noexcept -> BitType
{
  BitType mask = 0b0u;
  for (std::size_t bit = 0; bit < exponentBitSize(); ++bit) {
    constexpr BitType one = 0b1u;
    mask = cast<BitType>(mask | (one << (bit + significandBitSize())));
  }
  return mask;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<2>::exponentBitSize() noexcept
{
  return 5;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<4>::exponentBitSize() noexcept
{
  return 8;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<8>::exponentBitSize() noexcept
{
  return 11;
}

/*!
  */
template <std::size_t kN> inline
constexpr auto FloatingPoint<kN>::signBitMask() noexcept -> BitType
{
  const auto mask = BitType{0b1u} << (exponentBitSize() + significandBitSize());
  return mask;
}

/*!
  */
template <std::size_t kN> inline
constexpr auto FloatingPoint<kN>::significandBitMask() noexcept -> BitType
{
  BitType mask = 0b0u;
  for (std::size_t bit = 0; bit < significandBitSize(); ++bit) {
    constexpr BitType one = 0b1u;
    mask = cast<BitType>(mask | (one << bit));
  }
  return mask;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<2>::significandBitSize() noexcept
{
  return 10;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<4>::significandBitSize() noexcept
{
  return 23;
}

/*!
  */
template <> inline
constexpr std::size_t FloatingPoint<8>::significandBitSize() noexcept
{
  return 52;
}

} // namespace zisc

#endif // ZISC_FLOATING_POINT_INL_HPP
