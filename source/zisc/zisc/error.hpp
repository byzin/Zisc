/*!
  \file error.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_HPP
#define ZISC_ERROR_HPP

// Standard C++ library
#include <ostream>

// Preprocessor

#define ZISC_GET_FILE_NAME __FILE__

#define ZISC_GET_LINE_NUMBER __LINE__

#ifdef ZISC_ASSERTION

#define ZISC_ASSERT(condition, ...) \
    zisc::assertIfFalse(condition, \
                        "AssertError in ", \
                        ZISC_GET_FILE_NAME, \
                        " at ", \
                        ZISC_GET_LINE_NUMBER, \
                        ": ", \
                        __VA_ARGS__)

#else // ZISC_ASSERTION

#define ZISC_ASSERT(...)

#endif // ZISC_ASSERTION

#ifdef ZISC_LOGGING

#define ZISC_LOG(log_stream, ...) \
    zisc::outputMessage(log_stream, __VA_ARGS__)

#else // ZISC_LOGGING

#define ZISC_LOG(...)

#endif // ZISC_LOGGING

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

#include "error-inl.hpp"

#endif // ZISC_ERROR_HPP
