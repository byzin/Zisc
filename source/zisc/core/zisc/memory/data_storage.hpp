/*!
  \file data_storage.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
template <std::movable T>
class DataStorage : private NonCopyable<DataStorage<T>>
{
 public:
  // Type aliases
  using ValueT = std::remove_volatile_t<T>;
  using ConstT = std::add_const_t<ValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstT>;


  //! Create a storage without initialization
  DataStorage() noexcept;

  //! Move a data
  DataStorage(DataStorage&& other) noexcept;

  //! Create a storage and store a value initialized by the given args
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  DataStorage(Args&&... args) noexcept;


  //! Move a data
  DataStorage& operator=(DataStorage&& other) noexcept;

  //! Return the reference to the underlying data
  Reference operator*() noexcept;

  //! Return the reference to the underlying data
  ConstReference operator*() const noexcept;

  //! Destroy the value stored in the storage
  void destroy() noexcept;

  //! Return the reference to the underlying data
  Reference get() noexcept;

  //! Return the reference to the underlying data
  ConstReference get() const noexcept;

  //! Return the pointer to the underlying data
  Pointer memory() noexcept;

  //! Return the pointer to the underlying data
  ConstPointer memory() const noexcept;

  //! Create a value initialized with the given value and store it into the storage
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  Pointer set(Args&&... args) noexcept;

 private:
  //! Check if T is able to be used as storage directly. Otherwise use aligned_storage
  static constexpr bool kTCanBeStorageT =
      std::is_nothrow_default_constructible_v<ValueT> &&
      std::is_nothrow_destructible_v<ValueT>;


  // Type aliases
  using StorageT = std::conditional_t<kTCanBeStorageT,
      ValueT,
      std::aligned_storage_t<sizeof(ValueT), std::alignment_of_v<ValueT>>>;


  StorageT storage_;
};

} // namespace zisc

#include "data_storage-inl.hpp"

#endif // ZISC_DATA_STORAGE_HPP
