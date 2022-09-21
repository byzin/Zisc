/*!
  \file concurrent_map_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#include "concurrent_map_test.hpp"
// Standard C++ library
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <tuple>
#include <utility>
#include <vector>
#include <thread>
// Zisc
#include "zisc/algorithm.hpp"
#include "zisc/error.hpp"
#include "zisc/zisc_config.hpp"
#include "zisc/hash/fnv_1a_hash_engine.hpp"
#include "zisc/math/math.hpp"
#include "zisc/structure/map.hpp"

namespace test {

/*!
  \details No detailed description

  \param [in] num_of_items No description.
  \param [in] zipfian_const No description.
  */
Zipfian::Zipfian(const std::size_t num_of_items, const double zipfian_const) noexcept :
    num_of_items_{num_of_items},
    theta_{zipfian_const}
{
  ZISC_ASSERT(zisc::isInBounds(num_of_items, numOfItemsMin(), numOfItemsMax()), "");
  zeta_2_ = zeta(2, theta_);
  zeta_n_ = zeta(num_of_items, theta_);
  alpha_ = 1.0 / (1.0 - theta_);
  eta_ = eta();
}

/*!
  \details No detailed description

  \param [in] i No description.
  \return No description
  */
zisc::uint64b Zipfian::operator()(const std::size_t i) const noexcept
{
  const zisc::uint64b r = zisc::Fnv1aHash64::hash(zisc::cast<zisc::uint64b>(i));
  const zisc::uint64b m = (std::numeric_limits<zisc::uint64b>::max)();
  const double u = zisc::cast<double>(r) / zisc::cast<double>(m);
  const double uz = u * zeta_n_;
  const double result =
      (uz < 1.0)
          ? 0 :
      (uz < 1.0 + std::pow(0.5, theta_))
          ? 1
          : std::lround(zisc::cast<double>(num_of_items_ - 1) * std::pow(eta_ * u - eta_ + 1.0, alpha_));
  return result;
}

double Zipfian::eta() const noexcept
{
  const double e =
      (1.0 - std::pow(2.0 / zisc::cast<double>(num_of_items_), 1.0 - theta_)) /
      (1.0 - zeta_2_ / zeta_n_);
  return e;
}

double Zipfian::zeta(const std::size_t cur_num, const double theta) noexcept
{
  double sum = 0.0;
  double compensation = 0.0;
  for (std::size_t i = 0; i < cur_num; ++i) {
    const double v = 1.0 / std::pow(zisc::cast<double>(i + 1), theta);
    sum = zisc::Math::add(sum, v, &compensation);
  }
  return sum + compensation;
}

/*!
  \details No detailed description

  \param [in] start_time No description.
  \param [in] end_time No description.
  \return No description
  */
std::chrono::microseconds MapTest::calcElapsedTime(
    const Clock::time_point start_time,
    const Clock::time_point end_time) noexcept
{
  const Clock::duration elapsed_time = end_time - start_time;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
}

/*!
  \details No detailed description

  \param [in] total_op No description.
  \param [in] elapsed_time No description.
  \return No description
  */
double MapTest::calcMops(const std::size_t total_op,
                         const std::chrono::microseconds elapsed_time) noexcept
{
  const double mops = zisc::cast<double>(total_op) /
                      zisc::cast<double>(elapsed_time.count());
  return mops;
}

/*!
  \details No detailed description

  \param [in] num_of_keys No description.
  \param [in] use_sparse No description.
  \param [in] use_zipfian No description.
  \param [in] zipfian_param No description.
  \return No description
  */
std::tuple<std::vector<zisc::uint64b>, std::vector<zisc::uint64b>>
MapTest::generateMapInputList(const std::size_t num_of_samples,
                              const std::size_t num_of_keys,
                              const bool use_sparse,
                              const bool use_zipfian,
                              const double zipfian_param,
                              std::mt19937_64& sampler)
{
  using zisc::uint64b;
  std::vector<uint64b> sources;
  if (use_sparse) {
    const auto n = zisc::cast<std::size_t>(1.2 * zisc::cast<double>(num_of_keys));
    sources.resize(n);
    std::iota(sources.begin(), sources.end(), 0);
    std::transform(sources.begin(), sources.end(), sources.begin(),
                   zisc::Fnv1aHash64::hash<uint64b>);
    std::sort(sources.begin(), sources.end());
    auto last = std::unique(sources.begin(), sources.end());
    sources.erase(last, sources.end());
    ZISC_ASSERT(num_of_keys <= sources.size(), "The number of source isn't enough.");
    std::shuffle(sources.begin(), sources.end(), sampler);
    sources.erase(sources.begin() + num_of_keys, sources.end());
    std::for_each(sources.begin(), sources.end(), [](uint64b& in){++in;});
  }
  else {
    sources.resize(num_of_keys);
    std::iota(sources.begin(), sources.end(), 1);
    std::shuffle(sources.begin(), sources.end(), sampler);
  }

  std::vector<uint64b> inputs;
  inputs.resize(num_of_samples);
  std::iota(inputs.begin(), inputs.end(), 0);
  if (use_zipfian) {
    const std::size_t n = sources.size();
    const Zipfian z{n, zipfian_param};
    std::for_each(inputs.begin(), inputs.end(), [&z, &sources](uint64b& in)
    {
      const std::size_t index = z(in);
      in = sources[index];
    });
  }
  else {
    std::for_each(inputs.begin(), inputs.end(), [&sources](uint64b& in)
    {
      const std::size_t n = sources.size();
      const std::size_t index = zisc::Fnv1aHash64::hash(in) % n;
      in = sources[index];
    });
  }

  ZISC_ASSERT(sources.size() == num_of_keys, "The number of source isn't enough.");
  ZISC_ASSERT(inputs.size() == num_of_samples, "The number of input isn't enough.");

  return std::make_tuple(std::move(sources), std::move(inputs));
}

/*!
  \details No detailed description

  \param [in] num_of_samples No description.
  \param [in] update_percent No description.
  \return No description
  */
auto MapTest::generateMapOpList(const std::size_t num_of_samples,
                                const std::size_t update_percent)
    -> std::vector<Operation>
{
  std::vector<Operation> op_list;
  op_list.resize(num_of_samples, Operation::kContain);
  if (0 < update_percent) {
    const zisc::uint64b count = 2 * 100 / update_percent;
    for (zisc::uint64b i = 0; i < num_of_samples; ++i) {
      const zisc::uint64b h = zisc::Fnv1aHash64::hash(num_of_samples + i);
      const Operation op = (h % count == 0) ? Operation::kAdd :
                           (h % count == 1) ? Operation::kRemove
                                            : Operation::kContain;
      op_list[i] = op;
    }
  }
  return op_list;
}

} /* namespace test */
