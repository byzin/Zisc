/*!
  \file atomic.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_HPP
#define ZISC_ATOMIC_HPP

namespace zisc {

/*!
  \brief Atomic functions
  */
class Atomic
{
 public:
  //! Perform atomic addition
  template <typename Integer>
  static Integer add(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic subtraction
  template <typename Integer>
  static Integer sub(Integer* ptr, const Integer value) noexcept;

  //! Atomically replace with the given value
  template <typename Integer>
  static Integer exchange(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic increment
  template <typename Integer>
  static Integer increment(Integer* ptr) noexcept;

  //! Perform atomic decrement 
  template <typename Integer>
  static Integer decrement(Integer* ptr) noexcept;

  //! Atomically compare and perform exchange
  template <typename Integer>
  static Integer compareAndExchange(Integer* ptr,
                                    const Integer cmp,
                                    const Integer value) noexcept;

  //!
  template <typename Integer>
  static Integer min(Integer* ptr, const Integer value) noexcept;

  //!
  template <typename Integer>
  static Integer max(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise and
  template <typename Integer>
  static Integer andBit(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise or
  template <typename Integer>
  static Integer andOr(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise or
  template <typename Integer>
  static Integer andXor(Integer* ptr, const Integer value) noexcept;

  //! Indicate that the type is always lock-free
  template <typename Type>
  static constexpr bool isAlwaysLockFree() noexcept;

  //! Check if the atomic operations are lock-free
  template <typename Type>
  static bool isLockFree() noexcept;

 private:
  //! Perform atomic addition
  template <typename Integer>
  static Integer addImpl(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic subtraction
  template <typename Integer>
  static Integer subImpl(Integer* ptr, const Integer value) noexcept;

  //! Atomically replace with the given value
  template <typename Integer>
  static Integer exchangeImpl(Integer* ptr, const Integer value) noexcept;

  //! Indicate that the type is always lock-free
  template <typename Type>
  static constexpr bool isAlwaysLockFreeImpl() noexcept;

  //! Check if the atomic operations are lock-free
  template <typename Type>
  static bool isLockFreeImpl() noexcept;
};

} // namespace zisc

#include "atomic-inl.hpp"

#endif // ZISC_ATOMIC_HPP
