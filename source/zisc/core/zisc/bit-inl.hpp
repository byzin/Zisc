/*!
  \file bit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_INL_HPP
#define ZISC_BIT_INL_HPP

#include "bit.hpp"
// Standard C++ library
#include <bit>
#include <limits>
#include <type_traits>
// Zisc
#include "concepts.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam To No description.
  \tparam From No description.
  \param [in] from No description.
  \return No description
  */
template <TriviallyCopyable To, TriviallyCopyable From> inline
constexpr To Bit::castBit(const From& from) noexcept
{
  const To to =
#if defined(Z_CLANG)
      __builtin_bit_cast(To, from);
#else
      std::bit_cast<To, From>(from);
#endif
  return to;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int Bit::countLZero(const Integer x) noexcept
{
  const int result = std::countl_zero(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int Bit::countLOne(const Integer x) noexcept
{
  const int result = std::countl_one(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int Bit::countPop(const Integer x) noexcept
{
  const int result = std::popcount(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int Bit::countRZero(const Integer x) noexcept
{
  const int result = std::countr_zero(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int Bit::countROne(const Integer x) noexcept
{
  const int result = std::countr_one(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::ceil(const Integer x) noexcept
{
  const Integer result =
#if defined(Z_CLANG)
      Zisc::ceil(x);
#else // Z_CLANG
      std::bit_ceil(x);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::floor(const Integer x) noexcept
{
  const Integer result =
#if defined(Z_CLANG)
      Zisc::floor(x);
#else // Z_CLANG
      std::bit_floor(x);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::getWidth(const Integer x) noexcept
{
  const Integer result =
#if defined(Z_CLANG)
      Zisc::getWidth(x);
#else // Z_CLANG
      std::bit_width(x);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr bool Bit::isPowerOf2(const Integer x) noexcept
{
  const bool result =
#if defined(Z_CLANG)
      Zisc::isPowerOf2(x);
#else // Z_CLANG
      std::has_single_bit(x);
#endif // Z_CLANG
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::Zisc::ceil(const Integer x) noexcept
{
  Integer result = 1;
  if (2 <= x) {
    if constexpr (std::is_same_v<Integer, decltype(+x)>) {
      result = cast<Integer>(1) << bit_width(cast<Integer>(x - 1));
    }
    else { // for types subject to integral promotion
      constexpr int offset = std::numeric_limits<unsigned>::digits -
                             std::numeric_limits<Integer>::digits;
      result = cast<Integer>(1u << (bit_width(cast<Integer>(x - 1)) + offset) >> offset);
    }
  }
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::Zisc::floor(const Integer x) noexcept
{
  const Integer result = (x == 0)
    ? cast<Integer>(0)
    : cast<Integer>(cast<Integer>(1) << (bit_width(x) - 1));
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::Zisc::getWidth(const Integer x) noexcept
{
  constexpr auto n = cast<Integer>(std::numeric_limits<Integer>::digits);
  const auto zeros = cast<Integer>(countl_zero(x));
  const Integer result = n - zeros;
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr bool Bit::Zisc::isPowerOf2(const Integer x) noexcept
{
  const bool result = (x != 0) && ((x & (x - 1)) == 0);
  return result;
}

// STL style function aliases

/*!
  \details No detailed description

  \tparam To No description.
  \tparam From No description.
  \param [in] from No description.
  \return No description
  */
template <TriviallyCopyable To, TriviallyCopyable From> inline
constexpr To bit_cast(const From& from) noexcept
{
  const To to = Bit::castBit<To, From>(from);
  return to;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int countl_zero(const Integer x) noexcept
{
  const int result = Bit::countLZero(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int countl_one(const Integer x) noexcept
{
  const int result = Bit::countLOne(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int countr_zero(const Integer x) noexcept
{
  const int result = Bit::countRZero(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int countr_one(const Integer x) noexcept
{
  const int result = Bit::countROne(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr int popcount(const Integer x) noexcept
{
  const int result = Bit::countPop(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr bool has_single_bit(const Integer x) noexcept
{
  const bool result = Bit::isPowerOf2(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer bit_ceil(const Integer x) noexcept
{
  const Integer result = Bit::ceil(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer bit_floor(const Integer x) noexcept
{
  const Integer result = Bit::floor(x);
  return result;
}

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer bit_width(const Integer x) noexcept
{
  const Integer result = Bit::getWidth(x);
  return result;
}

} // namespace zisc

#endif // ZISC_BIT_INL_HPP
