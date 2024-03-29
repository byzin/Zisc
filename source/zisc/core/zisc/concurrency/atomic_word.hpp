/*!
  \file atomic_word.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  explicit AtomicWord(const Atomic::WordValueType value) noexcept;


  //! Return the underlying word value
  [[nodiscard]]
  auto get() noexcept -> Atomic::WordValueType&;

  //! Return the underlying word value
  [[nodiscard]]
  auto get() const noexcept -> const Atomic::WordValueType&;

  //! Check if the atomic word is specialized
  static constexpr auto isSpecialized() noexcept -> bool;

  //! Get the underlying word value atomically
  [[nodiscard]]
  auto load(const std::memory_order order = Atomic::defaultMemOrder()) const noexcept
      -> Atomic::WordValueType;

  //! Set a value to the underlying word atomically
  void store(const Atomic::WordValueType value,
             const std::memory_order order = Atomic::defaultMemOrder()) noexcept;

 private:
  Atomic::WordValueType word_ = 0;
};

} // namespace zisc

#include "atomic_word-inl.hpp"

#endif // ZISC_ATOMIC_WORD_HPP
