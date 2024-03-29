/*!
  \file queue-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_QUEUE_INL_HPP
#define ZISC_QUEUE_INL_HPP

#include "queue.hpp"
// Standard C++ library
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::capacity() const noexcept -> size_type
{
  ConstQueueReference q = ref();
  return q.capacity();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
constexpr auto Queue<QueueClass, T>::capacityMax() noexcept -> size_type
{
  return QueueT::capacityMax();
}

/*!
  \details No detailed description
  */
template <typename QueueClass, std::movable T> inline
void Queue<QueueClass, T>::clear() noexcept
{
  QueueReference q = ref();
  q.clear();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::dequeue() noexcept -> std::optional<ValueT>
{
  QueueReference q = ref();
  return q.dequeue();
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  \exception OverflowError No description.
  */
template <typename QueueClass, std::movable T>
template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...> inline
auto Queue<QueueClass, T>::enqueue(Args&&... args) -> std::optional<size_type>
{
  QueueReference q = ref();
  return q.enqueue(std::forward<Args>(args)...);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::get(const size_type index) noexcept -> Reference
{
  QueueReference q = ref();
  return q.get(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::get(const size_type index) const noexcept -> ConstReference
{
  ConstQueueReference q = ref();
  return q.get(index);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
constexpr auto Queue<QueueClass, T>::isBounded() noexcept -> bool
{
  return QueueT::isBounded();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
constexpr auto Queue<QueueClass, T>::isConcurrent() noexcept -> bool
{
  return QueueT::isConcurrent();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::isEmpty() const noexcept -> bool
{
  const bool result = size() == 0;
  return result;
}

/*!
  \details No detailed description

  \param [in] cap No description.
  */
template <typename QueueClass, std::movable T> inline
void Queue<QueueClass, T>::setCapacity(const size_type cap)
{
  QueueReference q = ref();
  q.setCapacity(cap);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::size() const noexcept -> size_type
{
  ConstQueueReference q = ref();
  return q.size();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename QueueClass, std::movable T> inline
Queue<QueueClass, T>::Queue([[maybe_unused]] const Queue& other) noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::operator=([[maybe_unused]] const Queue& other) noexcept
    -> Queue&
{
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::ref() noexcept -> QueueReference
{
  auto* q = static_cast<QueuePtr>(this);
  return *q;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename QueueClass, std::movable T> inline
auto Queue<QueueClass, T>::ref() const noexcept -> ConstQueueReference
{
  const auto* q = static_cast<ConstQueuePtr>(this);
  return *q;
}

} // namespace zisc

#endif // ZISC_QUEUE_INL_HPP
