/*!
  \file basic_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_REFERENCE_BASIC_TEST_HPP
#define ZISC_MATH_REFERENCE_BASIC_TEST_HPP

// Standard C++ library
#include <concepts>
#include <limits>
#include <ostream>
#include <vector>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/binary_serializer.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/ieee_754_binary.hpp"
#include "zisc/utility.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"
#include "zisc/math/math.hpp"

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr Float makeNormal(const Float x) noexcept
{
  using zisc::cast;
  // Sign
  const Float sign = zisc::signbit(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  // Exponent
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  const auto k = cast<Float>(2.0) * zisc::abs(x) - cast<Float>(1.0);
  constexpr auto bias = cast<int>(Binary::exponentBias());
  const int expt = zisc::clamp(cast<int>(k * cast<Float>(bias)), -bias + 1, bias);
  // Significand
  using BitT = typename Binary::BitT;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;
  const BitT h = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float s = zisc::mapTo01<Float>(h);
  // Make a normal float
  using Limits = std::numeric_limits<Float>;
  Float y = s * zisc::pow(cast<Float>(2), expt);
  y = sign * zisc::clamp(y, (Limits::min)(), (Limits::max)());
  return y;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr Float makeSubnormal(const Float x) noexcept
{
  using zisc::cast;
  using Limits = std::numeric_limits<Float>;
  constexpr Float subnormal_max = (Limits::min)() - Limits::denorm_min();
  static_assert(subnormal_max < (Limits::min)());
  // Sign
  const Float sign = zisc::signbit(x) ? cast<Float>(-1.0) : cast<Float>(1.0);
  // Significand
  Float subnormal = zisc::abs(x) * subnormal_max;
  subnormal = sign * zisc::clamp(subnormal, Limits::denorm_min(), subnormal_max);
  return subnormal;
}

template <std::floating_point Float>
constexpr Float kPowScale = static_cast<Float>(16);

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] data No description.
  \param [out] output No description.
  */
template <typename Type> inline
void write(const Type* data, std::ostream* output) noexcept
{
  if (output)
    zisc::BSerializer::write(data, output);
}

/*!
  \details Sample 'kN' float numbers in regularly spaced in [-1, 1] range and scaled by 's'

  \tparam Float No description.
  \tparam kN No description.
  \param [in] s No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 100'000> inline
std::vector<Float> makeXList(const Float s, std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = s * u;
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

/*!
  \details Sample 'kN' float numbers in regularly spaced in log scale in all float range

  \tparam Float No description.
  \tparam kN No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 200'000> inline
std::vector<Float> makeAllXList(std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = makeNormal(u);
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kN No description.
  \param [in] s No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 100'000> inline
std::vector<Float> makePositiveXList(const Float s, std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = s * u;
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kN No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 200'000> inline
std::vector<Float> makePositiveXList(std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr zisc::int32b end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = makeNormal(u);
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kN No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 200> inline
std::vector<Float> makeAllSubnormalXList(std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr int end = n / 2;
  x_list.reserve(n);
  for (int i = -end; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = makeSubnormal(u);
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam kN No description.
  \param [out] output No description.
  \return No description
  */
template <std::floating_point Float, zisc::int32b kN = 200> inline
std::vector<Float> makePositiveSubnormalXList(std::ostream* output = nullptr) noexcept
{
  const zisc::int32b n = kN;
  write(&n, output);

  std::vector<Float> x_list;
  constexpr int end = n;
  x_list.reserve(n);
  for (int i = 0; i < end; ++i) {
    const Float u = zisc::cast<Float>(i) / zisc::cast<Float>(end);
    const Float x = makeSubnormal(u);
    write(&x, output);
    x_list.push_back(x);
  }
  return x_list;
}

template <typename Func, typename Float>
concept Func1 = zisc::InvocableR<Func, Float, Float>;

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam Func No description.
  \param [in] func No description.
  \param [in] x_list No description.
  \param [in] x_spec_list No description.
  \param [out] output No description.
  */
