/*!
  \file algorithm.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "algorithm.hpp"
// Microsoft
#if defined(Z_MSVC)
#include <intrin.h>
#endif // Z_MSVC

namespace zisc {

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
short Algorithm::popcountMsvcImpl(const short x) noexcept
{
  static_assert(sizeof(short) == 2);
  const short y =
#if defined(Z_MSVC)
      __popcnt16(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
unsigned short Algorithm::popcountMsvcImpl(const unsigned short x) noexcept
{
  static_assert(sizeof(unsigned short) == 2);
  const unsigned short y =
#if defined(Z_MSVC)
      __popcnt16(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
int Algorithm::popcountMsvcImpl(const int x) noexcept
{
  static_assert(sizeof(int) == 4);
  const int y =
#if defined(Z_MSVC)
      __popcnt(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
unsigned int Algorithm::popcountMsvcImpl(const unsigned int x) noexcept
{
  static_assert(sizeof(unsigned int) == 4);
  const unsigned int y =
#if defined(Z_MSVC)
      __popcnt(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
long long Algorithm::popcountMsvcImpl(const long long x) noexcept
{
  static_assert(sizeof(long long) == 8);
  const long long y =
#if defined(Z_MSVC)
      __popcnt64(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

/*!
  \details No detailed description

  \param [in] x No description.
  \return No description
  */
unsigned long long Algorithm::popcountMsvcImpl(const unsigned long long x) noexcept
{
  static_assert(sizeof(unsigned long long) == 8);
  const unsigned long long y =
#if defined(Z_MSVC)
      __popcnt64(x);
#else // Z_MSVC
      0;
      static_cast<void>(x);
#endif // Z_MSVC
  return y;
}

} // namespace zisc
