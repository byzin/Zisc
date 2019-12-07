/*!
  \file linear_interp.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2019 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_LINEAR_INTERP_HPP
#define ZISC_LINEAR_INTERP_HPP

// Standard C++ library
#include <list>
#include <tuple>
#include <type_traits>
#include <utility>
// Zisc
#include "non_copyable.hpp"
#include "simple_memory_resource.hpp"
#include "std_memory_resource.hpp"

namespace zisc {

/*!
 \brief Linear interpolation class
 \details
 No detailed.

 \todo ensure to be sorted.
 */
template <typename Float>
class LinearInterp : private NonCopyable<LinearInterp<Float>>
{
 public:
  using FloatType = Float;


  //! Create an empty instance
  LinearInterp(
      std::pmr::memory_resource* mem_resource = SimpleMemoryResource::sharedResource())
          noexcept;

  //! Move instance data
  LinearInterp(LinearInterp&& other) noexcept;


  //! Move instance data
  LinearInterp& operator=(LinearInterp&& other) noexcept;


  //! Interpolate 
  Float operator()(const Float x) const noexcept;


  //! Add data
  template <typename XType, typename YType>
  void add(const XType x, const YType y) noexcept;

  //! Check whether the data at x exists
  bool exists(const Float x) const noexcept;

  //! Interpolate 
  Float interpolate(const Float x) const noexcept;

 private:
  static_assert(std::is_floating_point<Float>::value, 
                "Float isn't floating point type.");

  using Pair = std::tuple<Float, Float>;
  using Iterator = typename pmr::list<Pair>::iterator;
  using ConstIterator = typename pmr::list<Pair>::const_iterator;


  //! Check whether the data at x exists
  bool exists(const Float x, const ConstIterator& position) const noexcept;

  //!
  Iterator lowerBound(const Float x) noexcept;

  //!
  ConstIterator lowerBound(const Float x) const noexcept;


  pmr::list<Pair> data_;
};

} // namespace zisc

#include "linear_interp-inl.hpp"

#endif // ZISC_LINEAR_INTERP_HPP
