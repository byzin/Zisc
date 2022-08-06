/*!
  \file concurrent_map_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_CONCURRENT_MAP_TEST_HPP
#define TEST_CONCURRENT_MAP_TEST_HPP

// Standard C++ library
#include <atomic>
#include <chrono>
#include <cstddef>
#include <random>
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/structure/map.hpp"

namespace test {

/*!
  \brief No brief description

  No detailed description.
  */
class Zipfian
{
 public:
  //! Initialize a zipfian
  Zipfian(const std::size_t num_of_items, const double zipfian_const) noexcept;


  //
  zisc::uint64b operator()(const std::size_t i) const noexcept;


  //
  static constexpr std::size_t numOfItemsMax() noexcept;

  //
  static constexpr std::size_t numOfItemsMin() noexcept;

  //
  static constexpr double zipfianConst() noexcept;

 private:
  //
  double eta() const noexcept;

  //
  static double zeta(const std::size_t cur_num, const double theta) noexcept;


  std::size_t num_of_items_;
  double theta_;
  double zeta_n_;
  double eta_;
  double alpha_;
  double zeta_2_;
};

/*!
  \brief No brief description

  No detailed description.
  */
class MapTest
{
 public:
  //
  template <typename MapClass>
  static void testConcurrentThroughputOp(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const std::size_t num_of_keys,
      const std::size_t num_of_rounds,
      const zisc::uint64b sampler_seed,
      const bool use_sparse,
      const bool use_zipfian,
      const double zipfian_param,
      zisc::Map<MapClass, zisc::uint64b>* map);

  //
  template <typename MapClass>
  static void testConcurrentThroughputTime(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const std::size_t num_of_keys,
      const std::size_t num_of_rounds,
      const std::size_t update_percent,
      const zisc::int64b trial_time,
      const zisc::uint64b sampler_seed,
      const bool use_sparse,
      const bool use_zipfian,
      const double zipfian_param,
      zisc::Map<MapClass, zisc::uint64b>* map);


  static constexpr std::size_t kNumOfDefaultThreads = 128;
  static constexpr std::size_t kNumOfDefaultKeys = 100'000;
  static constexpr std::size_t kNumOfDefaultSamples = 20'000'000;
  static constexpr std::size_t kNumOfDefaultRounds = 4;
  static constexpr std::size_t kDefaultUpdatePercent = 50;
  static constexpr zisc::int64b kDefaultTrialTime = 10'000; // in milliseconds
  static constexpr zisc::uint64b kDefaultSamplerSeed = 123'456'789u;

 private:
  /*!
    \brief No brief description

    No detailed description.
    */
  enum class Operation : zisc::uint8b
  {
    kAdd,
    kRemove,
    kContain
  };

  using Clock = std::chrono::system_clock;


  //! Calculate the elapsed time in microseconds
  static std::chrono::microseconds calcElapsedTime(const Clock::time_point start_time,
                                                   const Clock::time_point end_time) noexcept;

  //
  static double calcMops(const std::size_t total_op,
                         const std::chrono::microseconds elapsed_time) noexcept;

  //
  static std::tuple<std::vector<zisc::uint64b>, std::vector<zisc::uint64b>>
  generateMapInputList(const std::size_t num_of_samples,
                       const std::size_t num_of_keys,
                       const bool use_sparse,
                       const bool use_zipfian,
                       const double zipfian_param,
                       std::mt19937_64& sampler);

  //
  static std::vector<Operation> generateMapOpList(
      const std::size_t num_of_samples,
      const std::size_t update_percent);

  //
  template <typename MapClass>
  static void testConcurrentThroughputTimeImpl(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const zisc::int64b trial_time,
      const std::vector<zisc::uint64b>& input_list,
      const std::vector<Operation>& op_list,
      const std::size_t i,
      zisc::Map<MapClass, zisc::uint64b>* map,
      std::vector<std::size_t>* total_list,
      std::vector<zisc::int64b>* added_list,
      std::atomic_flag* finish);
};

} /* namespace test */

#include "concurrent_map_test-inl.hpp"

#endif /* TEST_CONCURRENT_MAP_TEST_HPP */
