/*!
  \file bit-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_INL_HPP
#define ZISC_BIT_INL_HPP

#include "bit.hpp"
// Standard C++ library
#include <bit>
// Zisc
#include "concepts.hpp"
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
constexpr auto Bit::castBit(const From& from) noexcept -> To
{
  const To to =
#if defined(Z_CLANG)
      __builtin_bit_cast(To, from);
#else
      std::bit_cast<To, From>(from);
#endif
  return to;
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
constexpr auto bit_cast(const From& from) noexcept -> To
{
  const To to = Bit::castBit<To, From>(from);
  return to;
}

} // namespace zisc

#endif // ZISC_BIT_INL_HPP
