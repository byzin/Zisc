/*!
  \file binary_data-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BINARY_DATA_INL_HPP
#define ZISC_BINARY_DATA_INL_HPP

#include "binary_data.hpp"
// Standard C++ library
#include <istream>
#include <ostream>
#include <string>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename Type> inline
std::istream& read(Type* data,
                   std::istream* binary_stream,
                   const std::streamsize size) noexcept
{
  ZISC_ASSERT(data != nullptr, "The data is null.");
  ZISC_ASSERT(binary_stream != nullptr, "The input stream is null.");
  return binary_stream->read(reinterpret_cast<char*>(data), size);
}

/*!
  */
template <typename Type> inline
std::ostream& write(const Type* data,
                    std::ostream* binary_stream,
                    const std::streamsize size) noexcept
{
  ZISC_ASSERT(data != nullptr, "The data is null.");
  ZISC_ASSERT(binary_stream != nullptr, "The output stream is null.");
  return binary_stream->write(reinterpret_cast<const char*>(data), size);
}

} // namespace zisc

#endif // ZISC_BINARY_DATA_INL_HPP
