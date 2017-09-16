/*!
  \file error-inl.hpp"
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_INL_HPP
#define ZISC_ERROR_INL_HPP

#include "error.hpp"
// Standard C++ library
#include <cstdlib>
#include <iostream>

namespace zisc {

namespace inner {

/*!
  */
template <typename Type> inline
void outputMessage(const Type& message) noexcept
{
  std::cout << message;
}

/*!
  */
template <typename Type, typename ...Types> inline
void outputMessage(const Type& message, const Types&... messages) noexcept
{
  outputMessage(message);
  outputMessage(messages...);
}

/*!
  \details
  No detailed.
  */
template <typename Type> inline
void outputErrorMessage(const Type& message) noexcept
{
  std::cerr << message;
}

/*!
  \details
  No detailed.
  */
template <typename Type, typename ...Types> inline
void outputErrorMessage(const Type& message, const Types&... messages) noexcept
{
  outputErrorMessage(message);
  outputErrorMessage(messages...);
}

} // namespace inner

/*!
  \details
  No detailed.
  */
template <typename ...Types> inline
void assertError(const bool condition, const Types& ...messages) noexcept
{
  if (!condition) {
    raiseError(messages...);
  }
}

/*!
  */
template <typename ...Types> inline
void logMessage(const Types&... messages) noexcept
{
  inner::outputMessage(messages..., "\n");
}

/*!
  \details
  No detailed.
  */
template <typename ...Types> inline
void raiseError(const Types&... messages) noexcept
{
  inner::outputErrorMessage(messages..., "\n");
  std::exit(EXIT_FAILURE);
}

} // namespace zisc

#endif // ZISC_ERROR_INL_HPP
