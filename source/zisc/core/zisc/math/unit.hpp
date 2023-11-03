/*!
  \file unit.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_UNIT_HPP
#define ZISC_UNIT_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>

namespace zisc {

//! Convert degree to radian
template <std::floating_point Float>
constexpr auto toRadian(const Float angle) noexcept -> Float;

//! Convert radian to degree
template <std::floating_point Float>
constexpr auto toAngle(const Float radian) noexcept -> Float;

} // namespace zisc

/*!
  \example unit_example.cpp

  This is an example of how to use zisc unit functions.

  \note No notation.
  \attention No attention.
  */

#include "unit-inl.hpp"

#endif // ZISC_UNIT_HPP
