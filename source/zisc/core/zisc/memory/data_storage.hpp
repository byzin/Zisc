/*!
  \file data_storage.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_DATA_STORAGE_HPP
#define ZISC_DATA_STORAGE_HPP

// Standard C++ library
#include <concepts>
#include <memory>
#include <type_traits>
// Zisc
#include "zisc/non_copyable.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::move_constructible T>
class DataStorage : private NonCopyable<DataStorage<T>>
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


  //! Create a storage without initialization
  DataStorage() noexcept;

  //! Move a data
  DataStorage(DataStorage&& other) noexcept;

  //! Create a storage and store a value initialized by the given args
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  explicit DataStorage(Args&&... args) noexcept;


  //! Move a data
  auto operator=(DataStorage&& other) noexcept -> DataStorage&;

  //! Return the reference to the underlying data
  auto operator*() noexcept -> Reference;

  //! Return the reference to the underlying data
  auto operator*() const noexcept -> ConstReference;

  //! Destroy the value stored in the storage
  void destroy() noexcept;

  //! Return the reference to the underlying data
  [[nodiscard]]
  auto get() noexcept -> Reference;

  //! Return the reference to the underlying data
  [[nodiscard]]
  auto get() const noexcept -> ConstReference;

  //! Return the pointer to the underlying data
  [[nodiscard]]
  auto memory() noexcept -> Pointer;

  //! Return the pointer to the underlying data
  [[nodiscard]]
  auto memory() const noexcept -> ConstPointer;

  //! Create a value initialized with the given value and store it into the storage
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  auto set(Args&&... args) noexcept -> Pointer;

 private:
  //! Check if T is able to be used as storage directly. Otherwise use aligned_storage
  static constexpr bool kTCanBeStorageT =
      std::is_nothrow_default_constructible_v<ValueT> &&
      std::is_nothrow_destructible_v<ValueT> &&
      std::is_nothrow_move_assignable_v<ValueT>;


  // Type aliases
  using StorageT = std::conditional_t<kTCanBeStorageT,
      ValueT,
      std::aligned_storage_t<sizeof(ValueT), alignof(ValueT)>>;


  StorageT storage_;
};

} // namespace zisc

#include "data_storage-inl.hpp"

#endif // ZISC_DATA_STORAGE_HPP
