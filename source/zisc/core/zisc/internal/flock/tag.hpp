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
  using IT = std::size_t;


  //! Create a tag
  Tag(WriteAnnouncements* write_announcements) noexcept;


  //!
  static IT add(const IT old_value, const IT new_value) noexcept;

  //!
  bool cas(std::atomic<IT>& loc,
           IT old_value,
           const Type value,
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
  static IT init(const Type v) noexcept;

  //!
  static IT next(const IT old_value, const Type new_value) noexcept;

  //!
  IT next(const IT old_value, const Type new_value, const IT addr) noexcept;

  //!
  static constexpr IT panicBit() noexcept;

  //!
  void setLogList(std::span<Log> log_list, const WorkerInfo& info) noexcept;

  //! Return the number of bits to use for tag (including panic bit)
  static constexpr std::size_t tagBits() noexcept;

  //!
  static Type value(const IT v) noexcept;

  //!
  static constexpr IT topBit() noexcept;

 private:
  //! Return the underlying log list
  std::span<Log> logList() noexcept;

  //! Return the underlying log list
  std::span<const Log> logList() const noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;


  std::span<Log> log_list_;
  WriteAnnouncements* write_announcements_;
};

} /* namespace zisc::flock */

#include "tag-inl.hpp"

#endif /* ZISC_FLOCK_TAG_HPP */
