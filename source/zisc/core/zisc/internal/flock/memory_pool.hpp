/*!
  \file memory_pool.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_MEMORY_POOL_HPP
#define ZISC_FLOCK_MEMORY_POOL_HPP

// Standard C++ library
#include <concepts>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
// Zisc
#include "definitions.hpp"
#include "epoch_pool_impl.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class Epoch;
class Log;
class WorkerInfo;

/*!
  \brief memory pool using epoch based collection and safe (idempotent) allocation and retire in a lock

  No detailed description.

  \tparam Type No description.
  \tparam PoolType No description.
  */
template <typename Type, typename PoolType = EpochPoolImpl<Type>>
class MemoryPool : private NonCopyable<MemoryPool<Type, PoolType>>
{
 public:
  // Type aliases
  using ValueT = Type;
  using ConstT = std::add_const_t<ValueT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstT>;
  using PoolT = PoolType;


  //! Create a memory pool
  MemoryPool(Epoch* epoch,
             std::span<Log> log_list,
             pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  MemoryPool(MemoryPool&& other) noexcept;

  //! Destroy the memory pool
  ~MemoryPool() noexcept = default;


  //! Move a data
  MemoryPool& operator=(MemoryPool&& other) noexcept;


  //!
  void acquire(Pointer p) noexcept;

  //! Return the capacity of objects in the pool
  std::size_t capacity() const noexcept;

  //! Return the maximum possible capacity of objects in the pool
  static constexpr std::size_t capacityMax() noexcept;

  //!
  void clear() noexcept;

  //!
  bool doneValue(Pointer p) noexcept;

  //!
  template <typename RT>
  std::optional<RT> doneValueResult(Pointer p) noexcept;

  //! Return the underlying epoch
  Epoch& epoch() noexcept;

  //! Return the underlying epoch
  const Epoch& epoch() const noexcept;

  //! Return the index of the given object in the pool
  std::size_t getIndex(ConstReference object) const noexcept;

  //! Return the reference to an object by the index
  Reference getObject(const std::size_t index) noexcept;

  //! Return the reference to an object by the index
  ConstReference getObject(const std::size_t index) const noexcept;

  //!
  bool isDone(Pointer p) noexcept;

  //! Return the underlying log list
  std::span<Log> logList() noexcept;

  //! Return the underlying log list
  std::span<const Log> logList() const noexcept;

  //!
  template <std::invocable<Type*> Func, typename ...Args>
  Pointer newInit(Func&& func, Args&&... args) noexcept;

  //!
  template <typename ...Args>
  Pointer newObj(Args&&... args) noexcept;

  //!
  template <typename ...Args>
  std::pair<Pointer, LogEntry*> newObjAcquired(Args&&... args) noexcept;

  //!
  template <typename ...Args>
  std::pair<Pointer, bool> newObjFl(Args&&... args) noexcept;

  //!
  void reserve(const std::size_t size) noexcept;

  //!
  void retire(Pointer p) noexcept;

  //!
  void retireAcquired(Pointer p, LogEntry* le, const bool result) noexcept;

  //!
  template <typename TT>
  void retireAcquiredResult(Pointer p, LogEntry* le, const std::optional<TT> result) noexcept;

  //!
  void shuffle(const std::size_t n) noexcept;

  //! Return the number of allocated objects in the pool
  std::size_t size() const noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

 private:
  //!
  bool extractBool(Pointer p) noexcept;

  //!
  std::optional<std::size_t> extractResult(Pointer p) noexcept;

  //!
  bool isDoneFlag(Pointer p) noexcept;

  //!
  void* tagBool(const bool result) noexcept;

  //!
  template <typename TT>
  void* tagResult(std::optional<TT> result) noexcept;


  PoolT pool_impl_;
  std::span<Log> log_list_;
};

} /* namespace zisc::flock */

#include "memory_pool-inl.hpp"

#endif /* ZISC_FLOCK_MEMORY_POOL_HPP */
