/*!
  \file error.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
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
  */
#define ERROR_CODE_STRING_CASE(code, output) case ErrorCode::k ## code : { \
                                               output = #code ## s; \
                                               break; \
                                             }

namespace zisc {

/*!
  \details No detailed description

  \param [in] code No description.
  \return No description
  */
std::string getErrorCodeString(const ErrorCode code) noexcept
{
  using namespace std::string_literals;
  std::string code_string;
  switch (code) {
    ERROR_CODE_STRING_CASE(LockFreeBoundedQueueOverflow, code_string)
  }
  return code_string;
}

} // namespace zisc
