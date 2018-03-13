/*!
  \file fnv_1a_hash_engine-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
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

namespace inner {

template <typename ResultType> struct Fnv1aHashEngineImpl;

template <>
struct Fnv1aHashEngineImpl<uint32>
{
  static constexpr uint32 prime() noexcept
  {
    return 16777619u;
  }

  static constexpr uint32 offset() noexcept
  {
    return 2166136261u;
  }
};

template <>
struct Fnv1aHashEngineImpl<uint64>
{
  static constexpr uint64 prime() noexcept
  {
    return 1099511628211ull;
  }

  static constexpr uint64 offset() noexcept
  {
    return 14695981039346656037ull;
  }
};

} // namespace inner

/*!
  */
template <typename ResultType> template <typename Int8> inline
constexpr ResultType Fnv1aHashEngine<ResultType>::hashValue(
    const Int8* inputs,
    const std::size_t n) noexcept
{
  ResultType x = inner::Fnv1aHashEngineImpl<ResultType>::offset();
  for (std::size_t i = 0; i < n; ++i) {
    x = (x ^ cast<ResultType>(inputs[i])) *
        inner::Fnv1aHashEngineImpl<ResultType>::prime();
  }
  return x;
}

} // namespace zisc

#endif // ZISC_FNV_1A_HASH_ENGINE_INL_HPP
