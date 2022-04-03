/*!
  \file math_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_TEST_HPP
#define ZISC_MATH_TEST_HPP

// Standard C++ library
#include <array>
#include <bitset>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/binary_serializer.hpp"
#include "zisc/bit.hpp"
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/math/math.hpp"


/*!
  \brief No brief description

  No detailed description.

  \tparam Float No description.
  */
template <std::floating_point Float>
struct MathTestResult
{
  std::size_t num_of_trials = 0;
  std::size_t num_of_outliers_ = 0;
  Float total_ulp_diff_ = static_cast<Float>(0.0);
  Float compensation_ = static_cast<Float>(0.0);
  Float max_ulp_diff_ = static_cast<Float>(0.0);
  bool fatal_outlier_ = false;
  bool fatal_inf_ = false;
  bool fatal_nan_ = false;
  [[maybe_unused]] zisc::Padding<sizeof(Float) - 3> pad_;

  Float averageUlpDiff() const noexcept
  {
    const Float total = total_ulp_diff_ + compensation_;
    const Float average = (0 < num_of_outliers_)
        ? total / static_cast<Float>(num_of_outliers_)
        : static_cast<Float>(0);
    return average;
  }

  void print() noexcept
  {
    std::cout   << "--      num of trials  : " << num_of_trials << std::endl;
    if (0 < num_of_outliers_) {
      const Float average = averageUlpDiff();
      std::cout << "--      num of outliers: " << num_of_outliers_ << std::endl;
      std::cout << "-- avg outlier ulp diff: " << std::scientific << average
                << std::endl;
    }
    std::cout   << "--         max ulp diff: " << std::scientific << max_ulp_diff_
                << std::endl;
  }

