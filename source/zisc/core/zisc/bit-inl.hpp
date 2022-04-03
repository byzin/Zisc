/*!
  \file bit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
  const Integer result = std::bit_ceil(x);
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
  const Integer result = std::bit_floor(x);
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
  const Integer result = std::bit_width(x);
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
  const bool result = std::has_single_bit(x);
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