template <std::floating_point Float, Func1<Float> Func> inline
void testF1(const Func func,
            const std::vector<Float>& x_list,
            const std::vector<Float>& x_spec_list,
            std::ostream* output) noexcept
{
  for (const Float x : x_list) {
    const Float y = func(x);
    write(&y, output);
  }
  // Special case
  const auto n_spec = static_cast<zisc::int32b>(x_spec_list.size());
  write(&n_spec, output);
  for (const Float x : x_spec_list) {
    write(&x, output);
    const Float y = func(x);
    write(&y, output);
  }
}

/*!
  \brief No brief description

  No detailed description.

  \tparam Float No description.
  */
template <std::floating_point Float>
struct F2
{
  Float x_ = static_cast<Float>(0);
  Float y_ = static_cast<Float>(0);
};

template <typename Func, typename Float>
concept Func2 = zisc::InvocableR<Func, Float, F2<Float>>;

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam Func No description.
  \param [in] func No description.
  \param [in] x_list No description.
  \param [in] x_spec_list No description.
  \param [out] output No description.
  */
template <std::floating_point Float, Func2<Float> Func> inline
void testF2(const Func func,
            const std::vector<F2<Float>>& x_list,
            const std::vector<F2<Float>>& x_spec_list,
            std::ostream* output) noexcept
{
  static_assert(sizeof(F2<Float>) == 2 * sizeof(Float));
  for (const F2<Float> x : x_list) {
    const Float y = func(x);
    write(&y, output);
  }
  // Special case
  const auto n_spec = static_cast<zisc::int32b>(x_spec_list.size());
  write(&n_spec, output);
  for (const F2<Float> x : x_spec_list) {
    write(&x, output);
    const Float y = func(x);
    write(&y, output);
  }
}

/*!
  \brief No brief description

  No detailed description.

  \tparam Float No description.
  */
template <std::floating_point Float>
struct F3
{
  Float x_ = static_cast<Float>(0);
  Float y_ = static_cast<Float>(0);
  Float z_ = static_cast<Float>(0);
};

template <typename Func, typename Float>
concept Func3 = zisc::InvocableR<Func, Float, F3<Float>>;

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam Func No description.
  \param [in] func No description.
  \param [in] x_list No description.
  \param [in] x_spec_list No description.
  \param [out] output No description.
  */
template <std::floating_point Float, Func3<Float> Func> inline
void testF3(const Func func,
            const std::vector<F3<Float>>& x_list,
            const std::vector<F3<Float>>& x_spec_list,
            std::ostream* output) noexcept
{
  static_assert(sizeof(F3<Float>) == 3 * sizeof(Float));
  for (const F3<Float> x : x_list) {
    const Float y = func(x);
    write(&y, output);
  }
  // Special case
  const auto n_spec = static_cast<zisc::int32b>(x_spec_list.size());
  write(&n_spec, output);
  for (const F3<Float> x : x_spec_list) {
    write(&x, output);
    const Float y = func(x);
    write(&y, output);
  }
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr F3<Float> makeFmaInput(const Float x) noexcept
{
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  using BitT = typename Binary::BitT;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;

  constexpr auto t = zisc::cast<Float>(2.0);
  constexpr auto o = zisc::cast<Float>(1.0);
  constexpr auto k = zisc::cast<Float>(0.85);

  const Float x1 = makeNormal(k * x);

  const BitT u2 = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float x2 = makeNormal(k * (t * zisc::mapTo01<Float>(u2) - o));

  const BitT u3 = Fnv1aHash::hash(u2);
  const Float x3 = makeNormal(k * (t * zisc::mapTo01<Float>(u3) - o));

  return F3<Float>{x1, x2, x3};
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] x No description.
  \return No description
  */
template <std::floating_point Float> inline
constexpr F2<Float> makeFmodInput(const Float x) noexcept
{
  using Binary = zisc::BinaryFromBytes<sizeof(Float)>;
  using BitT = typename Binary::BitT;
  using Fnv1aHash = zisc::Fnv1aHashEngine<BitT>;

  constexpr auto t = zisc::cast<Float>(2.0);
  constexpr auto o = zisc::cast<Float>(1.0);

  const Float x1 = makeNormal(x);

  const BitT u2 = Fnv1aHash::hash(zisc::bit_cast<BitT>(x));
  const Float x2 = makeNormal(t * zisc::mapTo01<Float>(u2) - o);

  return F2<Float>{x1, x2};
}

#endif // ZISC_MATH_REFERENCE_BASIC_TEST_HPP
