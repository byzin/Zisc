/*!
  \file future.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FUTURE_HPP
#define ZISC_FUTURE_HPP

// Standard C++ library
#include <future>
// Zisc
#include "zisc/non_copyable.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.

  \tparam T No description.
  */
template <typename T>
class Future : private NonCopyable<Future<T>>
{
 public:
  //! Create a invalid future
  Future() noexcept = default;

  //! Create a future
  Future(std::future<T>&& impl, const int64b id) noexcept;

  //! Move a data
  Future(Future&& other) noexcept;


  //! Move a data
  Future& operator=(Future&& other) noexcept;


  //! Return the result of the associated task
  T get();

  //! Check the associated task ID of the future
  int64b id() const noexcept;

  //! Check if the future has shared state
  bool isValid() const noexcept;

  //! Block until the result becomes available
  void wait() const;

 private:
  std::future<T> impl_;
  int64b id_;
};

} /* namespace zisc */

#include "future-inl.hpp"

#endif /* ZISC_FUTURE_HPP */
