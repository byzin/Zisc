/*!
  \file binary_data-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
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
template <> inline
std::string read<std::string>(std::istream* binary_stream,
                              const std::streamsize size) noexcept
{
  ZISC_ASSERT(binary_stream != nullptr, "The input stream is null.");
  std::string data;
  data.resize(cast<std::size_t>(size));
  binary_stream->read(&data[0], size);
  return data;
}

/*!
  */
template <typename Type> inline
Type read(std::istream* binary_stream,
          const std::streamsize size) noexcept
{
  ZISC_ASSERT(binary_stream != nullptr, "The input stream is null.");
  constexpr uint n = sizeof(Type);
  char data[n];
  binary_stream->read(data, size);
  return *treatAs<const Type*>(data);
}

/*!
  */
template <> inline
void write<std::string>(const std::string& data,
                        std::ostream* binary_stream,
                        const std::streamsize size) noexcept
{
  ZISC_ASSERT(binary_stream != nullptr, "The output stream is null.");
  binary_stream->write(data.data(), size);
}

/*!
  */
template <typename Type> inline
void write(const Type& data,
           std::ostream* binary_stream,
           const std::streamsize size) noexcept
{
  ZISC_ASSERT(binary_stream != nullptr, "The output stream is null.");
  binary_stream->write(reinterpret_cast<const char*>(&data), size);
}

} // namespace zisc

#endif // ZISC_BINARY_DATA_INL_HPP
