/*!
  \file math.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_HPP
#define ZISC_MATH_HPP

// Standard C++ library
#include <concepts>
#include <cstddef>
#include <type_traits>
// Zisc
#include "zisc/concepts.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \brief No brief description

  No detailed description.
  */
class Math
{
 public:
  // Math constants

  //! Return circular constant
  template <std::floating_point Float>
  static constexpr Float pi() noexcept;

  // Basic operations

  //! Add two values with minimizing the loss of significance
  template <std::floating_point Float>
  static constexpr Float add(const Float lhs,
                             const Float rhs,
                             Float* compensation = nullptr) noexcept;

  //! Fused multiply-add operation
  template <std::floating_point Float>
  static constexpr Float fma(const Float x, const Float y, const Float z) noexcept;

  //! Calculate remainder of the floating point division operation
  template <std::floating_point Float>
  static constexpr Float fmod(const Float x, const Float y) noexcept;

  // Power functions

  //! Raise a number to the given power
  template <Arithmetic Arith, Integer Int>
  static constexpr Arith pow(Arith base, Int exponent) noexcept;

  //! Raise a number to the given power
  template <Arithmetic Arith, std::floating_point Float>
  static constexpr Arith pow(const Arith base, const Float exponent) noexcept;

  //! Calculate the square root
  template <std::floating_point Float>
  static constexpr Float sqrt(Float x) noexcept;

  //! Calculate the square root
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float sqrt(const Int x) noexcept;

  //! Calculate the cubic root
  template <std::floating_point Float>
  static constexpr Float cbrt(const Float x) noexcept;

  //! Calculate the cubic root
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float cbrt(const Int x) noexcept;

  // Exponential functions

  //! Calculate e raised to the given power
  template <std::floating_point Float>
  static constexpr Float exp(const Float x) noexcept;

  //! Calculate e raised to the given power
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float exp(const Int x) noexcept;

  //! Calculate 2 raised to the given power
  template <std::floating_point Float>
  static constexpr Float exp2(const Float x) noexcept;

  //! Calculate 2 raised to the given power
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float exp2(const Int x) noexcept;

  //! Calculate the natural logarithm
  template <std::floating_point Float>
  static constexpr Float log(const Float x) noexcept;

  //! Calculate the natural logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float log(const Int x) noexcept;

  //! Calculate the binary logarithm
  template <std::floating_point Float>
  static constexpr Float log2(const Float x) noexcept;

  //! Calculate the binary logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float log2(const Int x) noexcept;

  //! Calculate the common logarithm
  template <std::floating_point Float>
  static constexpr Float log10(const Float x) noexcept;

  //! Calculate the common logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr Float log10(const Int x) noexcept;

  // Trigonometric functions

  //! Calculate sin(theta)
  template <std::floating_point Float>
  static constexpr Float sin(const Float x) noexcept;

  //! Calculate cos(theta)
  template <std::floating_point Float>
  static constexpr Float cos(const Float x) noexcept;

  //! Calculate tan(theta)
  template <std::floating_point Float>
  static constexpr Float tan(const Float x) noexcept;

  //! Calculate asin(x)
  template <std::floating_point Float>
  static constexpr Float asin(const Float x) noexcept;

  //! Calculate acos(x)
  template <std::floating_point Float>
  static constexpr Float acos(const Float x) noexcept;

  //! Calculate atan(x)
  template <std::floating_point Float>
  static constexpr Float atan(const Float x) noexcept;

  //! Calculate atan(x)
  template <std::floating_point Float>
  static constexpr Float atan2(const Float y, const Float x) noexcept;

 private:
  template <std::floating_point Float>
  using IntT = std::conditional_t<sizeof(Float) == 4, int32b,
               std::conditional_t<sizeof(Float) == 8, int64b, int>>;
  template <std::floating_point Float>
  using BitT = std::conditional_t<sizeof(Float) == 4, uint32b,
               std::conditional_t<sizeof(Float) == 8, uint64b, int>>;

  struct Impl
  {
    // Float2

