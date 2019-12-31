/*!
  \file cumulative_distribution_function.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
#define ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP

// Standard C++ library
#include <cstddef>
#include <memory>
#include <vector>
// Zisc
#include "simple_memory_resource.hpp"
#include "std_memory_resource.hpp"
#include "type_traits.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename X, typename Pdf>
class CumulativeDistributionFunction
{
 public:
  using XType = X;
  using PdfType = Pdf;


  //! Create a CDF
  CumulativeDistributionFunction(
      std::pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource())
          noexcept;

  //! Create a CDF
  CumulativeDistributionFunction(
      const std::vector<XType>& x_list,
      const std::vector<PdfType>& y_list,
      std::pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Create a CDF
  CumulativeDistributionFunction(
      const pmr::vector<XType>& x_list,
      const pmr::vector<PdfType>& y_list,
      std::pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource()) noexcept;

  //! Create a CDF
  CumulativeDistributionFunction(
      pmr::vector<XType>&& x_list,
      pmr::vector<PdfType>&& y_list) noexcept;

  //! Move data
  CumulativeDistributionFunction(CumulativeDistributionFunction&& other) noexcept;


  //! Move data
  CumulativeDistributionFunction& operator=(
      CumulativeDistributionFunction&& other) noexcept;


  //! Return the x value associated with the y value
  const XType& invert(const PdfType y) const noexcept;

  //! Return the size of the lists
  std::size_t size() const noexcept;

  //! Set a CDF data
  void set(
      const std::vector<XType>& x_list,
      const std::vector<PdfType>& y_list) noexcept;

  //! Set a CDF data
  void set(
      const pmr::vector<XType>& x_list,
      const pmr::vector<PdfType>& y_list) noexcept;

  //! Set a CDF data
  void set(
      pmr::vector<XType>&& x_list,
      pmr::vector<PdfType>&& y_list) noexcept;

  //! Return the x list
  pmr::vector<XType>& xList() noexcept;

  //! Return the x list
  const pmr::vector<XType>& xList() const noexcept;

  //! Return the y list
  pmr::vector<PdfType>& yList() noexcept;

  //! Return the y list
  const pmr::vector<PdfType>& yList() const noexcept;

 private:
  static_assert(kIsFloat<PdfType>, "PdfType isn't floating point type.");


  //! Initialize the CDF
  void initCdf() noexcept;


  pmr::vector<XType> x_list_;
  pmr::vector<PdfType> y_list_;
};

} // namespace zisc

#include "cumulative_distribution_function-inl.hpp"

#endif // ZISC_CUMULATIVE_DISTRIBUTION_FUNCTION_HPP
