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
  enum class ImplType : uint
  {
    kGcc,
    kClang,
    kMsvc
  };


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
  static Integer orBit(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise or
  template <typename Integer>
  static Integer xorBit(Integer* ptr, const Integer value) noexcept;

  //! Return the implementation type
  static constexpr ImplType implType() noexcept;

  //! Indicate that the type is always lock-free
  template <typename Type>
  static constexpr bool isAlwaysLockFree() noexcept;

  //! Check if the atomic operations are lock-free
  template <typename Type>
  static bool isLockFree() noexcept;

  //! Perform an expression atomically
  template <typename Integer, typename Function, typename ...Types>
  static Integer perform(Integer* ptr,
                         Function&& expression,
                         Types&&... arguments) noexcept;

 private:
  //! Perform atomic addition
  template <typename Integer, ImplType kImpl>
  static Integer addImpl(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic subtraction
  template <typename Integer, ImplType kImpl>
  static Integer subImpl(Integer* ptr, const Integer value) noexcept;

  //! Atomically replace with the given value
  template <typename Integer, ImplType kImpl>
  static Integer exchangeImpl(Integer* ptr, Integer value) noexcept;

  //! Perform atomic increment
  template <typename Integer, ImplType kImpl>
  static Integer incrementImpl(Integer* ptr) noexcept;

  //! Perform atomic decrement 
  template <typename Integer, ImplType kImpl>
  static Integer decrementImpl(Integer* ptr) noexcept;

  //! Atomically compare and perform exchange
  template <typename Integer, ImplType kImpl>
  static Integer compareAndExchangeImpl(Integer* ptr,
                                        Integer cmp,
                                        Integer value) noexcept;

  //!
  template <typename Integer, ImplType kImpl>
  static Integer minImpl(Integer* ptr, const Integer value) noexcept;

  //!
  template <typename Integer, ImplType kImpl>
  static Integer maxImpl(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise and
  template <typename Integer, ImplType kImpl>
  static Integer andBitImpl(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise or
  template <typename Integer, ImplType kImpl>
  static Integer orBitImpl(Integer* ptr, const Integer value) noexcept;

  //! Perform atomic bitwise or
  template <typename Integer, ImplType kImpl>
  static Integer xorBitImpl(Integer* ptr, const Integer value) noexcept;

  //! Indicate that the type is always lock-free
  template <typename Type, ImplType kImpl>
  static constexpr bool isAlwaysLockFreeImpl() noexcept;

  //! Check if the atomic operations are lock-free
  template <typename Type, ImplType kImpl>
  static bool isLockFreeImpl() noexcept;
};

} // namespace zisc

#include "atomic-inl.hpp"

#endif // ZISC_ATOMIC_HPP