  void check(std::string_view name) noexcept
  {
    ASSERT_FALSE(fatal_nan_) << name.data() << " has unexpected NaN.";
    ASSERT_FALSE(fatal_inf_) << name.data() << " has unexpected Inf.";
    ASSERT_FALSE(fatal_outlier_) << name.data() << " has unexpected outlier.";
    ASSERT_FALSE(0 < num_of_outliers_) << name.data() << " has outliers.";
  }
};

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::floating_point Float> inline
Float calcRoughUlpDistance(const Float lhs, const Float rhs) noexcept
{
  constexpr Float eps = std::numeric_limits<Float>::epsilon();
  const Float ulps = std::fabs(lhs - rhs) / (eps * std::fabs(lhs + rhs));
  return ulps;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <std::floating_point Float> inline
Float calcUlpDistance(const Float lhs, const Float rhs) noexcept
{
  using BitT = std::conditional_t<sizeof(Float) == 4, zisc::uint32b, zisc::uint64b>;
  BitT l = zisc::bit_cast<BitT>(std::fabs(lhs));
  BitT r = zisc::bit_cast<BitT>(std::fabs(rhs));
  if (l < r)
    std::swap(l, r);
  const std::size_t ulps = l - r;
  return static_cast<Float>(ulps);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \tparam tolerance_ulps No description.
  \param [in] expected No description.
  \param [in] value No description.
  \param [out] result No description.
  \return No description
  */
template <std::floating_point Float, int tolerance_ulps = 1> inline
bool testMathFloat(const Float expected,
                   const Float value, 
                   MathTestResult<Float>* result) noexcept
{
  ++result->num_of_trials;
  // Nan
  if (std::isnan(expected) && std::isnan(value))
    return true;
  // Check if either expected or value is NaN
  if (std::isnan(expected) || std::isnan(value)) {
    result->fatal_nan_ = true;
    return false;
  }

  // Check sign
  constexpr Float one = static_cast<Float>(1.0);
  const Float sign = std::copysign(one, expected) * std::copysign(one, value);
  if (sign != one) {
    result->fatal_outlier_ = true;
    return false;
  }

  // Inf
  if (std::isinf(expected) && std::isinf(value))
    return true;
  // Check if either expected or value is Inf
  if (std::isinf(expected) || std::isinf(value)) {
    result->fatal_inf_ = true;
    return false;
  }

  // Finite
  if ((expected + value) == static_cast<Float>(0.0))
    return true;
  bool flag = false;
  // Compute rough UPL distance
  const Float rough_ulps = ::calcRoughUlpDistance(expected, value);
  const int rough_tolerance = (std::max)(std::numeric_limits<Float>::digits,
                                         tolerance_ulps);
  flag = rough_ulps <= rough_tolerance;
  if (flag) {
    const Float ulps = ::calcUlpDistance(expected, value);
    flag = ulps <= tolerance_ulps;
    if (!flag) {
      result->total_ulp_diff_ = zisc::Math::add(result->total_ulp_diff_,
                                                ulps,
                                                &result->compensation_);
      ++result->num_of_outliers_;
    }
    result->max_ulp_diff_ = (std::max)(result->max_ulp_diff_, ulps);
  }
  else {
    result->max_ulp_diff_ = rough_ulps;
    result->fatal_outlier_ = true;
  }
  return flag;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \param [in] file_path No description.
  \return No description
  */
template <typename Float> inline
std::vector<Float> loadXList(std::string_view file_path) noexcept
{
  std::ifstream reference_file{file_path.data(), std::ios_base::binary};

  zisc::int32b n = 0;
  zisc::BSerializer::read(&n, &reference_file);

  std::vector<Float> x_list;
  x_list.resize(n);
  zisc::BSerializer::read(x_list.data(), &reference_file, sizeof(Float) * n);

  return x_list;
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadPowXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xpowf_reference.bin"}
    : std::string_view{"resources/math_xpowd_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadPositivePowXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xpositivepowf_reference.bin"}
    : std::string_view{"resources/math_xpositivepowd_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadAllXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xallf_reference.bin"}
    : std::string_view{"resources/math_xalld_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
std::vector<Float> loadAllHalfXList() noexcept
{
  auto file_path = (sizeof(Float) == 2)
    ? std::string_view{"resources/math_xall_halfh_reference.bin"}
    : std::string_view{"resources/math_xall_halff_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadAllSubnormalXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xall_subf_reference.bin"}
    : std::string_view{"resources/math_xall_subd_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <typename Float> inline
std::vector<Float> loadAllHalfSubnormalXList() noexcept
{
  auto file_path = (sizeof(Float) == 2)
    ? std::string_view{"resources/math_xall_half_subh_reference.bin"}
    : std::string_view{"resources/math_xall_half_subf_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadPositiveXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xpositivef_reference.bin"}
    : std::string_view{"resources/math_xpositived_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Float No description.
  \return No description
  */
template <std::floating_point Float> inline
std::vector<Float> loadPositiveSubnormalXList() noexcept
{
  auto file_path = (sizeof(Float) == 4)
    ? std::string_view{"resources/math_xpositive_subf_reference.bin"}
    : std::string_view{"resources/math_xpositive_subd_reference.bin"};
  return loadXList<Float>(file_path);
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in] x No description.
  \return No description
  */
template <zisc::TriviallyCopyable Type> inline
std::bitset<8 * sizeof(Type)> toBitset(const Type x) noexcept
{
  using BitType = std::conditional_t<sizeof(Type) == 1, zisc::uint8b,
                  std::conditional_t<sizeof(Type) == 2, zisc::uint16b,
                  std::conditional_t<sizeof(Type) == 4, zisc::uint32b,
                                                        zisc::uint64b>>>;
  const auto u = zisc::bit_cast<BitType>(x);
  const std::bitset<8 * sizeof(Type)> bits{zisc::cast<unsigned long long>(u)};
  return bits;
}

/*!
  \details No detailed description

  \tparam Type No description.
  \param [in,out] s No description.
  \return No description
  */
template <zisc::TriviallyCopyable Type> inline
Type readValue(std::istream* s) noexcept
{
  Type t{};
  zisc::BSerializer::read(std::addressof(t), s);
  return t;
}

#endif // ZISC_MATH_TEST_HPP
