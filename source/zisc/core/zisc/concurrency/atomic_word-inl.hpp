/*!
  \file atomic_word-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
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
  std::mutex& lock() noexcept
  {
    return lock_;
  }

  //! Return the underlyling condition variable
  std::condition_variable& condition() noexcept
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
void Atomic::wait<false>(AtomicWord<false>*,
                         const Atomic::WordValueType,
                         const std::memory_order) noexcept;
template <>
void Atomic::wait<true>(AtomicWord<true>*,
                        const Atomic::WordValueType,
                        const std::memory_order) noexcept;
template <>
void Atomic::notifyOne<false>(AtomicWord<false>*) noexcept;
template <>
void Atomic::notifyOne<true>(AtomicWord<true>*) noexcept;
template <>
void Atomic::notifyAll<false>(AtomicWord<false>*) noexcept;
template <>
void Atomic::notifyAll<true>(AtomicWord<true>*) noexcept;

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
Atomic::WordValueType& AtomicWord<kOsSpecified>::get() noexcept
{
  return word_;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kOsSpecified> inline
const Atomic::WordValueType& AtomicWord<kOsSpecified>::get() const noexcept
{
  return word_;
}

/*!
  \details No detailed description

  \return No description
  */
template <bool kOsSpecified> inline
constexpr bool AtomicWord<kOsSpecified>::isSpecialized() noexcept
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
Atomic::WordValueType AtomicWord<kOsSpecified>::load(const std::memory_order order) const noexcept
{
  const auto result = atomic_load(&word_, order);
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
