/*!
  \file fnv_1a_hash_engine-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FNV_1A_HASH_ENGINE_INL_HPP
#define ZISC_FNV_1A_HASH_ENGINE_INL_HPP

#include "fnv_1a_hash_engine.hpp"
// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "hash_engine.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description

  \tparam Int8 No description.
  \param [in] seed No description.
  \param [in] n No description.
  \return No description
  */
template <HashValue ValueT> template <HashKeyElement Int8> inline
constexpr auto Fnv1aHashEngine<ValueT>::hashValue(
    const Int8* seed,
    const std::size_t n) noexcept -> ValueType
{
  ValueType x = offset();
  for (std::size_t i = 0; i < n; ++i)
    x = (x ^ cast<ValueType>(seed[i])) * prime();
  return x;
}

/*!
  \details No detailed description

  \return No description
  */
template <HashValue ValueT> inline
constexpr auto Fnv1aHashEngine<ValueT>::prime() noexcept -> ValueType
{
  ValueType p = 0;
  if constexpr (sizeof(ValueType) == 4)
    p = cast<ValueType>(16777619u);
  else if constexpr (sizeof(ValueType) == 8)
    p = cast<ValueType>(1099511628211ull);
  return p;
}

/*!
  \details No detailed description

  \return No description
  */
template <HashValue ValueT> inline
constexpr auto Fnv1aHashEngine<ValueT>::offset() noexcept -> ValueType
{
  ValueType p = 0;
  if constexpr (sizeof(ValueType) == 4)
    p = cast<ValueType>(2166136261u);
  else if constexpr (sizeof(ValueType) == 8)
    p = cast<ValueType>(14695981039346656037ull);
  return p;
}

} // namespace zisc

#endif // ZISC_FNV_1A_HASH_ENGINE_INL_HPP
