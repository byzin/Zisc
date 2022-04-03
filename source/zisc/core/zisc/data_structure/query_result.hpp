/*!
  \file query_result.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUERY_RESULT_HPP
#define ZISC_QUERY_RESULT_HPP

// Standard C++ library
#include <concepts>
#include <memory>
#include <type_traits>
// Zisc
#include "zisc/boolean.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T>
class QueryResult : private NonCopyable<QueryResult<T>>
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
  QueryResult() noexcept;

  //! Create a result
  QueryResult(RReference value) noexcept;

  //! Move data
  QueryResult(QueryResult&& other) noexcept;


  //! Move data
  QueryResult& operator=(RReference value) noexcept;

  //! Move data
  QueryResult& operator=(QueryResult&& other) noexcept;

  //! Return the underlying value
  operator Reference() noexcept;

  //! Return the underlying value
  operator ConstReference() const noexcept;


  //! Return the underlying value
  Reference get() noexcept;

  //! Return the underlying value
  ConstReference get() const noexcept;

  //! Check if the underlying value is valid
  bool isSuccess() const noexcept;

  //! Return the result type. For debug
  static constexpr int type() noexcept;

 private:
  Type value_;
  Boolean is_valid_;
  [[maybe_unused]] Padding<std::alignment_of_v<Type> - 1> pad_;
};

} /* namespace zisc */

#include "query_result-inl.hpp"

#endif /* ZISC_QUERY_RESULT_HPP */
