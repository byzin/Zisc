/*!
  \file binary_data-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  No copyright
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
  \details No detailed description

  \tparam Type No description.
  \param [out] data No description.
  \param [in,out] binary_stream No description.
  \param [in] size No description.
  \return No description
  */
template <typename Type> inline
std::istream& read(Type* data,
                   std::istream* binary_stream,
                   const std::streamsize size) noexcept
{
  //! \todo Exception check
  ZISC_ASSERT(data != nullptr, "The data is null.");
  ZISC_ASSERT(binary_stream != nullptr, "The input stream is null.");
  return binary_stream->read(reinterpret_cast<char*>(data), size);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] data No description.
  \param [in,out] binary_stream No description.
  \param [in] size No description.
  \return No description
  */
template <typename Type> inline
std::ostream& write(const Type* data,
                    std::ostream* binary_stream,
                    const std::streamsize size) noexcept
{
  //! \todo Exception check
  ZISC_ASSERT(data != nullptr, "The data is null.");
  ZISC_ASSERT(binary_stream != nullptr, "The output stream is null.");
  return binary_stream->write(reinterpret_cast<const char*>(data), size);
}

} // namespace zisc

#endif // ZISC_BINARY_DATA_INL_HPP
