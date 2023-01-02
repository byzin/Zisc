/*!
  \file bit.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BIT_HPP
#define ZISC_BIT_HPP

// Zisc
#include "concepts.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Bit
{
 public:
  //! Reinterpret the object representation of one type as that of another
  template <TriviallyCopyable To, TriviallyCopyable From>
  static constexpr To castBit(const From& from) noexcept;
};

// STL style function aliases

//! Reinterpret the object representation of one type as that of another
template <TriviallyCopyable To, TriviallyCopyable From>
constexpr To bit_cast(const From& from) noexcept;

} // namespace zisc

#include "bit-inl.hpp"

#endif // ZISC_BIT_HPP
