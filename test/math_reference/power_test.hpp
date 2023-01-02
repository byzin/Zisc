/*!
  \file pow_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_POWER_TEST_HPP
#define ZISC_MATH_REFERENCE_POWER_TEST_HPP

// Standard C++ library
#include <concepts>
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"
// Test
#include "basic_test.hpp"

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr Float makePowInput(const Float x) noexcept
{
  constexpr auto k = zisc::cast<Float>(16.0);
  const Float e = k * x;
  return e;
}

#endif // ZISC_MATH_REFERENCE_POWER_TEST_HPP
