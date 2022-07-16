/*!
  \file write_once.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WRITE_ONCE_HPP
#define ZISC_FLOCK_WRITE_ONCE_HPP

// Standard C++ library
#include <atomic>
#include <type_traits>
// Zisc
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
template <typename Type>
class WriteOnce : private NonCopyable<WriteOnce<Type>>
{
 public:
  // Type aliases
  using ValueT = Type;
  using ConstValueT = std::add_const_t<ValueT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstValueT>;


  //! Create a write once instance
  WriteOnce() noexcept;

  //! Create a write once instance
  WriteOnce(ValueT initial) noexcept;

  //! Move data
  WriteOnce(WriteOnce&& other) noexcept;

  //! Destroy the instance
  ~WriteOnce() noexcept = default;


  //!
  WriteOnce& operator=(ValueT value) noexcept;

  //! Move data
  WriteOnce& operator=(WriteOnce&& other) noexcept;


  //!
  void init(ValueT value) noexcept;

  //!
  ValueT load(Log* log) const noexcept;

  //!
  void store(ValueT value) noexcept;

 private:
  std::atomic<Type> value_;
};

} /* namespace zisc::flock */

#include "write_once-inl.hpp"

#endif /* ZISC_FLOCK_WRITE_ONCE_HPP */
