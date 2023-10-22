/*!
  \file error.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_ERROR_HPP
#define ZISC_ERROR_HPP

// Standard C++ library
#include <ostream>
#include <string>
#include <string_view>
#include <system_error>

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
#if defined(ZISC_ASSERTION)
  #define ZISC_ASSERT(condition, ...) \
      zisc::assertIfFalse(condition, \
                          "AssertError in " ZISC_GET_FILE_NAME " at ", \
                          ZISC_GET_LINE_NUMBER, \
                          ": ", \
                          __VA_ARGS__)
#else // ZISC_ASSERTION
  #define ZISC_ASSERT(condition, ...)
#endif // ZISC_ASSERTION

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
enum class ErrorCode : int
{
  kBoundedQueueOverflow,
  kThreadManagerQueueOverflow
};

//! Return the string of the given error code
[[nodiscard]]
auto getErrorCodeString(const ErrorCode code) -> std::string;

/*!
  \brief No brief description

  No detailed description.
  */
class ErrorCategory : public std::error_category
{
 public:
  //! Construct an error category
  ErrorCategory() noexcept = default;

  //! Destruct an error category
  ~ErrorCategory() noexcept override;


  //! Return the name of the category
  [[nodiscard]]
  auto name() const noexcept -> const char* override;

  //! Return the explanatory string
  [[nodiscard]]
  auto message(const int condition) const -> std::string override;
};

/*!
  \brief No brief description

  No detailed description.
  */
class SystemError : public std::system_error
{
 public:
  //! Construct the system error
  explicit SystemError(const ErrorCode code);

  //! Construct the system error
  SystemError(const ErrorCode code, const std::string_view what_arg);

  //! Move data
  SystemError(SystemError&& other) noexcept;

  //! Finalize the system error
  ~SystemError() noexcept override;


  //! Move data
  auto operator=(SystemError&& other) noexcept -> SystemError&;
};

//! If condition is false, print messages and raise an error
template <typename ...Types>
void assertIfFalse(const bool condition, Types&&... messages) noexcept;

//! Send messages to the 'output_stream' stream
template <typename ...Types>
auto outputMessage(std::ostream& output_stream, Types&&... messages) noexcept -> std::ostream&;

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
