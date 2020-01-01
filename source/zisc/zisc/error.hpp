/*!
  \file error.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_HPP
#define ZISC_ERROR_HPP

// Standard C++ library
#include <ostream>

// Preprocessor

//! Return the full path to the current file
#define ZISC_GET_FILE_NAME __FILE__

//! Return the current line number in the source file
#define ZISC_GET_LINE_NUMBER __LINE__

/*!
  \def ZISC_ASSERT(condition, ...)
  \brief If condition is false, assert outputs messages and calls std::abort

  No detailed description.

  \param [in] condition No description.
  \param [in] ... No description.
  */
#ifdef ZISC_ASSERTION
  #define ZISC_ASSERT(condition, ...) \
      zisc::assertIfFalse(condition, \
                          "AssertError in " ZISC_GET_FILE_NAME " at ", \
                          ZISC_GET_LINE_NUMBER, \
                          ": ", \
                          __VA_ARGS__)
#else // ZISC_ASSERTION
  #define ZISC_ASSERT(condition, ...) static_cast<void>(condition)
#endif // ZISC_ASSERTION

/*!
  \def ZISC_STATIC_ASSERT(condition, message)
  \brief If condition is false, a compile-time error is issued, and assert outputs message

  No detailed description.

  \param [in] condition No description.
  \param [in] message No description.
  */
#ifdef ZISC_ASSERTION
  #define ZISC_STATIC_ASSERT(condition, message) \
      static_assert(condition, message)
#else // ZISC_ASSERTION
  #define ZISC_STATIC_ASSERT(condition, message) \
      static_cast<void>(condition); \
      static_cast<void>(message)
#endif // ZISC_ASSERTION

namespace zisc {

//! If condition is false, print messages and raise an error
template <typename ...Types>
void assertIfFalse(const bool condition, Types&&... messages) noexcept;

//! Send messages to the 'output_stream' stream
template <typename ...Types>
std::ostream& outputMessage(std::ostream& output_stream,
                            Types&&... messages) noexcept;

//! Print messages and raise an error
template <typename ...Types>
void raiseError(Types&&... messages) noexcept;

} // namespace zisc

/*!
  \example error_example.cpp

  This is an example of how to use the zisc error functions.
  */

#include "error-inl.hpp"

#endif // ZISC_ERROR_HPP
