/*!
  \file pow_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <ostream>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/utility.hpp"

template <zisc::FloatingPoint Float> inline
constexpr Float getPowInput(const int i, const int end) noexcept
{
  constexpr Float k = zisc::cast<Float>(8.0);
  const Float e = k * zisc::cast<Float>(i) / zisc::cast<Float>(end);
  return e;
}

void testPowF(std::ostream* output) noexcept;

void testPowD(std::ostream* output) noexcept;

void testPowIntF(std::ostream* output) noexcept;

void testPowIntD(std::ostream* output) noexcept;
