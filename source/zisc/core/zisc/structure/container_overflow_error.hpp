/*!
  \file container_overflow_error.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONTAINER_OVERFLOW_ERROR_HPP
#define ZISC_CONTAINER_OVERFLOW_ERROR_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <memory_resource>
#include <string_view>
#include <type_traits>
// Zisc
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T>
class ContainerOverflowError : public SystemError
{
 public:
  // Type aliases
  using ValueT = std::remove_volatile_t<T>;
  using ConstT = std::add_const_t<ValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_const_t<std::add_lvalue_reference_t<ConstT>>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_const_t<std::add_pointer_t<ConstT>>;



  //! Construct the lock free queue error
  ContainerOverflowError(const std::string_view what_arg,
                         std::pmr::memory_resource* mem_resource,
                         ConstReference value);

  //! Construct the lock free queue error
  ContainerOverflowError(const std::string_view what_arg,
                         std::pmr::memory_resource* mem_resource,
                         RReference value);

  //! Move data
  ContainerOverflowError(ContainerOverflowError&& other) noexcept;

  //! Finalize the lock free queue error
  ~ContainerOverflowError() noexcept override = default;


  //! Move data
  auto operator=(ContainerOverflowError&& other) noexcept -> ContainerOverflowError&;


  //! Return the overflowing value
  [[nodiscard]]
  auto get() noexcept -> Reference;

  //! Return the overflowing value
  [[nodiscard]]
  auto get() const noexcept -> ConstReference;

 private:
  std::shared_ptr<ValueT> value_;
};

} // namespace zisc

#include "container_overflow_error-inl.hpp"

#endif // ZISC_CONTAINER_OVERFLOW_ERROR_HPP
