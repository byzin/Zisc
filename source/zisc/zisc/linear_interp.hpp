/*!
  \file linear_interp.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LINEAR_INTERP_HPP
#define ZISC_LINEAR_INTERP_HPP

// Standard C++ library
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
// Zisc
#include "non_copyable.hpp"
#include "std_memory_resource.hpp"

namespace zisc {

/*!
  \brief Linear interpolation class

  No detailed description.

  \tparam Float No description.
  \todo ensure to be sorted.
  \todo Add tests
  */
template <typename Float>
class LinearInterp : private NonCopyable<LinearInterp<Float>>
{
 public:
  using FloatType = Float;


  //! Create an empty instance
  LinearInterp(std::pmr::memory_resource* mem_resource) noexcept;

  //! Move instance data
  LinearInterp(LinearInterp&& other) noexcept;


  //! Move instance data
  LinearInterp& operator=(LinearInterp&& other) noexcept;


  //! Interpolate 
  Float operator()(const Float x) const noexcept;


  //! Add data
  template <typename XType, typename YType>
  void add(const XType x, const YType y) noexcept;

  //! Return the number of elements that can be held in allocated storage
  std::size_t capacity() const noexcept;

  //! Clear all data
  void clear() noexcept;

  //! Check whether the data at x exists
  bool exists(const Float x) const noexcept;

  //! Interpolate 
  Float interpolate(const Float x) const noexcept;

  //! Reserve storage
  void setCapacity(const std::size_t cap) noexcept;

 private:
  static_assert(std::is_floating_point<Float>::value, 
                "Float isn't floating point type.");

  using Pair = std::tuple<Float, Float>;
  using Iterator = typename pmr::vector<Pair>::iterator;
  using ConstIterator = typename pmr::vector<Pair>::const_iterator;


  //! Check whether the data at x exists
  bool exists(const Float x, const ConstIterator& position) const noexcept;

  //!
  Iterator lowerBound(const Float x) noexcept;

  //!
  ConstIterator lowerBound(const Float x) const noexcept;


  pmr::vector<Pair> data_;
};

} // namespace zisc

#include "linear_interp-inl.hpp"

#endif // ZISC_LINEAR_INTERP_HPP
