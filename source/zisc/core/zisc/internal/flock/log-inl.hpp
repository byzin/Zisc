/*!
  \file log-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_LOG_INL_HPP
#define ZISC_FLOCK_LOG_INL_HPP

#include "log.hpp"
// Standard C++ library
#include <atomic>
#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>
// Zisc
#include "definitions.hpp"
#include "epoch_pool_impl.hpp"
#include "log_array.hpp"
#include "zisc/bit.hpp"
#include "zisc/error.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \details No detailed description
  */
inline
Log::Log() noexcept :
    log_array_pool_{nullptr},
    values_{nullptr},
    count_{0}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
inline
Log::Log(Log&& other) noexcept :
    log_array_pool_{other.log_array_pool_},
    values_{other.values_},
    count_{other.count_}
{
}

/*!
  \details No detailed description
  */
inline
Log::~Log() noexcept
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  \return No description
  */
inline
Log& Log::operator=(Log&& other) noexcept
{
  log_array_pool_ = other.log_array_pool_;
  values_ = other.values_;
  count_ = other.count_;
  return *this;
}

/*!
  \details commits a value to the log, or returns existing value if already committed
  along with a false flag. Type must be convertible to void*
  initialized with null pointer, so should not commit a nullptr (or zero)
  code tags pointers with a count or flag to avoid this

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename Type> inline
auto Log::commitValue(Type&& value) noexcept -> CommitResultT<Type>
{
  if (isEmpty())
    return std::make_pair(std::forward<Type>(value), true);

  EntryPtr l = nextEntry();
  using ValueT = EntryT::value_type;
  ValueT old_value = l->load(std::memory_order::acquire);
  ValueT new_value = bit_cast<ValueT>(value);
  using T = std::remove_cvref_t<Type>;
  CommitResultT<Type> result = ((old_value == nullptr) &&
                                l->compare_exchange_strong(old_value,
                                                           new_value,
                                                           std::memory_order::acq_rel,
                                                           std::memory_order::acquire))
      ? std::make_pair(std::forward<Type>(value), true)
      : std::make_pair(bit_cast<T>(old_value), false);
  return result;
}

/*!
  \details this version tags 48th bit so value can be zero

  \tparam Type No description.
  \param [in] value No description.
  \return No description
  */
template <typename Type> inline
auto Log::commitValueSafe(Type&& value) noexcept -> CommitResultT<Type>
{
  static_assert(sizeof(Type) <= 6 || std::is_pointer_v<Type>,
                "Type for commit_value_safe must be a pointer or at most 6 bytes");
  if (isEmpty())
    return std::make_pair(std::forward<Type>(value), true);

  constexpr std::size_t set_bit = 1ull << 48ull;
  EntryPtr l = nextEntry();
  using ValueT = EntryT::value_type;
  ValueT old_value = l->load(std::memory_order::acquire);
  ValueT new_value = bit_cast<ValueT>(bit_cast<std::size_t>(value) | set_bit);
  using T = std::remove_cvref_t<Type>;
  CommitResultT<Type> result = ((old_value == nullptr) &&
                                l->compare_exchange_strong(old_value,
                                                           new_value,
                                                           std::memory_order::acq_rel,
                                                           std::memory_order::acquire))
      ? std::make_pair(std::forward<Type>(value), true)
      : std::make_pair(bit_cast<T>(bit_cast<std::size_t>(old_value) & ~set_bit), false);
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
std::size_t Log::count() const noexcept
{
  return count_;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Log::currentEntry() noexcept -> EntryPtr
{
  ZISC_ASSERT(!isEmpty(), "The log is empty.");
  EntryReference ref = (*values_)[count() - 1];
  return std::addressof(ref);
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Log::currentEntry() const noexcept -> ConstEntryPtr
{
  ZISC_ASSERT(!isEmpty(), "The log is empty.");
  ConstEntryReference ref = (*values_)[count() - 1];
  return std::addressof(ref);
}

template <std::invocable Function> inline
std::invoke_result_t<Function> Log::doWith(Function&& func,
                                           LogArray* pa,
                                           const std::size_t c) noexcept
{
  LogArray* hold_pa = values_;
  const std::size_t hold_count = count();

  using ResultT = std::invoke_result_t<Function>;
  if constexpr (std::is_void_v<ResultT>) {
    set(pa, c);
    func();
    set(hold_pa, hold_count);
  }
  else {
    set(pa, c);
    ResultT result = func();
    set(hold_pa, hold_count);
    return result;
  }
}

/*!
  \details No detailed description

  \return No description
  */
inline
bool Log::isEmpty() const noexcept
{
  const bool result = values_ == nullptr;
  return result;
}

/*!
  \details No detailed description

  \return No description
  */
inline
auto Log::nextEntry() noexcept -> EntryPtr
{
  ZISC_ASSERT(!isEmpty(), "The log is empty.");
  if (count() == LogArray::length()) {
    count_ = 0;
    LogArray* next_array = values_->next().load(std::memory_order::acquire);
    if (next_array != nullptr) {
      values_ = next_array;
    }
    else { // next_array == nullptr, try to commit a new log array
      LogArray* new_array = log_array_pool_->newObj();
      if (values_->next().compare_exchange_strong(next_array,
                                                  new_array,
                                                  std::memory_order::acq_rel,
                                                  std::memory_order::acquire)) {
        values_ = new_array;
      }
      else {
        values_ = next_array;
        new_array->destroy(log_array_pool_);
        log_array_pool_->destruct(new_array);
      }
    }
  }
  EntryReference ref = (*values_)[count_++];
  return std::addressof(ref);
}

/*!
  \details No detailed description

  \param [in] log_array_pool No description.
  */
inline
void Log::setLogArrayPool(MemoryPoolT* log_array_pool) noexcept
{
  log_array_pool_ = log_array_pool;
}

/*!
  \details No detailed description

  \tparam Function No description.
  \param [in] func No description.
  \return No description
  */
template <std::invocable Function> inline
bool Log::skipIfDone(Function&& func) noexcept
{
  bool result = isEmpty();
  if (result) {
    func();
  }
  else {
    EntryPtr l = nextEntry();
    if (l->load(std::memory_order::acquire) == nullptr) { // check that no already completed
      func();
      // mark as completed
      auto* v = bit_cast<void*>(cast<std::size_t>(1));
      l->store(v, std::memory_order::release);
      result = true;
    }
  }
  return result;
}

/*!
  \details No detailed description

  \param [in] pa No description.
  \param [in] c No description.
  */
inline
void Log::set(LogArray* pa, const std::size_t c) noexcept
{
  values_ = pa;
  count_ = c;
}

} /* namespace zisc::flock */

#endif /* ZISC_FLOCK_LOG_INL_HPP */
