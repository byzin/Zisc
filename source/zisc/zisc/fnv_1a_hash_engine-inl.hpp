/*!
  \file fnv_1a_hash_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
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
#include "utility.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename ResultType> template <typename Int8> inline
constexpr ResultType Fnv1aHashEngine<ResultType>::hashValue(
    const Int8* seed,
    const std::size_t n) noexcept
{
  static_assert(sizeof(Int8) == 1, "The Int8 isn't 8bit integer.");

  ResultType x = offset();
  for (std::size_t i = 0; i < n; ++i)
    x = (x ^ cast<ResultType>(seed[i])) * prime();
  return x;
}

template <typename ResultType> inline
constexpr auto Fnv1aHashEngine<ResultType>::prime() noexcept -> ResultType
{
  if constexpr (sizeof(ResultType) == 4) {
    const ResultType p = cast<ResultType>(16777619u);
    return p;
  }
  else if constexpr (sizeof(ResultType) == 8) {
    const ResultType p = cast<ResultType>(1099511628211ull);
    return p;
  }
  else {
    static_assert(sizeof(ResultType) == 0, "Unsupported result type is specified.");
  }
}

template <typename ResultType> inline
constexpr auto Fnv1aHashEngine<ResultType>::offset() noexcept -> ResultType
{
  if constexpr (sizeof(ResultType) == 4) {
    const ResultType p = cast<ResultType>(2166136261u);
    return p;
  }
  else if constexpr (sizeof(ResultType) == 8) {
    const ResultType p = cast<ResultType>(14695981039346656037ull);
    return p;
  }
  else {
    static_assert(sizeof(ResultType) == 0, "Unsupported result type is specified.");
  }
}

} // namespace zisc

#endif // ZISC_FNV_1A_HASH_ENGINE_INL_HPP
