/*!
  \file file binary_data.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_BINARY_DATA_HPP
#define ZISC_BINARY_DATA_HPP

// Standard C++ library
#include <istream>
#include <ostream>
#include <string>

namespace zisc {

//! Extract data from stream
template <typename Type>
std::istream& read(Type* data,
                   std::istream* binary_stream,
                   const std::streamsize size = sizeof(Type)) noexcept;

//! Insert data into the stream
template <typename Type>
std::ostream& write(const Type* data,
                    std::ostream* binary_stream,
                    const std::streamsize size = sizeof(Type)) noexcept;

} // namespace zisc

#include "binary_data-inl.hpp"

#endif // ZISC_BINARY_DATA_HPP
