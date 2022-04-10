/*!
  \file query_result-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUERY_RESULT_INL_HPP
#define ZISC_QUERY_RESULT_INL_HPP

#include "query_result.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "query_value.hpp"
#include "zisc/bit.hpp"
#include "zisc/boolean.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <std::movable T> inline
QueryResult<T>::QueryResult() noexcept : value_{}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <std::movable T> inline
QueryResult<T>::QueryResult(RReference value) noexcept :
    value_{std::move(value)},
    is_valid_{true}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::movable T> inline
QueryResult<T>::QueryResult(QueryResult&& other) noexcept :
    value_{std::move(other.value_)},
    is_valid_{other.is_valid_}
{
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  */
template <std::movable T> inline
auto QueryResult<T>::operator=(RReference value) noexcept -> QueryResult&
{
  value_ = std::move(value);
  is_valid_ = true;
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::movable T> inline
auto QueryResult<T>::operator=(QueryResult&& other) noexcept -> QueryResult&
{
  value_ = std::move(other.value_);
  is_valid_ = other.is_valid_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
QueryResult<T>::operator Reference() noexcept
{
  return get();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
QueryResult<T>::operator ConstReference() const noexcept
{
  return get();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto QueryResult<T>::get() noexcept -> Reference
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto QueryResult<T>::get() const noexcept -> ConstReference
{
  return value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
bool QueryResult<T>::isSuccess() const noexcept
{
  return is_valid_;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr int QueryResult<T>::type() noexcept
{
  return 0;
}

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T> requires std::floating_point<T>
class QueryResult<T> : private NonCopyable<QueryResult<T>>
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
  using BitType = std::conditional_t<sizeof(Type) == 2, uint16b,
                  std::conditional_t<sizeof(Type) == 4, uint32b, uint64b>>;


  //! Create an invalid result
  QueryResult() noexcept : value_{invalidValue()}
  {
  }

  //! Create a result
  QueryResult(RReference value) noexcept : value_{std::move(value)}
  {
  }

  //! Move data
  QueryResult(QueryResult&& other) noexcept : value_{std::move(other.value_)}
  {
  }


  //! Move data
  QueryResult& operator=(RReference value) noexcept
  {
    value_ = std::move(value);
    return *this;
  }

  //! Move data
  QueryResult& operator=(QueryResult&& other) noexcept
  {
    value_ = std::move(other.value_);
    return *this;
  }

  //! Return the underlying value
  operator Reference() noexcept
  {
    return get();
  }

  //! Return the underlying value
  operator ConstReference() const noexcept
  {
    return get();
  }


  //! Return the underlying value
  Reference get() noexcept
  {
    return value_;
  }

  //! Return the underlying value
  ConstReference get() const noexcept
  {
    return value_;
  }

  //! Return the invalid value
  static constexpr Type invalidValue() noexcept
  {
    constexpr Type invalid = std::numeric_limits<Type>::quiet_NaN();
    return invalid;
  }

  //! Check if the underlying value is valid
  bool isSuccess() const noexcept
  {
    constexpr auto invalid = bit_cast<BitType>(invalidValue());
    const BitType v = bit_cast<BitType>(value_);
    const bool result = v != invalid;
    return result;
  }

  //! Return the result type. For debug
  static constexpr int type() noexcept
  {
    return 1;
  }

 private:
  Type value_;
};

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T> requires std::derived_from<T, QueryValueU8> ||
                                   std::derived_from<T, QueryValueU16> ||
                                   std::derived_from<T, QueryValueU32> ||
                                   std::derived_from<T, QueryValueU64> ||
                                   std::derived_from<T, QueryValue<std::size_t>>
class QueryResult<T> : private NonCopyable<QueryResult<T>>
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


  //! Create an invalid result
  QueryResult() noexcept : value_{}
  {
  }

  //! Create a result
  QueryResult(RReference value) noexcept : value_{std::move(value)}
  {
  }

  //! Move data
  QueryResult(QueryResult&& other) noexcept : value_{std::move(other.value_)}
  {
  }


  //! Move data
  QueryResult& operator=(RReference value) noexcept
  {
    value_ = std::move(value);
    return *this;
  }

  //! Move data
  QueryResult& operator=(QueryResult&& other) noexcept
  {
    value_ = std::move(other.value_);
    return *this;
  }

  //! Return the underlying value
  operator Reference() noexcept
  {
    return get();
  }

  //! Return the underlying value
  operator ConstReference() const noexcept
  {
    return get();
  }


  //! Return the underlying value
  Reference get() noexcept
  {
    return value_;
  }

  //! Return the underlying value
  ConstReference get() const noexcept
  {
    return value_;
  }

  //! Check if the underlying value is valid
  bool isSuccess() const noexcept
  {
    const bool result = get().isValid();
    return result;
  }

  //! Return the result type. For debug
  static constexpr int type() noexcept
  {
    return 2;
  }

 private:
  Type value_;
};

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T> requires (std::alignment_of_v<T> == 1)
class QueryResult<T> : private NonCopyable<QueryResult<T>>
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


  //! Create an invalid result
  QueryResult() noexcept : value_{}
  {
  }

  //! Create a result
  QueryResult(RReference value) noexcept : value_{std::move(value)}, is_valid_{true}
  {
  }

  //! Move data
  QueryResult(QueryResult&& other) noexcept : value_{std::move(other.value_)}, is_valid_{other.is_valid_}
  {
  }


  //! Move data
  QueryResult& operator=(RReference value) noexcept
  {
    value_ = std::move(value);
    is_valid_ = true;
    return *this;
  }

  //! Move data
  QueryResult& operator=(QueryResult&& other) noexcept
  {
    value_ = std::move(other.value_);
    is_valid_ = other.is_valid_;
    return *this;
  }

  //! Return the underlying value
  operator Reference() noexcept
  {
    return get();
  }

  //! Return the underlying value
  operator ConstReference() const noexcept
  {
    return get();
  }


  //! Return the underlying value
  Reference get() noexcept
  {
    return value_;
  }

  //! Return the underlying value
  ConstReference get() const noexcept
  {
    return value_;
  }

  //! Check if the underlying value is valid
  bool isSuccess() const noexcept
  {
    return is_valid_;
  }

  //! Return the result type. For debug
  static constexpr int type() noexcept
  {
    return 3;
  }

 private:
  Type value_;
  Boolean is_valid_;
};

} /* namespace zisc */

#endif /* ZISC_QUERY_RESULT_INL_HPP */