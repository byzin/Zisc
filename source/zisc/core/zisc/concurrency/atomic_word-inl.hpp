/*!
  \file atomic_word-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_WORD_INL_HPP
#define ZISC_ATOMIC_WORD_INL_HPP

#include "atomic_word.hpp"
// Standard C++ library
#include <condition_variable>
#include <atomic>
#include <cstddef>
#include <mutex>
#include <type_traits>
#include <utility>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

#if defined(Z_WINDOWS) || defined(Z_LINUX)

/*!
  \brief No brief description

  No detailed description.
  */
template <>
class AtomicWordBase<true> : private NonCopyable<AtomicWordBase<true>>
{
};

#endif // Z_WINDOWS || Z_LINUX

/*!
  \brief No brief description

  No detailed description.
  */
template <bool kOsSpecified>
class AtomicWordBase : private NonCopyable<AtomicWordBase<kOsSpecified>>
{
 public:
  //! Return the underlying mutex
  auto lock() noexcept -> std::mutex&
  {
    return lock_;
  }

  //! Return the underlyling condition variable
  auto condition() noexcept -> std::condition_variable&
  {
    return condition_;
  }

 private:
  std::mutex lock_;
  std::condition_variable condition_;
  static_assert(std::alignment_of_v<std::condition_variable> <=
                std::alignment_of_v<std::mutex>);
};

// template explicit instantiation
template <>
void Atomic::wait<false>(AtomicWord<false>* word,
                         const Atomic::WordValueType old,
                         const std::memory_order order) noexcept;
template <>
void Atomic::wait<true>(AtomicWord<true>* word,
                        const Atomic::WordValueType old,
                        const std::memory_order order) noexcept;
template <>
void Atomic::notifyOne<false>(AtomicWord<false>* word) noexcept;
template <>
void Atomic::notifyOne<true>(AtomicWord<true>* word) noexcept;
template <>
void Atomic::notifyAll<false>(AtomicWord<false>* word) noexcept;
template <>
void Atomic::notifyAll<true>(AtomicWord<true>* word) noexcept;

/*!
  \details No detailed description
  */
template <bool kOsSpecified> inline
AtomicWord<kOsSpecified>::AtomicWord() noexcept
{
  store(0, std::memory_order::release);
}

/*!
  \details No detailed description

  \param [in] value No description.
  */
template <bool kOsSpecified> inline
AtomicWord<kOsSpecified>::AtomicWord(const Atomic::WordValueType value) noexcept
    : word_{value}
{
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kOsSpecified> inline
auto AtomicWord<kOsSpecified>::get() noexcept -> Atomic::WordValueType&
{
  return word_;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kOsSpecified> inline
auto AtomicWord<kOsSpecified>::get() const noexcept -> const Atomic::WordValueType&
{
  return word_;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kOsSpecified> inline
constexpr auto AtomicWord<kOsSpecified>::isSpecialized() noexcept -> bool
{
  const bool flag =
#if defined(Z_WINDOWS) || defined(Z_LINUX)
      kOsSpecified;
#else
      false;
#endif
  return flag;
}

/*!
  \details No detailed description

  \param [in] order No description.
  \return No description
  */
template <bool kOsSpecified> inline
auto AtomicWord<kOsSpecified>::load(const std::memory_order order) const noexcept
    -> Atomic::WordValueType
{
  const Atomic::WordValueType result = atomic_load(&word_, order);
  return result;
}

/*!
  \details No detailed description

  \param [in] value No description.
  \param [in] order No description.
  */
template <bool kOsSpecified> inline
void AtomicWord<kOsSpecified>::store(const Atomic::WordValueType value,
                                     const std::memory_order order) noexcept
{
  Atomic::store(&word_, value, order);
}

} // namespace zisc

#endif // ZISC_ATOMIC_WORD_INL_HPP
