/*!
  \file test.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_TEST_HPP
#define ZISC_TEST_HPP

// Standard C++ library
#include <limits>
// Zisc
#include "zisc/ieee_754_binary.hpp"
#include "zisc/math.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"

namespace {

/*!
  */
template <typename Float> inline
constexpr Float makeNormal(const Float x) noexcept
{
  using zisc::cast;
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;

  constexpr int bias = cast<int>(Binary::exponentBias());

  const Float k = cast<Float>(2.0) * zisc::abs(x) - cast<Float>(1.0);
  const Float sign = zisc::isNegative(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  const int exponent = zisc::clamp(cast<int>(k * cast<Float>(bias)), -bias+1, bias);

  const auto h = zisc::Fnv1aHash32::hash(cast<zisc::uint32b>(zisc::abs(exponent)));
  const Float s = zisc::mapTo01<Float>(h);

  Float normal = s * zisc::power(cast<Float>(2.0), exponent);
  normal = sign * zisc::clamp(normal,
                              std::numeric_limits<Float>::min(),
                              std::numeric_limits<Float>::max());
  return normal;
}

/*!
  */
template <typename Float> inline
constexpr Float makeSubnormal(const Float x) noexcept
{
  using zisc::cast;

  constexpr Float subnormal_max = std::numeric_limits<Float>::min() -
                                  std::numeric_limits<Float>::denorm_min();
  static_assert(zisc::isSubnormal(subnormal_max), "The sub_max isn't subnormal.");

  const Float sign = zisc::isNegative(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  Float subnormal = zisc::power<4>(x) * subnormal_max;
  subnormal = sign * zisc::clamp(subnormal,
                                 std::numeric_limits<Float>::denorm_min(),
                                 subnormal_max);
  return subnormal;
}

} // namespace 

#endif // ZISC_TEST_HPP
