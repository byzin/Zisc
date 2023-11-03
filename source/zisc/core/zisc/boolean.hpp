/*!
  \file boolean.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BOOLEAN_HPP
#define ZISC_BOOLEAN_HPP

// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief Capable of holding one of the two values: true or false.

  Capable of holding one of the two values: true or false.
  The sizeof(Boolean) is 1 is guaranteed.

  \note No notation.
  \attention No attention.
  */
class Boolean
{
 public:
  //! Create a false value
  constexpr Boolean() noexcept;

  //! Create a value with a built-in boolean
  constexpr Boolean(const bool value) noexcept;


  //! Convert from a built-in boolean
  constexpr auto operator=(const bool value) noexcept -> Boolean&;

  //! Convert to a built-in boolean
  constexpr operator bool() const noexcept;


  //! Convert from a built-in boolean
  constexpr void set(const bool value) noexcept;

  //! Convert to a built-in boolean
  [[nodiscard]]
  constexpr auto toBool() const noexcept -> bool;

 private:
  //! Return the false value
  static constexpr auto getFalse() noexcept -> uint8b;

  //! Return the true value
  static constexpr auto getTrue() noexcept -> uint8b;

  //! Return the underlying state
  [[nodiscard]]
  constexpr auto state() const noexcept -> uint8b;


  uint8b state_;
};

} /* namespace zisc */

#include "boolean-inl.hpp"

#endif /* ZISC_BOOLEAN_HPP */
