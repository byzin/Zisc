/*!
  \file log_array-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOG_ARRAY_INL_HPP
#define ZISC_FLOCK_LOG_ARRAY_INL_HPP

#include "log_array.hpp"
// Standard C++ library
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <utility>
// Zisc
#include "epoch_pool_impl.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description
  */
inline
LogArray::LogArray() noexcept :
    next_{nullptr}
{
  initialize();
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
LogArray::LogArray(LogArray&& other) noexcept
{
  copy(other);
}

/*!
  \details No detailed description
  */
inline
LogArray::~LogArray() noexcept
{
  printf("LogArrayDestructor\n");
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
LogArray& LogArray::operator=(LogArray&& other) noexcept
{
  copy(other);
  return *this;
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto LogArray::operator[](const std::size_t index) noexcept -> EntryReference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto LogArray::operator[](const std::size_t index) const noexcept -> ConstEntryReference
{
  return get(index);
}

/*!
  \details No detailed description

  \param [in] pool No description.
  */
inline
void LogArray::destroy(MemoryPoolT* pool) noexcept
{
  LogArray* next_array = next().load(std::memory_order::acquire);
  while (next_array != nullptr) {
    LogArray* p = next_array;
    next_array = p->next_.load(std::memory_order::acquire);
    pool->destruct(p);
  }
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto LogArray::get(const std::size_t index) noexcept -> EntryReference
{
  return entries_[index];
}

/*!
  \details No detailed description

  \param [in] index No description.
  \return No description
  */
inline
auto LogArray::get(const std::size_t index) const noexcept -> ConstEntryReference
{
  return entries_[index];
}

/*!
  \details No detailed description

  \return No description
  */
inline
constexpr std::size_t LogArray::length() noexcept
{
  return kLogLength;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::atomic<LogArray*>& LogArray::next() noexcept
{
  return next_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
const std::atomic<LogArray*>& LogArray::next() const noexcept
{
  return next_;
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
void LogArray::copy(LogArray& other) noexcept
{
  for (std::size_t i = 0; i < entries_.size(); ++i) {
    EntryT value = other.entries_[i].load(std::memory_order::acquire);
    entries_[i].store(value, std::memory_order::release);
  }
  {
    LogArray* n = other.next_.load(std::memory_order::acquire);
    next_.store(n, std::memory_order::release);
  }
}

/*!
  \details No detailed description
  */
inline
void LogArray::initialize() noexcept
{
  std::for_each_n(entries_.begin(), length(), [](EntryReference e) noexcept
  {
    e.store(nullptr, std::memory_order::relaxed);
  });
  next_.store(nullptr, std::memory_order::relaxed);
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_LOG_ARRAY_INL_HPP */
