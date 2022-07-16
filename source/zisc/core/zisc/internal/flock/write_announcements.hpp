/*!
  \file write_announcements.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WRITE_ANNOUNCEMENT_HPP
#define ZISC_FLOCK_WRITE_ANNOUNCEMENT_HPP

// Standard C++ library
#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/memory/std_memory_resource.hpp"

namespace zisc::flock {

// Forward declaration
class WorkerInfo;

/*!
  \brief No brief description

  No detailed description.
  */
class WriteAnnouncements : private NonCopyable<WriteAnnouncements>
{
 public:
  //! Create an announcements
  WriteAnnouncements(const WorkerInfo& info,
                     pmr::memory_resource* mem_resource) noexcept;

  //! Move a data
  WriteAnnouncements(WriteAnnouncements&& other) noexcept;


  //! Move a data
  WriteAnnouncements& operator=(WriteAnnouncements&& other) noexcept;


  //!
  void clear() noexcept;

  //!
  pmr::vector<std::size_t> scan() const noexcept;

  //!
  void set(const std::size_t value) noexcept;

  //!
  static constexpr std::size_t stride() noexcept;

  //! Return the underlying worker info
  const WorkerInfo& workerInfo() const noexcept;

 private:
  /*!
    \brief No brief description

    No detailed description.
    */
  struct AtomicT
  {
    //! Create a atomic value
    AtomicT() noexcept;

    //! Move data
    AtomicT(AtomicT&& other) noexcept;

    //! Move data
    AtomicT& operator=(AtomicT&& other) noexcept;

    std::atomic_size_t v_;
  };


  //! Set the worker info
  void setWorkerInfo(const WorkerInfo& info) noexcept;


  pmr::vector<AtomicT> announcement_;
  const WorkerInfo* worker_info_;
};

} /* namespace zisc::flock */

#include "write_announcements-inl.hpp"

#endif /* ZISC_FLOCK_WRITE_ANNOUNCEMENT_HPP */