    template <std::floating_point Float>
    struct F2 
    {
      constexpr F2() {}
      constexpr F2(const Float x) : x_{x} {}
      constexpr F2(const Float x, const Float y) : x_{x}, y_{y} {}
      Float x_ = cast<Float>(0.0);
      Float y_ = cast<Float>(0.0);
    };

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add(const Float x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add(const Float x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add(const F2<Float> x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add2(const Float x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add2(const Float x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add2(const F2<Float> x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> add2(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> div(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> mul(const Float x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> mul(const F2<Float> x, const Float y) noexcept;

    //!
    template <typename Return, std::floating_point Float>
    static constexpr Return mul(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> normalize(const F2<Float> x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> rec(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> rec(const F2<Float> x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> scale(const F2<Float> x, const Float y) noexcept;

    //!
    template <typename Return, std::floating_point Float>
    static constexpr Return squ(const F2<Float> x) noexcept;

    // Constants

    struct ExpDConstants
    {
      static constexpr double get(const std::size_t index) noexcept;
    };

    struct LogDConstants
    {
      static constexpr double get(const std::size_t index) noexcept;
    };

    //!
    template <std::floating_point Float>
    static constexpr Float ln2() noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float l2l() noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float l2u() noexcept;

    // Poly

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly2(const Float x) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly3(const Float x,
                                 const Float x2) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly4(const Float x,
                                 const Float x2) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly5(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly6(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly7(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly8(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly9(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly10(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly11(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly12(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly13(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly14(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly15(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly16(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly17(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly18(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr Float poly19(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    // Math

    //!
    template <std::floating_point Float>
    static constexpr Float abs(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float copysign(const Float x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float exp(const F2<Float> x) noexcept;

    //!
    template <std::floating_point Float, Arithmetic Arith1, Arithmetic Arith2>
    static constexpr auto fvalue(const Arith1 v1, const Arith2 v2) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr IntT<Float> ilogb2(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr bool isInf(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr bool isInt(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr bool isNan(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr bool isOdd(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float ldexp(Float x, int e) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float ldexp3(const Float x, const IntT<Float> e) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr F2<Float> log(Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float mla(const Float x, const Float y, const Float z) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float mulsign(const Float x, const Float y) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float rint(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float sign(const Float x) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr Float upper(const Float x) noexcept;
  };
};

// Constant values

////! Napier's constant
//template <typename Float>
//constexpr Float kE = constant::Math::exp(static_cast<Float>(1.0));

//! Circular constant
template <std::floating_point Float>
constexpr Float kPi = Math::pi<Float>();

// Basic operations

//! Fused multiply-add operation
template <std::floating_point Float>
constexpr Float fma(const Float x, const Float y, const Float z) noexcept;

//! Calculate remainder of the floating point division operation
template <std::floating_point Float>
constexpr Float fmod(const Float x, const Float y) noexcept;

// Power functions

//! Raise a number to the given power
template <Arithmetic Arith1, Arithmetic Arith2>
constexpr Arith1 pow(const Arith1 base, const Arith2 exponent) noexcept;

//! Calculate the square root
template <std::floating_point Float>
constexpr Float sqrt(const Float x) noexcept;

//! Calculate the square root
template <std::floating_point Float = double, Integer Int>
constexpr Float sqrt(const Int x) noexcept;

//! Calculate the cubic root
template <std::floating_point Float>
constexpr Float cbrt(const Float x) noexcept;

//! Calculate the cubic root
template <std::floating_point Float = double, Integer Int>
constexpr Float cbrt(const Int x) noexcept;

// Exponential functions

//! Calculate e raised to the given power
template <std::floating_point Float>
constexpr Float exp(const Float x) noexcept;

//! Calculate e raised to the given power
template <std::floating_point Float = double, Integer Int>
constexpr Float exp(const Int x) noexcept;

//! Calculate 2 raised to the given power
template <std::floating_point Float>
constexpr Float exp2(const Float x) noexcept;

//! Calculate 2 raised to the given power
template <std::floating_point Float = double, Integer Int>
constexpr Float exp2(const Int x) noexcept;

//! Calculate the natural logarithm
template <std::floating_point Float>
constexpr Float log(const Float x) noexcept;

//! Calculate the natural logarithm
template <std::floating_point Float = double, Integer Int>
constexpr Float log(const Int x) noexcept;

//! Calculate the binary logarithm
template <std::floating_point Float>
constexpr Float log2(const Float x) noexcept;

//! Calculate the binary logarithm
template <std::floating_point Float = double, Integer Int>
constexpr Float log2(const Int x) noexcept;

//! Calculate the common logarithm
template <std::floating_point Float>
constexpr Float log10(const Float x) noexcept;

//! Calculate the common logarithm
template <std::floating_point Float = double, Integer Int>
constexpr Float log10(const Int x) noexcept;

// Trigonometric functions

//! Calculate sin(theta)
template <std::floating_point Float>
constexpr Float sin(const Float x) noexcept;

//! Calculate cos(theta)
template <std::floating_point Float>
constexpr Float cos(const Float x) noexcept;

//! Calculate tan(theta)
template <std::floating_point Float>
constexpr Float tan(const Float x) noexcept;

//! Calculate asin(x)
template <std::floating_point Float>
constexpr Float asin(const Float x) noexcept;

//! Calculate acos(x)
template <std::floating_point Float>
constexpr Float acos(const Float x) noexcept;

//! Calculate atan(x)
template <std::floating_point Float>
constexpr Float atan(const Float x) noexcept;

//! Calculate atan(x)
template <std::floating_point Float>
constexpr Float atan2(const Float y, const Float x) noexcept;

} // namespace zisc

#include "math-inl.hpp"

#endif // ZISC_MATH_HPP
