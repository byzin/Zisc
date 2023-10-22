/*!
  \file error-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace zisc {

/*!
  \details No detailed description

  \param [in] code No description.
  */
inline
SystemError::SystemError(const ErrorCode code) :
    std::system_error(static_cast<int>(code), ErrorCategory{})
{
}

/*!
  \details No detailed description

  \param [in] code No description.
  \param [in] what_arg No description.
  */
inline
SystemError::SystemError(const ErrorCode code, const std::string_view what_arg) :
    std::system_error(static_cast<int>(code), ErrorCategory{}, what_arg.data())
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
SystemError::SystemError(SystemError&& other) noexcept:
    std::system_error(other)
{
}

/*!
  \details No detailed description

  \param [in,out] other No description.
  */
inline
auto SystemError::operator=(SystemError&& other) noexcept -> SystemError&
{
  std::system_error::operator=(other);
  return *this;
}

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
auto outputMessage(std::ostream& output_stream, Types&&... messages) noexcept -> std::ostream&
{
  return (output_stream << ... << std::forward<Types>(messages)) << std::endl;
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
