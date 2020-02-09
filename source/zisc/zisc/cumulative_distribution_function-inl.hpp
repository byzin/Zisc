/*!
  \file cumulative_distribution_function-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP
#define ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP

#include "cumulative_distribution_function.hpp"
// Standard C++ library
#include <cstddef>
#include <limits>
#include <vector>
#include <utility>
// Zisc
#include "algorithm.hpp"
#include "compensated_summation.hpp"
#include "error.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  */
template <typename X, typename Pdf> inline
CumulativeDistributionFunction<X, Pdf>::CumulativeDistributionFunction(
    zisc::pmr::memory_resource* mem_resource) noexcept :
        x_list_{zisc::pmr::polymorphic_allocator<XType>{mem_resource}},
        y_list_{zisc::pmr::polymorphic_allocator<PdfType>{mem_resource}}
{
}

/*!
  */
template <typename X, typename Pdf> inline
CumulativeDistributionFunction<X, Pdf>::CumulativeDistributionFunction(
    const std::vector<XType>& x_list,
    const std::vector<PdfType>& y_list,
    zisc::pmr::memory_resource* mem_resource) noexcept :
        x_list_{zisc::pmr::polymorphic_allocator<XType>{mem_resource}},
        y_list_{zisc::pmr::polymorphic_allocator<PdfType>{mem_resource}}
{
  set(x_list, y_list);
}

/*!
  */
template <typename X, typename Pdf> inline
CumulativeDistributionFunction<X, Pdf>::CumulativeDistributionFunction(
    const pmr::vector<XType>& x_list,
    const pmr::vector<PdfType>& y_list,
    zisc::pmr::memory_resource* mem_resource) noexcept :
        x_list_{x_list, mem_resource},
        y_list_{y_list, mem_resource}
{
  initCdf();
}

/*!
  */
template <typename X, typename Pdf> inline
CumulativeDistributionFunction<X, Pdf>::CumulativeDistributionFunction(
    pmr::vector<XType>&& x_list,
    pmr::vector<PdfType>&& y_list) noexcept :
        x_list_{std::move(x_list)},
        y_list_{std::move(y_list)}
{
  initCdf();
}

/*!
  \details
  No detailed.
  */
template <typename X, typename Pdf> inline
CumulativeDistributionFunction<X, Pdf>::CumulativeDistributionFunction(
    CumulativeDistributionFunction&& other) noexcept :
        x_list_{std::move(other.x_list_)},
        y_list_{std::move(other.y_list_)}
{
}

/*!
  \details
  No detailed.
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::operator=(
    CumulativeDistributionFunction&& other) noexcept -> CumulativeDistributionFunction&
{
  x_list_ = std::move(other.x_list_);
  y_list_ = std::move(other.y_list_);
}

/*!
  \details
  No detailed.
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::invert(const PdfType y) const noexcept
    -> const XType&
{
  ZISC_ASSERT(isInClosedBounds(y, 0.0, 1.0), "y is out of range: [0, 1].");
  const auto position = Algorithm::searchBinaryTree(y_list_.begin(),
                                                    y_list_.end(),
                                                    y);
  const auto index = std::distance(y_list_.begin(), position);
  return x_list_[index];
}

/*!
  */
template <typename X, typename Pdf> inline
std::size_t CumulativeDistributionFunction<X, Pdf>::size() const noexcept
{
  return x_list_.size();
}

/*!
  */
template <typename X, typename Pdf> inline
void CumulativeDistributionFunction<X, Pdf>::set(
    const std::vector<XType>& x_list,
    const std::vector<PdfType>& y_list) noexcept
{
  ZISC_ASSERT(x_list.size() == y_list.size(),
              "The x and y lists aren't same length.");

  x_list_.resize(x_list.size());
  y_list_.resize(y_list.size());

  for (std::size_t index = 0; index < size(); ++index) {
    x_list_[index] = x_list[index];
    y_list_[index] = y_list[index];
  }

  initCdf();
}

/*!
  */
template <typename X, typename Pdf> inline
void CumulativeDistributionFunction<X, Pdf>::set(
    const pmr::vector<XType>& x_list,
    const pmr::vector<PdfType>& y_list) noexcept
{
  ZISC_ASSERT(x_list.size() == y_list.size(),
              "The x and y lists aren't same length.");

  x_list_ = x_list;
  y_list_ = y_list;

  initCdf();
}

/*!
  */
template <typename X, typename Pdf> inline
void CumulativeDistributionFunction<X, Pdf>::set(
    pmr::vector<XType>&& x_list,
    pmr::vector<PdfType>&& y_list) noexcept
{
  ZISC_ASSERT(x_list.size() == y_list.size(),
              "The x and y lists aren't same length.");

  x_list_ = std::move(x_list);
  y_list_ = std::move(y_list);

  initCdf();
}

/*!
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::xList() noexcept
    -> pmr::vector<XType>&
{
  return x_list_;
}

/*!
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::xList() const noexcept
    -> const pmr::vector<XType>&
{
  return x_list_;
}

/*!
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::yList() noexcept
    -> pmr::vector<PdfType>&
{
  return y_list_;
}

/*!
  */
template <typename X, typename Pdf> inline
auto CumulativeDistributionFunction<X, Pdf>::yList() const noexcept
    -> const pmr::vector<PdfType>&
{
  return y_list_;
}

/*!
  */
template <typename X, typename Pdf> inline
void CumulativeDistributionFunction<X, Pdf>::initCdf() noexcept
{
  ZISC_ASSERT(x_list_.size() == y_list_.size(),
              "The xlist and ylist aren't same length.");

  // Calculate the CDF
  CompensatedSummation<PdfType> pdf_sum{};
  for (std::size_t index = 0; index < size(); ++index) {
    const auto pdf = y_list_[index];
    y_list_[index] = pdf_sum.get();
    pdf_sum.add(pdf);
  }
  ZISC_ASSERT(isInBounds(
      pdf_sum.get(),
      cast<PdfType>(1.0) - std::numeric_limits<PdfType>::epsilon(),
      cast<PdfType>(1.0) + std::numeric_limits<PdfType>::epsilon()),
      "The sum of the pdf list isn't 1.");

  // Make CDF arrays
  Algorithm::toBinaryTree(x_list_.begin(), x_list_.end());
  Algorithm::toBinaryTree(y_list_.begin(), y_list_.end());
}

} // namespace zisc

#endif // ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_INL_HPP
