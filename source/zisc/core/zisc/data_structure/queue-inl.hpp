/*!
  \file queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUEUE_INL_HPP
#define ZISC_QUEUE_INL_HPP

#include "queue.hpp"
// Standard C++ library
#include <cstddef>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] what_arg No description.
  \param [in] value No description.
  */
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::OverflowError::OverflowError(const std::string_view what_arg,
                                                   pmr::memory_resource* mem_resource,
                                                   ConstReference value)
    : SystemError(ErrorCode::kBoundedQueueOverflow, what_arg),
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
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::OverflowError::OverflowError(const std::string_view what_arg,
                                                   pmr::memory_resource* mem_resource,
                                                   RReference value)
    : SystemError(ErrorCode::kBoundedQueueOverflow, what_arg),
      value_{std::allocate_shared<Type>(
          pmr::polymorphic_allocator<Type>{mem_resource},
          std::move(value))}
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::OverflowError::OverflowError(OverflowError&& other) :
    SystemError(std::move(other)),
    value_{std::move(other.value_)}
{
}

/*!
  \details No detailed description
  */
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::OverflowError::~OverflowError() noexcept
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::OverflowError::operator=(OverflowError&& other)
    -> OverflowError&
{
  SystemError::operator=(std::move(other));
  value_ = std::move(other.value_);
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::OverflowError::get() noexcept
    -> Reference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::OverflowError::get() const noexcept
    -> ConstReference
{
  return *value_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::capacity() const noexcept -> size_type
{
  const auto& q = ref();
  const size_type cap = q.capacity();
  return cap;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
constexpr auto Queue<QueueClass, T>::capacityMax() noexcept -> size_type
{
  const size_type cap = QueueT::capacityMax();
  return cap;
}

/*!
  \details No detailed description
  */
template <typename QueueClass, Movable T> inline
void Queue<QueueClass, T>::clear() noexcept
{
  auto& q = ref();
  q.clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::dequeue() noexcept -> std::tuple<bool, Type>
{
  auto& q = ref();
  return q.dequeue();
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename QueueClass, Movable T> inline
bool Queue<QueueClass, T>::enqueue(ConstReference value)
{
  auto& q = ref();
  const bool result = q.enqueue(value);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename QueueClass, Movable T> inline
bool Queue<QueueClass, T>::enqueue(RReference value)
{
  auto& q = ref();
  const bool result = q.enqueue(std::move(value));
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
constexpr bool Queue<QueueClass, T>::isBounded() noexcept
{
  const size_type c = capacityMax();
  const bool result = c < kUnboundedCapacityMax;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
constexpr bool Queue<QueueClass, T>::isConcurrent() noexcept
{
  const bool result = QueueT::isConcurrent();
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
bool Queue<QueueClass, T>::isEmpty() const noexcept
{
  const auto& q = ref();
  const bool result = q.isEmpty();
  return result;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename QueueClass, Movable T> inline
void Queue<QueueClass, T>::setCapacity(const size_type cap) noexcept
{
  auto& q = ref();
  q.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::size() const noexcept -> size_type
{
  const auto& q = ref();
  const size_type s = q.size();
  return s;
}

/*!
  \details No detailed description
  */
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::Queue() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename QueueClass, Movable T> inline
Queue<QueueClass, T>::Queue([[maybe_unused]] const Queue& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::operator=([[maybe_unused]] const Queue& other) noexcept
    -> Queue&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::ref() noexcept -> QueueReference
{
  auto q = cast<QueuePtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, Movable T> inline
auto Queue<QueueClass, T>::ref() const noexcept -> ConstQueueReference
{
  auto q = cast<ConstQueuePtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_QUEUE_INL_HPP
