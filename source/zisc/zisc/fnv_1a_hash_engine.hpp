/*!
  \file fnv_1a_hash_engine.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_FNV_1A_HASH_ENGINE_HPP
#define ZISC_FNV_1A_HASH_ENGINE_HPP

// Standard C++ library
#include <array>
#include <cstdint>
#include <cstddef>
#include <type_traits>
// Zisc
#include "hash_engine.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename ResultType>
class Fnv1aHashEngine : public HashEngine<Fnv1aHashEngine<ResultType>, ResultType>
{
 private:
  template <typename, typename> friend class HashEngine;
  //! Implementation of the hash function
  template <typename Int8>
  static constexpr ResultType hashValue(const Int8* inputs,
                                        const std::size_t n) noexcept;
};

using Fnv1aHash32 = Fnv1aHashEngine<uint32>;
using Fnv1aHash64 = Fnv1aHashEngine<uint64>;

} // namespace zisc

#include "fnv_1a_hash_engine-inl.hpp"

#endif // ZISC_FNV_1A_HASH_ENGINE_HPP
