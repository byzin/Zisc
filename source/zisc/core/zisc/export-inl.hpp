/*!
  \file export-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_EXPORT_INL_HPP
#define ZISC_EXPORT_INL_HPP

#include "export.hpp"

#if defined(Z_WINDOWS)

/*!
  \def ZISC_EXPORT_C_FUNCTION
  \brief No brief description

  No detailed description.
  */
#define ZISC_EXPORT_C_FUNCTION extern "C" __declspec(dllexport)

/*!
  \def ZISC_EXPORT_CXX_FUNCTION
  \brief No brief description

  No detailed description.
  */
#define ZISC_EXPORT_CXX_FUNCTION __declspec(dllexport)

#else // Z_WINDOWS

/*!
  \def ZISC_EXPORT_C_FUNCTION
  \brief No brief description

  No detailed description.
  */
#define ZISC_EXPORT_C_FUNCTION extern "C"

/*!
  \def ZISC_EXPORT_CXX_FUNCTION
  \brief No brief description

  No detailed description.
  */
#define ZISC_EXPORT_CXX_FUNCTION

#endif // Z_WINDOWS

#endif /* ZISC_EXPORT_INL_HPP */
