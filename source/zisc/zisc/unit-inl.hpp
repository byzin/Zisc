/*!
  \file unit-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_INL_HPP
#define ZISC_UNIT_INL_HPP

#include "unit.hpp"
// Standard C++ library
#include <cstddef>
#include <type_traits>
// Zisc
#include "math.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace zisc {

/*!
  \details
  No detailed.

  \param[in] angle angle
  \return radian
  */
template <typename Float> inline
constexpr Float toRadian(const Float angle) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float k = kPi<Float> / cast<Float>(180.0);
  return k * angle;
}

/*!
  \details
  No detailed.

  \param[in] radian radian
  \return degree
  */
template <typename Float> inline
constexpr Float toAngle(const Float radian) noexcept
{
  static_assert(kIsFloat<Float>, "Float isn't floating point type.");
  constexpr Float k = cast<Float>(180.0) / kPi<Float>;
  return k * radian;
}

} // namespace zisc

#endif // ZISC_UNIT_INL_HPP
