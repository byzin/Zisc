/*!
  \file tag.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_TAG_HPP
#define ZISC_FLOCK_TAG_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <span>
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

// Forward declaration
class Log;
class WorkerInfo;
class WriteAnnouncements;

/*!
  \brief No brief description

  No detailed description.

  \tparam Type No description.
  */
template <typename Type>
class Tag
{
 public:
  // Type aliases
  using ValueT = Type;
  using ConstT = std::add_const_t<ValueT>;
  using Pointer = std::add_pointer_t<ValueT>;
  using ConstPointer = std::add_pointer_t<ConstT>;
  using Reference = std::add_lvalue_reference_t<ValueT>;
  using RReference = std::add_rvalue_reference_t<ValueT>;
  using ConstReference = std::add_lvalue_reference_t<ConstT>;
  using IT = std::size_t;


  //!
  static IT add(const IT old_value, const IT new_value) noexcept;

  //!
  static bool cas(std::atomic<IT>& loc,
                  IT old_value,
                  ConstReference value,
                  WriteAnnouncements* write_announcements,
                  Log* log,
                  const bool aba_free = false) noexcept;

  //!
  static constexpr IT cntBit() noexcept;

  //!
  static constexpr IT cntMask() noexcept;

  //!
  static constexpr IT dataMask() noexcept;

  //!
  static IT inc(const IT old_value) noexcept;

  //!
  static IT init(ConstReference v) noexcept;

  //!
  static IT next(const IT old_value, ConstReference new_value) noexcept;

  //!
  static IT next(const IT old_value,
                 ConstReference new_value,
                 const IT addr,
                 WriteAnnouncements* write_announcements) noexcept;

  //!
  static constexpr IT panicBit() noexcept;

  //! Return the number of bits to use for tag (including panic bit)
  static constexpr std::size_t tagBits() noexcept;

  //!
  static Type value(const IT v) noexcept;

  //!
  static constexpr IT topBit() noexcept;
};

} /* namespace zisc::flock */

#include "tag-inl.hpp"

#endif /* ZISC_FLOCK_TAG_HPP */
