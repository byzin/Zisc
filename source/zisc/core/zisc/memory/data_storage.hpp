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
template <typename T>
class DataStorage : private NonCopyable<DataStorage<T>>
{
 public:
  // Type aliases
  using Type = std::remove_volatile_t<T>;
  using ConstType = std::add_const_t<Type>;
  using Pointer = std::add_pointer_t<Type>;
  using ConstPointer = std::add_pointer_t<ConstType>;
  using Reference = std::add_lvalue_reference<Type>;
  using ConstReference = std::add_lvalue_reference<ConstType>;


  //! Check if T is able to be used as storage directly. Otherwise use aligned_storage
  static constexpr bool canBeUsedAsStorage = std::is_default_constructible_v<Type> &&
                                             std::is_destructible_v<Type>;

  //! Create a empty data
  DataStorage() noexcept;

  //! Move a data
  DataStorage(DataStorage&& other) noexcept;


  //! Move a data
  DataStorage& operator=(DataStorage&& other) noexcept;


  //! Destroyt the storage
  void destroy() noexcept;

  //! Return the reference to the underlying data
  Reference get() noexcept;

  //! Return the reference to the underlying data
  ConstReference get() const noexcept;

  //! Store the given value into the storage
  template <typename DataT>
  void store(DataT&& data) noexcept
      requires (std::is_constructible_v<T, DataT> || std::is_assignable_v<T, DataT>);

 private:
  // Type aliases
  using StorageT = std::conditional_t<canBeUsedAsStorage,
      Type,
      std::aligned_storage_t<sizeof(Type), std::alignment_of_v<Type>>>;


  //! Return the pointer to the storage
  Pointer ptr() noexcept;

  //! Return the pointer to the storage
  ConstPointer ptr() const noexcept;


  StorageT storage_;
};

} // namespace zisc

#include "data_storage-inl.hpp"

#endif // ZISC_DATA_STORAGE_HPP
