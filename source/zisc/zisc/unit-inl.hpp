/*!
  \file unit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_INL_HPP
#define ZISC_UNIT_INL_HPP

#include "unit.hpp"
// Standard C++ library
#include <cstddef>
// Zisc
#include "concepts.hpp"
#include "utility.hpp"
#include "math/math.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] angle No description.
  \return Radian
  */
template <FloatingPoint Float> inline
constexpr Float toRadian(const Float angle) noexcept
{
  constexpr Float k = kPi<Float> / cast<Float>(180.0);
  const Float radian = k * angle;
  return radian;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] radian No description.
  \return Degree
  */
template <FloatingPoint Float> inline
constexpr Float toAngle(const Float radian) noexcept
{
  constexpr Float k = cast<Float>(180.0) / kPi<Float>;
  const Float angle = k * radian;
  return angle;
}

} // namespace zisc

#endif // ZISC_UNIT_INL_HPP
