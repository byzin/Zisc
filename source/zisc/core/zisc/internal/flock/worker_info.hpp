/*!
  \file worker_info.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FLOCK_WORKER_INFO_HPP
#define ZISC_FLOCK_WORKER_INFO_HPP

// Standard C++ library
#include <span>
#include <thread>
#include <type_traits>
// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc::flock {

/*!
  \brief No brief description

  No detailed description.
  */
class WorkerInfo
{
 public:
  //! Create empty info
  WorkerInfo() noexcept;

  //! Create a worker info
  WorkerInfo(const std::span<const std::thread::id> id_list) noexcept;


  //! Return the current worker index
  std::size_t getCurrentWorkerId() const noexcept;

  //! Return the number of workers
  std::size_t numOfWorkers() const noexcept;

  //! Take out one in the given list for current worker
  template <typename Type>
  std::add_lvalue_reference_t<Type> takeOut(std::span<Type> list) const noexcept;

  //! Return the underlying thread id list
  std::span<const std::thread::id> threadIdList() const noexcept;

 private:
  std::span<const std::thread::id> thread_id_list_;
};

} /* namespace zisc::flock */

#include "worker_info-inl.hpp"

#endif /* ZISC_FLOCK_WORKER_INFO_HPP */
