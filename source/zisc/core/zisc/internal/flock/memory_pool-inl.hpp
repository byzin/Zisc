/*!
  \file memory_pool-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_MEMORY_POOL_INL_HPP
#define ZISC_FLOCK_MEMORY_POOL_INL_HPP

#include "memory_pool.hpp"
// Standard C++ library
#include <concepts>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
// Zisc
#include "definitions.hpp"
#include "log.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

/*!
  \details No detailed description

  \tparam Type No description.
  \tparam PoolType No description.
  \param [in] epoch No description.
  \param [in] log_list No description.
  \param [in,out] mem_resource No description.
  */
template <typename Type, typename PoolType> inline
MemoryPool<Type, PoolType>::MemoryPool(Epoch* epoch,
                                       std::span<Log> log_list,
                                       pmr::memory_resource* mem_resource) noexcept :
    pool_impl_{epoch, mem_resource},
    log_list_{log_list}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, typename PoolType> inline
MemoryPool<Type, PoolType>::MemoryPool(MemoryPool&& other) noexcept :
    pool_impl_{std::move(other.pool_impl_)},
    log_list_{std::move(other.log_list_)}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Type, typename PoolType> inline
auto MemoryPool<Type, PoolType>::operator=(MemoryPool&& other) noexcept -> MemoryPool&
{
  pool_impl_ = std::move(other.pool_impl_);
  log_list_ = std::move(other.log_list_);
  return *this;
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::acquire(Pointer p) noexcept
{
  pool_impl_.acquire(p);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
std::size_t MemoryPool<Type, PoolType>::capacity() const noexcept
{
  return pool_impl_.capacity();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
constexpr std::size_t MemoryPool<Type, PoolType>::capacityMax() noexcept
{
  return PoolT::capacityMax();
}

/*!
  \details No detailed description
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::clear() noexcept
{
  pool_impl_.clear();
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
bool MemoryPool<Type, PoolType>::doneValue(Pointer p) noexcept
{
  return extractBool(p);
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */
template <typename Type, typename PoolType> template <typename RT> inline
std::optional<RT> MemoryPool<Type, PoolType>::doneValueResult(Pointer p) noexcept
{
  const std::optional<std::size_t> r = extractResult(p);
  using OptionalT = std::optional<RT>;
  const OptionalT result = r.has_value()
      ? static_cast<OptionalT>(r)
      : OptionalT{};
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
Epoch& MemoryPool<Type, PoolType>::epoch() noexcept
{
  return pool_impl_.epoch();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
const Epoch& MemoryPool<Type, PoolType>::epoch() const noexcept
{
  return pool_impl_.epoch();
}

/*!
  \details No detailed description

  \param [in] object No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
std::size_t MemoryPool<Type, PoolType>::getIndex(ConstReference object) const noexcept
{
  return pool_impl_.getIndex(object);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
auto MemoryPool<Type, PoolType>::getObject(const std::size_t index) noexcept
    -> Reference
{
  return pool_impl_.getObject(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
auto MemoryPool<Type, PoolType>::getObject(const std::size_t index) const noexcept
    -> ConstReference
{
  return pool_impl_.getObject(index);
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
bool MemoryPool<Type, PoolType>::isDone(Pointer p) noexcept
{
  return isDoneFlag(p);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
std::span<Log> MemoryPool<Type, PoolType>::logList() noexcept
{
  return log_list_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
std::span<const Log> MemoryPool<Type, PoolType>::logList() const noexcept
{
  return log_list_;
}

/*!
  \details No detailed description

  \tparam Func No description.
  \tparam Args No description.
  \param [in] func A function that initializes a new object before it is shared
  \param [in] args No description.
  \return No description
  */
template <typename Type, typename PoolType>
template <std::invocable<Type*> Func, typename ...Args> inline
auto MemoryPool<Type, PoolType>::newInit(Func&& func, Args&&... args) noexcept
    -> Pointer
{
  Log& log = workerInfo().takeOut(logList());
  // Run f with no logging (i.e. an empty log)
  auto f = [this, &func, ...args = std::forward<Args>(args)]() noexcept
  {
    Pointer x = pool_impl_.newObj(std::forward<Args>(args)...);
    if (x != nullptr) [[likely]]
      func(x);
    return x;
  };
  Pointer new_value = log.doWith(nullptr, 0, std::move(f));
  std::pair<Pointer, bool> result = log.commitValue(new_value);
  if ((new_value != nullptr) && !result.second)
    pool_impl_.destruct(new_value);
  return result.first;
}

/*!
  \details No detailed description

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename Type, typename PoolType> template <typename ...Args> inline
auto MemoryPool<Type, PoolType>::newObj(Args&&... args) noexcept -> Pointer
{
  auto [ptr, flag] = newObjFl(std::forward<Args>(args)...);
  return ptr;
}

/*!
  \details Idempotent allocation
  Returns a pointer to the new object and a possible pointer
  to a location in the log containing the pointer.
  The location is null if this was not the first among thunks
  to allocate the object.
  The returned pointer can be one of done_true or done_false
  The returned pointer can be one of done_true or done_false

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename Type, typename PoolType> template <typename ...Args> inline
auto MemoryPool<Type, PoolType>::newObjAcquired(Args&&... args) noexcept
    -> std::pair<Pointer, LogEntry*>
{
  auto [ptr, flag] = newObjFl(std::forward<Args>(args)...);
  Log& log = workerInfo().takeOut(logList());

  using ResultT = std::pair<Pointer, LogEntry*>;
  ResultT result{ptr, nullptr};
  if (!log.isEmpty()) {
    LogEntry* l = log.currentEntry();
    if (!flag && !isDone(ptr)) {
      pool_impl_.acquire(ptr);
      auto* p = Log::cast<Pointer>(l->load(std::memory_order::acquire));
      result = ResultT{p, nullptr};
    }
    else {
      result = ResultT{ptr, flag ? l : nullptr};
    }
  }
  return result;
}

/*!
  \details this version also returns a flag to specify whether actually allocated
  vs., having been allocated by another instance of a thunk

  \tparam Args No description.
  \param [in] args No description.
  \return No description
  */
template <typename Type, typename PoolType> template <typename ...Args> inline
auto MemoryPool<Type, PoolType>::newObjFl(Args&&... args) noexcept
    -> std::pair<Pointer, bool>
{
  Log& log = workerInfo().takeOut(logList());
  // TODO: helpers might do lots of allocates and frees,
  // can potentially optimize by checking if a log value has already been
  // committed.
  auto func = [this, &args...]() noexcept
  {
    return pool_impl_.newObj(std::forward<Args>(args)...);
  };
  Pointer new_value = log.doWith(nullptr, 0, std::move(func));
  std::pair<Pointer, bool> result = log.commitValue(new_value);
  // If already allocated return back to pool
  if ((new_value != nullptr) && !result.second)
    pool_impl_.destruct(new_value);
  return result;
}

/*!
  \details No detailed description

  \param [in] size No description.
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::reserve(const std::size_t size) noexcept
{
  pool_impl_.reserve(size);
}

/*!
  \details No detailed description

  \param [in] p No description.
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::retire(Pointer p) noexcept
{
  Log& log = workerInfo().takeOut(logList());
  std::pair<Pointer, bool> x = log.commitValueSafe(p);
  if (x.second)
    pool_impl_.retire(p); // only retire if first try
}

/*!
  \details No detailed description

  \param [in] p No description.
  \param [in,out] le No description.
  \param [in] result No description.
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::retireAcquired(Pointer p,
                                                LogEntry* le,
                                                const bool result) noexcept
{
  const Log& log = workerInfo().takeOut(logList());
  if (log.isEmpty()) {
    pool_impl_.retire(p);
  }
  else if (le != nullptr) {
    le->store(tagBool(result), std::memory_order::release);
    pool_impl_.retire(p);
  }
}

/*!
  \details No detailed description

  \tparam TT No description.
  \param [in] p No description.
  \param [in,out] le No description.
  \param [in] result No description.
  */
template <typename Type, typename PoolType> template <typename TT> inline
void MemoryPool<Type, PoolType>::retireAcquiredResult(Pointer p,
                                                      LogEntry* le,
                                                      const std::optional<TT> result) noexcept
{
  const Log& log = workerInfo().takeOut(logList());
  if (log.isEmpty()) {
    pool_impl_.retire(p);
  }
  else if (le != nullptr) {
    le->store(tagResult(result), std::memory_order::release);
    pool_impl_.retire(p);
  }
}

/*!
  \details No detailed description

  \param [in] n No description.
  */
template <typename Type, typename PoolType> inline
void MemoryPool<Type, PoolType>::shuffle(const std::size_t n) noexcept
{
  pool_impl_.shuffle(n);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
std::size_t MemoryPool<Type, PoolType>::size() const noexcept
{
  return pool_impl_.size();
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Type, typename PoolType> inline
const WorkerInfo& MemoryPool<Type, PoolType>::workerInfo() const noexcept
{
  return pool_impl_.workerInfo();
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
bool MemoryPool<Type, PoolType>::extractBool(Pointer p) noexcept
{
  const bool result = (Log::cast<std::size_t>(p) >> 48) == 1ull;
  return result;
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
std::optional<std::size_t> MemoryPool<Type, PoolType>::extractResult(Pointer p) noexcept
{
  using ResultT = std::optional<std::size_t>;
  const ResultT result = extractBool(p)
      ? ResultT{Log::cast<std::size_t>(p) & Log::cast<std::size_t>((1ull << 48) - 1)}
      : ResultT{};
  return result;
}

/*!
  \details No detailed description

  \param [in] p No description.
  \return No description
  */

template <typename Type, typename PoolType> inline
bool MemoryPool<Type, PoolType>::isDoneFlag(Pointer p) noexcept
{
  const bool result = 0 < (Log::cast<std::size_t>(p) >> 48);
  return result;
}

/*!
  \details No detailed description

  \param [in] result No description.
  \return No description
  */
template <typename Type, typename PoolType> inline
void* MemoryPool<Type, PoolType>::tagBool(const bool result) noexcept
{
  constexpr std::size_t p1 = 1ull << 48;
  constexpr std::size_t p2 = 2ull << 48;
  auto* p = Log::cast<void*>(result ? p1 : p2);
  return p;
}

/*!
  \details No detailed description

  \tparam TT No description.
  \param [in] result No description.
  \return No description
  */
template <typename Type, typename PoolType> template <typename TT> inline
void* MemoryPool<Type, PoolType>::tagResult(const std::optional<TT> result) noexcept
{
  constexpr std::size_t p1 = 1ull << 48;
  constexpr std::size_t p2 = 2ull << 48;
  const std::size_t p = (!result.has_value()) ? p2 : (p1 | cast<std::size_t>(**result));
  return Log::cast<void*>(p);
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_MEMORY_POOL_INL_HPP */
