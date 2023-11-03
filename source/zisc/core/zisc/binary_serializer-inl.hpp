/*!
  \file binary_serializer-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  No copyright
  */

#ifndef ZISC_BINARY_SERIALIZER_INL_HPP
#define ZISC_BINARY_SERIALIZER_INL_HPP

#include "binary_serializer.hpp"
// Standard C++ library
#include <ios>
#include <istream>
#include <memory>
#include <ostream>
// Zisc
#include "error.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \param [in] data_stream No description.
  \return No description
  */
inline
auto BinarySerializer::getDistance(std::istream* data_stream) noexcept -> std::streamsize
{
  std::streamsize distance = 0;
  const std::istream::pos_type pos = data_stream->tellg();
  if (pos != -1) {
    const std::istream::iostate state = data_stream->rdstate();
    // Get the end position
    data_stream->seekg(0, std::ios_base::end);
    const std::istream::pos_type end = data_stream->tellg();
    // Back to current position
    data_stream->clear(state);
    data_stream->seekg(pos, std::ios_base::beg);

    distance = cast<std::streamsize>(end - pos);
  }
  return distance;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [out] data No description.
  \param [in,out] data_stream No description.
  \param [in] size No description.
  \return No description
  */
template <typename Type> inline
auto BinarySerializer::read(Type* data,
                            std::istream* data_stream,
                            const std::streamsize size) noexcept -> std::istream&
{
  static_assert(sizeof(char) == 1, "The size of 'char' isn't 1.");
  //! \todo Exception check
  ZISC_ASSERT(data != nullptr, "The given data is null.");
  ZISC_ASSERT(data_stream != nullptr, "The given stream is null.");
  return data_stream->read(reinterp<char*>(data), size);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] data No description.
  \param [in,out] data_stream No description.
  \param [in] size No description.
  \return No description
  */
template <typename Type> inline
auto BinarySerializer::write(const Type* data,
                             std::ostream* data_stream,
                             const std::streamsize size) noexcept -> std::ostream&
{
  static_assert(sizeof(const char) == 1, "The size of 'char' isn't 1.");
  //! \todo Exception check
  ZISC_ASSERT(data != nullptr, "The given data is null.");
  ZISC_ASSERT(data_stream != nullptr, "The given stream is null.");
  return data_stream->write(reinterp<const char*>(data), size);
}

/*!
  \details No detailed description

  \param [in,out] data_stream No description.
  */
inline
void BinarySerializer::backToBegin(std::istream* data_stream) noexcept
{
  data_stream->clear();
  data_stream->seekg(0, std::ios_base::beg);
}

} // namespace zisc

#endif // ZISC_BINARY_SERIALIZER_INL_HPP
