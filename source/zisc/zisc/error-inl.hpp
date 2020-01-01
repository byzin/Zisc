/*!
  \file error-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_INL_HPP
#define ZISC_ERROR_INL_HPP

#include "error.hpp"
// Standard C++ library
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <utility>

namespace zisc {

/*!
  \details No detailed description

  \tparam Types No description.
  \param [in] condition No description.
  \param [in] messages No description.
  */
template <typename ...Types> inline
void assertIfFalse(const bool condition, Types&&... messages) noexcept
{
  if (!condition) {
    raiseError(std::forward<Types>(messages)...);
  }
}

/*!
  \details No detailed description

  \tparam Types No description.
  \param [out] output_stream No description.
  \param [in] messages No description.
  \return No description
  */
template <typename ...Types> inline
std::ostream& outputMessage(std::ostream& output_stream,
                            Types&&... messages) noexcept
{
  return (output_stream << ... << messages) << std::endl;
}

/*!
  \details No detailed description

  \tparam Types No description.
  \param [in] messages No description.
  */
template <typename ...Types> inline
void raiseError(Types&&... messages) noexcept
{
  outputMessage(std::cerr, std::forward<Types>(messages)...);
  std::abort();
}

} // namespace zisc

#endif // ZISC_ERROR_INL_HPP
