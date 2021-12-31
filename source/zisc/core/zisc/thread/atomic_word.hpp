/*!
  \file atomic_word.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ATOMIC_WORD_HPP
#define ZISC_ATOMIC_WORD_HPP

// Standard C++ library
#include <atomic>
#include <type_traits>
// Zisc
#include "atomic.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// Forward declaration
template <bool kOsSpecified> class AtomicWordBase;

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // Z_GCC || Z_CLANG

/*!
  \brief No brief description

  No detailed description.

  \tparam kOsSpecified No description.
  */
template <bool kOsSpecified>
class AtomicWord : public AtomicWordBase<kOsSpecified>
{
 public:
  //! Construct an atomic word
  AtomicWord() noexcept;

  //! Construct an atomic word
  AtomicWord(const Atomic::WordValueType value) noexcept;


  //! Return the underlying word value
  Atomic::WordValueType& get() noexcept;

  //! Return the underlying word value
  const Atomic::WordValueType& get() const noexcept;

  //! Check if the atomic word is specialized
  static constexpr bool isSpecialized() noexcept;

  //! Get the underlying word value atomically
  Atomic::WordValueType load(const std::memory_order order = Atomic::defaultMemOrder()) const noexcept;

  //! Set a value to the underlying word atomically
  void store(const Atomic::WordValueType value,
             const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

 private:
  Atomic::WordValueType word_ = 0;
};

#if defined(Z_GCC) || defined(Z_CLANG)
#pragma GCC diagnostic pop
#endif // Z_GCC || Z_CLANG

} // namespace zisc

#include "atomic_word-inl.hpp"

#endif // ZISC_ATOMIC_WORD_HPP
