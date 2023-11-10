/*!
  \file mutex_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_QUEUE_INL_HPP
#define ZISC_MUTEX_QUEUE_INL_HPP

#include "mutex_queue.hpp"
// Standard C++ library
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <memory_resource>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "container_overflow_error.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in,out] mem_resource No description.
  */
template <std::movable T> inline
MutexQueue<T>::MutexQueue(std::pmr::memory_resource* mem_resource) noexcept :
    MutexQueue(1, mem_resource)
{
}

/*!
  \details No detailed description

  \param [in] cap No description.
  \param [in,out] mem_resource No description.
  */
template <std::movable T> inline
MutexQueue<T>::MutexQueue(const size_type cap,
                          std::pmr::memory_resource* mem_resource) noexcept :
    elements_{typename decltype(elements_)::allocator_type{mem_resource}},
    head_{0},
    tail_{0}
{
  setCapacity(cap);
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable T> inline
MutexQueue<T>::MutexQueue(MutexQueue&& other) noexcept :
    elements_{std::move(other.elements_)},
    head_{other.head_},
    tail_{other.tail_}
{
}

/*!
  \details No detailed description
  */
template <std::movable T> inline
MutexQueue<T>::~MutexQueue() noexcept
{
  clear();
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <std::movable T> inline
auto MutexQueue<T>::operator=(MutexQueue&& other) noexcept -> MutexQueue&
{
  clear();
  BaseQueueT::operator=(std::move(other));
  elements_ = std::move(other.elements_);
  head_ = other.head_;
  tail_ = other.tail_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::capacity() const noexcept -> size_type
{
  const size_type cap = elements_.size();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr auto MutexQueue<T>::capacityMax() noexcept -> size_type
{
  const size_type cap = (std::numeric_limits<size_type>::max)() >> 1;
  return cap;
}

/*!
  \details No detailed description
  */
template <std::movable T> inline
void MutexQueue<T>::clear() noexcept
{
  // Skip clear operation after moving data to other
  if (elements_.empty())
    return;

  for (size_type index = increment(&head_); head_ <= tail_; index = increment(&head_))
    getStorage(index).destroy();
  head_ = 0;
  tail_ = 0;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::data() const noexcept -> std::span<ConstT>
{
  std::span<ConstT> d{getStorage(0).memory(), elements_.size()};
  return d;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::dequeue() noexcept -> std::optional<ValueT>
{
  std::optional<ValueT> result{};
  {
    std::unique_lock lock{mutex_};
    if (head_ < tail_) {
      const size_type index = increment(&head_);
      StorageRef storage = getStorage(index);
      result = std::move(*storage);
      storage.destroy();
    }
  }
  return result;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  \exception OverflowError No description.
  */
template <std::movable T>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
auto MutexQueue<T>::enqueue(Args&&... args) -> std::optional<size_type>
{
  size_type index = invalidId();
  {
    std::unique_lock lock{mutex_};
    // Check overflow
    using OverflowErr = typename BaseQueueT::OverflowError;
    if (capacity() <= distance()) {
      const char* message = "Queue overflow happened.";
      throw OverflowErr{message, resource(), ValueT{std::forward<Args>(args)...}};
    }

    index = increment(&tail_);
    StorageRef storage = getStorage(index);
    storage.set(std::forward<Args>(args)...);
  }
  return std::make_optional(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::get(const size_type index) noexcept -> Reference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::get(const size_type index) const noexcept -> ConstReference
{
  return *getStorage(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr auto MutexQueue<T>::isBounded() noexcept -> bool
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr auto MutexQueue<T>::isConcurrent() noexcept -> bool
{
  return true;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::resource() const noexcept -> std::pmr::memory_resource*
{
  std::pmr::memory_resource* mem_resource = elements_.get_allocator().resource();
  return mem_resource;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <std::movable T> inline
void MutexQueue<T>::setCapacity(size_type cap) noexcept
{
  constexpr size_type lowest_size = 1;
  cap = (std::max)(lowest_size, cap);

  clear();
  elements_.resize(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::size() const noexcept -> size_type
{
  size_type s = 0;
  {
    std::shared_lock lock{mutex_};
    s = distance();
  }
  return s;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::distance() const noexcept -> size_type
{
  ZISC_ASSERT(head_ <= tail_, "The tail is behind the head.");
  const size_type d = tail_ - head_;
  return d;
}

/*!
  \details No detailed description

  \param [in] index No description
  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::getStorage(const size_type index) noexcept -> StorageRef
{
  return elements_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::getStorage(const size_type index) const noexcept -> ConstStorageRef
{
  return elements_[index];
}

/*!
  \details No detailed description

  \param [in] current No description.
  \return No description
  */
template <std::movable T> inline
auto MutexQueue<T>::increment(size_type* current) const noexcept -> size_type
{
  size_type index = (*current)++;
  index = index % capacity();
  return index;
}

/*!
  \details No detailed description

  \return No description
  */
template <std::movable T> inline
constexpr auto MutexQueue<T>::invalidId() noexcept -> size_type
{
  constexpr size_type invalid = (std::numeric_limits<size_type>::max)();
  return invalid;
}

} /* namespace zisc */

#endif /* ZISC_MUTEX_QUEUE_INL_HPP */
