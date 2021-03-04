/*!
  \file binary_serializer.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  No copyright
  */

#ifndef ZISC_BINARY_SERIALIZER_HPP
#define ZISC_BINARY_SERIALIZER_HPP

// Standard C++ library
#include <ios>
#include <istream>
#include <ostream>

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class BinarySerializer
{
 public:
  //! Return the distance from current position to the end of the given stream
  static std::streamsize getDistance(std::istream* data_stream) noexcept;

  //! Deserialize a data from the given stream
  template <typename Type>
  static std::istream& read(Type* data,
                            std::istream* data_stream,
                            const std::streamsize size = sizeof(Type)) noexcept;

  //! Serialize the given data into the stream
  template <typename Type>
  static std::ostream& write(const Type* data,
                             std::ostream* data_stream,
                             const std::streamsize size = sizeof(Type)) noexcept;

  //! Set the input position indicator to begin
  static void backToBegin(std::istream* data_stream) noexcept;
};

// Type aliases

using BSerializer = BinarySerializer;

} // namespace zisc

#include "binary_serializer-inl.hpp"

#endif // ZISC_BINARY_SERIALIZER_HPP
