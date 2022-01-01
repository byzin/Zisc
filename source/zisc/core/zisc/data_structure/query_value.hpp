/*!
  \file query_value.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUERY_VALUE_HPP
#define ZISC_QUERY_VALUE_HPP

// Standard C++ library
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <UnsignedInteger T>
class QueryValue
{
 public:
  // Type aliases
  using Type = std::remove_volatile_t<T>;
  using ConstType = std::add_const_t<Type>;
  using Reference = std::add_lvalue_reference_t<Type>;
  using RReference = std::add_rvalue_reference_t<Type>;
  using ConstReference = std::add_lvalue_reference_t<ConstType>;
  using Pointer = std::add_pointer_t<Type>;
  using ConstPointer = std::add_pointer_t<ConstType>;


  //! Create a invalid value
  QueryValue() noexcept;

  //! Create a value
  QueryValue(ConstType value) noexcept;


  //! Move data
  QueryValue& operator=(ConstType value) noexcept;


  //! Set the underlying value to the invalid value
  void clear() noexcept;

  //! Return the underlying value
  Reference get() noexcept;

  //! Return the underlying value
  ConstReference get() const noexcept;

  //! Return the invalid value
  static constexpr Type invalidValue() noexcept;

  //! Check if the underlying value is valid
  bool isValid() const noexcept;

 private:
  Type value_;
};

// Type aliases
using QueryValueU8 = QueryValue<uint8b>;
using QueryValueU16 = QueryValue<uint16b>;
using QueryValueU32 = QueryValue<uint32b>;
using QueryValueU64 = QueryValue<uint64b>;

//! Check if two values are equal
template <UnsignedInteger T>
bool operator==(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

//! Check if two values aren't equal
template <UnsignedInteger T>
bool operator!=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

//!
template <UnsignedInteger T>
bool operator<(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

//!
template <UnsignedInteger T>
bool operator<=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

//!
template <UnsignedInteger T>
bool operator>(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

//!
template <UnsignedInteger T>
bool operator>=(const QueryValue<T>& lhs, const QueryValue<T>& rhs) noexcept;

} /* namespace zisc */

#include "query_value-inl.hpp"

#endif /* ZISC_QUERY_VALUE_HPP */

