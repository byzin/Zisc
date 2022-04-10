/*!
  \file concurrent_search_tree_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_CONCURRENT_SEARCH_TREE_TEST_HPP
#define TEST_CONCURRENT_SEARCH_TREE_TEST_HPP

// Standard C++ library
#include <chrono>
#include <cstddef>
#include <random>
#include <tuple>
#include <utility>
#include <vector>
// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/structure/search_tree.hpp"

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
class SearchTreeTest
{
 public:
  //
  template <typename SearchTreeClass>
  static void testConcurrentThroughputOp(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const std::size_t num_of_keys,
      const std::size_t num_of_rounds,
      const zisc::uint64b sampler_seed,
      const bool use_sparse,
      const bool use_zipfian,
      const double zipfian_param,
      zisc::SearchTree<SearchTreeClass>* search_tree);

  //
  template <typename SearchTreeClass>
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
      zisc::SearchTree<SearchTreeClass>* search_tree);

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
  generateSearchTreeInputList(const std::size_t num_of_keys,
                              const bool use_sparse,
                              const bool use_zipfian,
                              const double zipfian_param,
                              std::mt19937_64& sampler);

  //
  static std::vector<Operation> generateSearchTreeOpList(
      const std::size_t num_of_samples,
      const std::size_t update_percent);

  //
  template <typename SearchTreeClass>
  static void testConcurrentThroughputTimeImpl(
      const std::size_t num_of_threads,
      const std::size_t num_of_samples,
      const zisc::int64b trial_time,
      const std::vector<zisc::uint64b>& input_list,
      const std::vector<Operation>& op_list,
      const std::size_t i,
      zisc::SearchTree<SearchTreeClass>* search_tree,
      std::vector<std::size_t>* total_list,
      std::vector<zisc::int64b>* added_list,
      std::atomic_flag* finish);
};

} /* namespace test */

#include "concurrent_search_tree_test-inl.hpp"

#endif /* TEST_CONCURRENT_SEARCH_TREE_TEST_HPP */
