/*!
  \file concurrent_queue_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "concurrent_queue_test.hpp"
// Standard C++ library
#include <chrono>
#include <cstddef>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/utility.hpp"

namespace test {

/*!
  \details No detailed description

  \param [in] start_time No description.
  \param [in] end_time No description.
  \return No description
  */
std::chrono::microseconds QueueTest::calcElapsedTime(
    const Clock::time_point start_time,
    const Clock::time_point end_time) noexcept
{
  const Clock::duration elapsed_time = end_time - start_time;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
}

/*!
  \details No detailed description

  \param [in] total_op No description.
  \param [in] elapsed_time No description.
  \return No description
  */
double QueueTest::calcMops(const std::size_t total_op,
                           const std::chrono::microseconds elapsed_time) noexcept
{
  const double mops = zisc::cast<double>(total_op) /
                      zisc::cast<double>(elapsed_time.count());
  return mops;
}

} /* namespace test */
