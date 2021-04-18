/*!
  \file data_storage-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_DATA_STORAGE_INL_HPP
#define ZISC_DATA_STORAGE_INL_HPP

#include "data_storage.hpp"
// Standard C++ library
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/utility.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename T> inline
DataStorage<T>::DataStorage() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename T> inline
DataStorage<T>::DataStorage(DataStorage&& other) noexcept
{
  store(std::move(other.get()));
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename T> inline
auto DataStorage<T>::operator=(DataStorage&& other) noexcept -> DataStorage&
{
  store(std::move(other.get()));
  return *this;
}

/*!
  \details No detailed description
  */
template <typename T> inline
void DataStorage<T>::destroy() noexcept
{
  if constexpr (std::is_same_v<Type, StorageT>) {
    storage_ = StorageT{};
  }
  else if constexpr (std::is_destructible_v<Type>) {
    std::destroy_at(ptr());
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto DataStorage<T>::get() noexcept -> Reference
{
  if constexpr (std::is_same_v<Type, StorageT>)
    return storage_;
  else
    return *reinterp<Pointer>(ptr());
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto DataStorage<T>::get() const noexcept -> ConstReference
{
  if constexpr (std::is_same_v<Type, StorageT>)
    return storage_;
  else
    return *reinterp<ConstPointer>(ptr());
}

/*!
  \details No detailed description

  \tparam DataT No description.
  \param [in] data No description.
  */
template <typename T> template <typename DataT> inline
void DataStorage<T>::store(DataT&& data) noexcept
    requires (std::is_constructible_v<T, DataT> || std::is_assignable_v<T, DataT>)
{
  if constexpr (std::is_same_v<Type, StorageT>) {
    if constexpr (std::is_assignable_v<T, DataT>) {
      storage_ = std::forward<DataT>(data);
    }
    else { // is_constructible
      auto p = ptr();
      std::destroy_at(p);
      new (p) Type{std::forward<DataT>(data)};
    }
  }
  else {
    if constexpr (std::is_assignable_v<T, DataT>) {
      new (ptr()) Type{std::forward<DataT>(data)};
    }
    static_assert(!std::is_constructible_v<T, DataT>,
                  "The data isn't able to be constructible.");
  }
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto DataStorage<T>::ptr() noexcept -> Pointer
{
  if constexpr (std::is_same_v<Type, StorageT>)
    return std::addressof(storage_);
  else
    return *reinterp<Pointer>(&storage_);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename T> inline
auto DataStorage<T>::ptr() const noexcept -> ConstPointer
{
  if constexpr (std::is_same_v<Type, StorageT>)
    return std::addressof(storage_);
  else
    return *reinterp<ConstPointer>(&storage_);
}

} // namespace zisc

#endif // ZISC_DATA_STORAGE_INL_HPP
