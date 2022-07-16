/*!
  \file mutable.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_MUTABLE_HPP
#define ZISC_FLOCK_MUTABLE_HPP

// Standard C++ library
#include <atomic>
#include <concepts>
#include <span>
// Zisc
#include "tag.hpp"
#include "zisc/concepts.hpp"
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

// Forward declaration
class Log;

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <Pointer Type>
class Mutable : private NonCopyable<Type>
{
 public:
  // Type aliases
  using ValueT = Type;


  //! Create a mutable value
  Mutable() noexcept;

  //! Create a mutable value
  Mutable(ValueT value) noexcept;

  //! Move data
  Mutable(Mutable&& other) noexcept;

  //! Destroy the value
  ~Mutable() noexcept = default;


  //! Move data
  Mutable& operator=(Mutable&& other) noexcept;


  //!
  void init(ValueT value) noexcept;

  //!
  ValueT load(Log* log) const noexcept;

  //!
  ValueT read() const noexcept;

  //!
  void store(ValueT value, Log* log) noexcept;

 private:
  // Type aliases
  using TaggedT = Tag<ValueT>;
  using IT = typename TaggedT::IT;


  //!
  IT getValue(Log* log) const noexcept;


  std::atomic<IT> value_;
};

} /* namespace zisc::flock */

#include "mutable-inl.hpp"

#endif /* ZISC_FLOCK_MUTABLE_HPP */
