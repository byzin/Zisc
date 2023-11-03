/*!
  \file unit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_INL_HPP
#define ZISC_UNIT_INL_HPP

#include "unit.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <numbers>
// Zisc
#include "zisc/utility.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] angle No description.
  \return Radian
  */
template <std::floating_point Float> inline
constexpr auto toRadian(const Float angle) noexcept -> Float
{
  constexpr Float k = std::numbers::pi_v<Float> / cast<Float>(180.0);
  const Float radian = k * angle;
  return radian;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] radian No description.
  \return Degree
  */
template <std::floating_point Float> inline
constexpr auto toAngle(const Float radian) noexcept -> Float
{
  constexpr Float k = cast<Float>(180.0) / std::numbers::pi_v<Float>;
  const Float angle = k * radian;
  return angle;
}

} // namespace zisc

#endif // ZISC_UNIT_INL_HPP
