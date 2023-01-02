/*!
  \file error.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "error.hpp"
// Standard C++ library
#include <string>
#include <system_error>
#include <utility>

/*!
  \brief Add a case of an error code

  No detailed description.

  \param [in] code No description.
  \param [out] output No description.
  */
#define ERROR_CODE_STRING_CASE(code, output) case ErrorCode::k ## code : { \
                                               (output) = #code ## s; \
                                               break; \
                                             }

namespace zisc {

/*!
  \details No detailed description

  \param [in] code No description.
  \return No description
  */
std::string getErrorCodeString(const ErrorCode code)
{
  using namespace std::string_literals;
  std::string code_string;
  switch (code) {
    ERROR_CODE_STRING_CASE(BoundedQueueOverflow, code_string)
    ERROR_CODE_STRING_CASE(ThreadManagerQueueOverflow, code_string)
  }
  return code_string;
}

/*!
  \details No detailed description
  */
ErrorCategory::~ErrorCategory() noexcept
{
}

/*!
  \details No detailed description
  */
SystemError::~SystemError() noexcept
{
}

/*!
  \details No detailed description

  \return No description
  */
const char* ErrorCategory::name() const noexcept
{
  const char* n = "Zisc";
  return n;
}

/*!
  \details No detailed description

  \param [in] condition No description.
  \return No description
  */
std::string ErrorCategory::message(const int condition) const
{
  const auto code = static_cast<ErrorCode>(condition);
  std::string code_string{getErrorCodeString(code)};
  return code_string;
}

} // namespace zisc
