/*!
  \file zisc_config-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CONFIG_INL_HPP
#define ZISC_CONFIG_INL_HPP

// Zisc
#include "zisc/zisc_config.hpp"

namespace zisc {

static_assert(sizeof(int8b) == 1, "The size of int8b is wrong.");
static_assert(sizeof(int16b) == 2, "The size of int16b is wrong.");
static_assert(sizeof(int32b) == 4, "The size of int32b is wrong.");
static_assert(sizeof(int64b) == 8, "The size of int64b is wrong.");
static_assert(sizeof(uint8b) == 1, "The size of uint8b is wrong.");
static_assert(sizeof(uint16b) == 2, "The size of uint16b is wrong.");
static_assert(sizeof(uint32b) == 4, "The size of uint32b is wrong.");
static_assert(sizeof(uint64b) == 8, "The size of uint64b is wrong.");

/*!
  */
inline
constexpr auto Config::implType() noexcept -> ImplType
{
  ImplType type = ImplType::kGcc;
#if defined(Z_GCC)
  type = ImplType::kGcc;
#elif defined(Z_CLANG)
  type = ImplType::kClang;
#elif defined(Z_MSVC)
  type = ImplType::kMsvc;
#else
  static_assert(false, "Unsupported implementation is specified.");
#endif
  return type;
}

/*!
  */
inline
constexpr bool Config::isStlBinaryTreeUsed() noexcept
{
#if defined(ZISC_ALGORITHM_STL_BINARY_TREE)
  const bool flag = true;
#else // ZISC_ALGORITHM_STL_BINARY_TREE
  const bool flag = false;
#endif // ZISC_ALGORITHM_STL_BINARY_TREE
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathUsed() noexcept
{
#if defined(ZISC_MATH_STL)
  const bool flag = true;
#else // ZISC_MATH_STL
  const bool flag = false;
#endif // ZISC_MATH_STL
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathExpUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_EXP)
  flag = true;
#endif // ZISC_MATH_STL_EXP
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathLogUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_LOG)
  flag = true;
#endif // ZISC_MATH_STL_LOG
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathPowUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_POW)
  flag = true;
#endif // ZISC_MATH_STL_POW
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathSqrtUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_SQRT)
  flag = true;
#endif // ZISC_MATH_STL_SQRT
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathCbrtUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_CBRT)
  flag = true;
#endif // ZISC_MATH_STL_CBRT
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathTrigonometricUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_TRIGONOMETRIC)
  flag = true;
#endif // ZISC_MATH_STL_TRIGONOMETRIC
  return flag;
}

/*!
  */
inline
constexpr bool Config::isStlMathInvTrigonometricUsed() noexcept
{
  bool flag = isStlMathUsed();
#if defined(ZISC_MATH_STL_INV_TRIGONOMETRIC)
  flag = true;
#endif // ZISC_MATH_STL_INV_TRIGONOMETRIC
  return flag;
}

} // namespace zisc

#endif // ZISC_CONFIG_INL_HPP
