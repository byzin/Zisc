/*!
  \file mutex_queue.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MUTEX_QUEUE_HPP
#define ZISC_MUTEX_QUEUE_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <span>
#include <type_traits>
#include <vector>
// Zisc
#include "queue.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/data_storage.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <std::movable T>
class MutexQueue : public Queue<MutexQueue<T>, T>
{
 public:
  // Type aliases
  using BaseQueueT = Queue<MutexQueue<T>, T>;
  using ValueT = typename BaseQueueT::ValueT;
  using ConstT = typename BaseQueueT::ConstT;
  using Reference = typename BaseQueueT::Reference;
  using RReference = typename BaseQueueT::RReference;
  using ConstReference = typename BaseQueueT::ConstReference;
  using Pointer = typename BaseQueueT::Pointer;
  using ConstPointer = typename BaseQueueT::ConstPointer;

  // Type aliases for STL
  using value_type = typename BaseQueueT::value_type;
  using size_type = typename BaseQueueT::size_type;
  using reference = typename BaseQueueT::reference;
  using const_reference = typename BaseQueueT::const_reference;


  //! Create a queue
  explicit MutexQueue(pmr::memory_resource* mem_resource) noexcept;

  //! Create a queue
  MutexQueue(const size_type cap, pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  MutexQueue(MutexQueue&& other) noexcept;

  //! Destroy the queue
  ~MutexQueue() noexcept;


  //! Move a data
  auto operator=(MutexQueue&& other) noexcept -> MutexQueue&;


  //! Return the maximum possible number of elements can be queued
  auto capacity() const noexcept -> size_type;

  //! Return the maximum possible capacity
  static constexpr auto capacityMax() noexcept -> size_type;

  //! Clear the contents
  void clear() noexcept;

  //! Return the direct access to the underlying array
  auto data() const noexcept -> std::span<ConstT>;

  //! Take the first element of the queue
  [[nodiscard]]
  auto dequeue() noexcept -> std::optional<ValueT>;

  //! Append the given element value to the end of the queue
  template <typename ...Args> requires std::is_nothrow_constructible_v<T, Args...>
  [[nodiscard]]
  auto enqueue(Args&&... args) -> std::optional<size_type>;

  //! Return the value by the given index
  auto get(const size_type index) noexcept -> Reference;

  //! Return the value by the given index
  auto get(const size_type index) const noexcept -> ConstReference;

  //! Check if the queue is bounded
  static constexpr auto isBounded() noexcept -> bool;

  //! Check if the queue is concurrent
  static constexpr auto isConcurrent() noexcept -> bool;

  //! Return the pointer to the underlying memory resource
  auto resource() const noexcept -> pmr::memory_resource*;

  //! Change the maximum possible number of elements. The queued data is cleared
  void setCapacity(size_type cap) noexcept;

  //! Return the number of elements
  auto size() const noexcept -> size_type;

 private:
  using StorageT = DataStorage<ValueT>;
  using ConstStorageT = std::add_const_t<StorageT>;
  using StorageRef = std::add_lvalue_reference_t<StorageT>;
  using ConstStorageRef = std::add_lvalue_reference_t<ConstStorageT>;
  using StoragePtr = std::add_pointer_t<StorageT>;
  using ConstStoragePtr = std::add_pointer_t<ConstStorageT>;


  //! Return the distance between the head and tail pointers
  auto distance() const noexcept -> size_type;

  //! Return the storage by the given index
  auto getStorage(const size_type index) noexcept -> StorageRef;

  //! Return the storage by the given index
  auto getStorage(const size_type index) const noexcept -> ConstStorageRef;

  //! Increment the given indicator and return next index in memory
  auto increment(size_type* current) const noexcept -> size_type;

  //! Return the invalid ID
  static constexpr auto invalidId() noexcept -> size_type;


  mutable std::shared_mutex mutex_;
  pmr::vector<StorageT> elements_;
  size_type head_;
  size_type tail_;
};

} /* namespace zisc */

#include "mutex_queue-inl.hpp"

#endif /* ZISC_MUTEX_QUEUE_HPP */
