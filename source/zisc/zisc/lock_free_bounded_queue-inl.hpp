/*!
  \file lock_free_bounded_queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP
#define ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP

#include "lock_free_bounded_queue.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>
// Zisc
#include "error.hpp"
#include "std_memory_resource.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in] value No description.
  */
template <typename QueueClass, typename T> inline
LockFreeBoundedQueue<QueueClass, T>::OverflowError::OverflowError(
    const std::string_view what_arg,
    pmr::memory_resource* mem_resource,
    ConstReference value) :
        SystemError(ErrorCode::kLockFreeBoundedQueueOverflow, what_arg),
        value_{std::allocate_shared<Type>(
            pmr::polymorphic_allocator<Type>{mem_resource},
            value)}
{
}

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in] value No description.
  */
template <typename QueueClass, typename T> inline
LockFreeBoundedQueue<QueueClass, T>::OverflowError::OverflowError(
    const std::string_view what_arg,
    pmr::memory_resource* mem_resource,
    RReference value) :
        SystemError(ErrorCode::kLockFreeBoundedQueueOverflow, what_arg),
        value_{std::allocate_shared<Type>(
            pmr::polymorphic_allocator<Type>{mem_resource},
            std::move(value))}
{
}

/*!
  \details No detailed description
  */
template <typename QueueClass, typename T> inline
LockFreeBoundedQueue<QueueClass, T>::OverflowError::~OverflowError()
{
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::OverflowError::get() noexcept
    -> Reference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::OverflowError::get() const noexcept
    -> ConstReference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::capacity() const noexcept -> size_type
{
  const auto q = cast<ConstQueuePtr>(this);
  const size_type cap = q->capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
constexpr auto LockFreeBoundedQueue<QueueClass, T>::capacityMax() noexcept -> size_type
{
  const size_type cap = QueueClass::capacityMax();
  return cap;
}

/*!
  \details No detailed description
  */
template <typename QueueClass, typename T> inline
void LockFreeBoundedQueue<QueueClass, T>::clear() noexcept
{
  auto q = cast<QueuePtr>(this);
  q->clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::dequeue() noexcept -> std::tuple<bool, Type>
{
  auto q = cast<QueuePtr>(this);
  const auto result = q->dequeue();
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename QueueClass, typename T> inline
bool LockFreeBoundedQueue<QueueClass, T>::enqueue(ConstReference value)
{
  auto q = cast<QueuePtr>(this);
  const bool result = q->enqueue(value);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename QueueClass, typename T> inline
bool LockFreeBoundedQueue<QueueClass, T>::enqueue(RReference value)
{
  auto q = cast<QueuePtr>(this);
  const bool result = q->enqueue(std::move(value));
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
bool LockFreeBoundedQueue<QueueClass, T>::isEmpty() const noexcept
{
  const auto q = cast<ConstQueuePtr>(this);
  const bool result = q->isEmpty();
  return result;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename QueueClass, typename T> inline
void LockFreeBoundedQueue<QueueClass, T>::setCapacity(const size_type cap) noexcept
{
  auto q = cast<QueuePtr>(this);
  q->setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::size() const noexcept -> size_type
{
  const auto q = cast<ConstQueuePtr>(this);
  const size_type s = q->size();
  return s;
}

/*!
  \details No detailed description
  */
template <typename QueueClass, typename T> inline
LockFreeBoundedQueue<QueueClass, T>::LockFreeBoundedQueue() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename QueueClass, typename T> inline
LockFreeBoundedQueue<QueueClass, T>::LockFreeBoundedQueue(
    const LockFreeBoundedQueue& other) noexcept
{
  static_cast<void>(other);
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename QueueClass, typename T> inline
auto LockFreeBoundedQueue<QueueClass, T>::operator=(
    const LockFreeBoundedQueue& other) noexcept -> LockFreeBoundedQueue&
{
  static_cast<void>(other);
  return *this;
}

} // namespace zisc

#endif // ZISC_LOCK_FREE_BOUNDED_QUEUE_INL_HPP
