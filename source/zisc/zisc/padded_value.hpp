/*!
  \file padded_value.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_PADDED_VALUE_HPP
#define ZISC_PADDED_VALUE_HPP

// Standard C++ library
#include <array>
#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc_config.hpp"

namespace zisc {

// Forward declaration
template <typename, std::size_t> struct PaddedValueImpl;

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  \tparam kSize No description.
  */
template <typename Type, std::size_t kSize>
class PaddedValue
{
 public:
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using RReference = std::add_rvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;


  //! Create a value
  PaddedValue() noexcept;

  //! Move a data
  PaddedValue(RReference other) noexcept;

  //! Move a data
  PaddedValue(PaddedValue&& other) noexcept;


  //! Move a data
  PaddedValue& operator=(RReference other) noexcept;

  //! Move a data
  PaddedValue& operator=(PaddedValue&& other) noexcept;

  //! Return the value
  operator Reference() noexcept
  {
    return get();
  }

  //! Return the value
  operator ConstReference() const noexcept
  {
    return get();
  }


  //! Return the value
  Reference get() noexcept;

  //! Return the value
  ConstReference get() const noexcept;

 private:
  static constexpr std::size_t kPadSize = std::max(sizeof(Type), kSize) -
                                          sizeof(Type);
  PaddedValueImpl<Type, kPadSize> impl_;
};

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  \tparam kPadSize No description.
  */
template <typename Type, std::size_t kPadSize>
struct PaddedValueImpl
{
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using RReference = std::add_rvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;


  //! Create a value
  PaddedValueImpl() noexcept {}

  //! Copy a data
  PaddedValueImpl(ConstReference other) noexcept : value_{other}, padding_{} {}

  //! Move a data
  PaddedValueImpl(RReference other) noexcept : value_{std::move(other)}, padding_{} {}


  //! Copy a data
  PaddedValueImpl& operator=(ConstReference other) noexcept
  {
    value_ = other;
    return *this;
  }

  //! Copy a data
  PaddedValueImpl& operator=(RReference other) noexcept
  {
    value_ = std::move(other);
    return *this;
  }


  Type value_;
  std::array<uint8b, kPadSize> padding_;
};

} // namespace zisc

#include "padded_value-inl.hpp"

#endif // ZISC_PADDED_VALUE_HPP
