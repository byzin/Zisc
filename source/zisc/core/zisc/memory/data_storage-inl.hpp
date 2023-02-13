/*!
  \file data_storage-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_DATA_STORAGE_INL_HPP
#define ZISC_DATA_STORAGE_INL_HPP

#include "data_storage.hpp"
// Standard C++ library
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/utility.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <std::move_constructible T> inline
DataStorage<T>::DataStorage() noexcept
{
  static_assert(sizeof(StorageT) == sizeof(T), "The storage size isn't correct.");
  static_assert(alignof(StorageT) == alignof(T), "The storage alignment isn't correct.");
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <std::move_constructible T> inline
DataStorage<T>::DataStorage(DataStorage&& other) noexcept
{
  set(std::move(other.get()));
  other.destroy();
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  */
template <std::move_constructible T>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
DataStorage<T>::DataStorage(Args&&... args) noexcept
{
  set(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::operator=(DataStorage&& other) noexcept -> DataStorage&
{
  set(std::move(other.get()));
  other.destroy();
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::operator*() noexcept -> Reference
{
  return get();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::operator*() const noexcept -> ConstReference
{
  return get();
}

/*!
  \details No detailed description
  */
template <std::move_constructible T> inline
void DataStorage<T>::destroy() noexcept
{
  if constexpr (kTCanBeStorageT)
    storage_ = StorageT{};
  else if constexpr (std::is_nothrow_destructible_v<ValueT>)
    std::destroy_at(memory());
  else
    static_assert(std::is_nothrow_destructible_v<ValueT>,
                  "Destructor with exception isn't supported yet.");
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::get() noexcept -> Reference
{
  if constexpr (kTCanBeStorageT)
    return storage_;
  else
    return *memory();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::get() const noexcept -> ConstReference
{
  if constexpr (kTCanBeStorageT)
    return storage_;
  else
    return *memory();
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::memory() noexcept -> Pointer
{
  StorageT* ptr = std::addressof(storage_);
  if constexpr (kTCanBeStorageT)
    return ptr;
  else
    return reinterp<Pointer>(ptr);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::move_constructible T> inline
auto DataStorage<T>::memory() const noexcept -> ConstPointer
{
  const StorageT* ptr = std::addressof(storage_);
  if constexpr (kTCanBeStorageT)
    return ptr;
  else
    return reinterp<ConstPointer>(ptr);
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  */
template <std::move_constructible T>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
auto DataStorage<T>::set(Args&&... args) noexcept -> Pointer
{
  Pointer ptr = nullptr;
  if constexpr (kTCanBeStorageT) {
    storage_ = ValueT{std::forward<Args>(args)...};
    ptr = std::addressof(storage_);
  }
  else {
    ptr = ::new (memory()) ValueT{std::forward<Args>(args)...};
  }
  return ptr;
}

} // namespace zisc

#endif // ZISC_DATA_STORAGE_INL_HPP
