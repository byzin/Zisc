/*!
  \file error.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_HPP
#define ZISC_ERROR_HPP

#ifdef ZISC_ASSERTION

#define ZISC_ASSERT(condition, ...) \
    zisc::assertError(condition, "AssertError in ", __FILE__, " at ", __LINE__, \
                      ": ", __VA_ARGS__)

#define ZISC_ASSERTION_STATEMENT(statement) statement

#else // ZISC_ASSERTION

#define ZISC_ASSERT(condition, ...)

#define ZISC_ASSERTION_STATEMENT(statement)

#endif // ZISC_ASSERTION

#ifdef ZISC_LOGGING

#define ZISC_LOG(...) zisc::logMessage(__VA_ARGS__)

#else // ZISC_LOGGING

#define ZISC_LOG(...)

#endif // ZISC_LOGGING

namespace zisc {

//! If condition is false, print messages and raise an error
template <typename ...Types>
void assertError(const bool condition, const Types&... messages) noexcept;

//! Log messages
template <typename ...Types>
void logMessage(const Types&... messages) noexcept;

//! Print messages and raise an error
template <typename ...Types>
void raiseError(const Types&... messages) noexcept;

} // namespace zisc

#include "error-inl.hpp"

#endif // ZISC_ERROR_HPP
