/*!
  \file bit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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

namespace zisc {

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
constexpr Integer Bit::ceil(const Integer x) noexcept
{
  Integer result = 1;
  if (2 <= x) {
    if constexpr (std::is_same_v<Integer, decltype(+x)>) {
      result = cast<Integer>(1) << bit_width(cast<Integer>(x - 1));
    }
    else { // for types subject to integral promotion
      constexpr int offset = std::numeric_limits<unsigned>::digits - std::numeric_limits<Integer>::digits;
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
constexpr Integer Bit::floor(const Integer x) noexcept
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
constexpr Integer Bit::getWidth(const Integer x) noexcept
{
  constexpr auto n = cast<Integer>(std::numeric_limits<Integer>::digits);
  const auto zeros = cast<Integer>(std::countl_zero(x));
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
constexpr bool Bit::isPowerOf2(const Integer x) noexcept
{
  const bool result = (x != 0) && ((x & (x - 1)) == 0);
  return result;
}

// STL style function aliases

/*!
  \details No detailed description

  \tparam Integer No description.
  \param [in] x No description.
  \return No description
  */
template <UnsignedInteger Integer> inline
static constexpr bool has_single_bit(const Integer x) noexcept
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
static constexpr Integer bit_ceil(const Integer x) noexcept
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
static constexpr Integer bit_floor(const Integer x) noexcept
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
static constexpr Integer bit_width(const Integer x) noexcept
{
  const Integer result = Bit::getWidth(x);
  return result;
}

} // namespace zisc

#endif // ZISC_BIT_INL_HPP
