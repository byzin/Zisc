/*!
  \file math.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2023 Sho Ikeda
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
  static constexpr auto pi() noexcept -> Float;

  // Basic operations

  //! Add two values with minimizing the loss of significance
  template <std::floating_point Float>
  static constexpr auto add(const Float lhs,
                            const Float rhs,
                            Float* compensation = nullptr) noexcept -> Float;

  //! Fused multiply-add operation
  template <std::floating_point Float>
  static constexpr auto fma(const Float x, const Float y, const Float z) noexcept -> Float;

  //! Calculate remainder of the floating point division operation
  template <std::floating_point Float>
  static constexpr auto fmod(const Float x, const Float y) noexcept -> Float;

  // Power functions

  //! Raise a number to the given power
  template <Arithmetic Arith, Integer Int>
  static constexpr auto pow(Arith base, Int exponent) noexcept -> Arith;

  //! Raise a number to the given power
  template <Arithmetic Arith, std::floating_point Float>
  static constexpr auto pow(const Arith base, const Float exponent) noexcept -> Arith;

  //! Calculate the square root
  template <std::floating_point Float>
  static constexpr auto sqrt(Float x) noexcept -> Float;

  //! Calculate the square root
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto sqrt(const Int x) noexcept -> Float;

  //! Calculate the cubic root
  template <std::floating_point Float>
  static constexpr auto cbrt(const Float x) noexcept -> Float;

  //! Calculate the cubic root
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto cbrt(const Int x) noexcept -> Float;

  // Exponential functions

  //! Calculate e raised to the given power
  template <std::floating_point Float>
  static constexpr auto exp(const Float x) noexcept -> Float;

  //! Calculate e raised to the given power
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto exp(const Int x) noexcept -> Float;

  //! Calculate 2 raised to the given power
  template <std::floating_point Float>
  static constexpr auto exp2(const Float x) noexcept -> Float;

  //! Calculate 2 raised to the given power
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto exp2(const Int x) noexcept -> Float;

  //! Calculate the natural logarithm
  template <std::floating_point Float>
  static constexpr auto log(const Float x) noexcept -> Float;

  //! Calculate the natural logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto log(const Int x) noexcept -> Float;

  //! Calculate the binary logarithm
  template <std::floating_point Float>
  static constexpr auto log2(const Float x) noexcept -> Float;

  //! Calculate the binary logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto log2(const Int x) noexcept -> Float;

  //! Calculate the common logarithm
  template <std::floating_point Float>
  static constexpr auto log10(const Float x) noexcept -> Float;

  //! Calculate the common logarithm
  template <std::floating_point Float = double, Integer Int>
  static constexpr auto log10(const Int x) noexcept -> Float;

  // Trigonometric functions

  //! Calculate sin(theta)
  template <std::floating_point Float>
  static constexpr auto sin(const Float x) noexcept -> Float;

  //! Calculate cos(theta)
  template <std::floating_point Float>
  static constexpr auto cos(const Float x) noexcept -> Float;

  //! Calculate tan(theta)
  template <std::floating_point Float>
  static constexpr auto tan(const Float x) noexcept -> Float;

  //! Calculate asin(x)
  template <std::floating_point Float>
  static constexpr auto asin(const Float x) noexcept -> Float;

  //! Calculate acos(x)
  template <std::floating_point Float>
  static constexpr auto acos(const Float x) noexcept -> Float;

  //! Calculate atan(x)
  template <std::floating_point Float>
  static constexpr auto atan(const Float x) noexcept -> Float;

  //! Calculate atan(x)
  template <std::floating_point Float>
  static constexpr auto atan2(const Float y, const Float x) noexcept -> Float;

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
     public:
      constexpr F2() noexcept = default;
      explicit constexpr F2(const Float x) noexcept : x_{x} {}
      constexpr F2(const Float x, const Float y) noexcept : x_{x}, y_{y} {}
      constexpr auto x() noexcept -> Float& {return x_;}
      constexpr auto x() const noexcept -> const Float& {return x_;}
      constexpr auto y() noexcept -> Float& {return y_;}
      constexpr auto y() const noexcept -> const Float& {return y_;}

     private:
      Float x_ = cast<Float>(0.0);
      Float y_ = cast<Float>(0.0);
    };

    //!
    template <std::floating_point Float>
    static constexpr auto add(const Float x, const Float y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add(const Float x, const F2<Float> y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add(const F2<Float> x, const Float y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add(const F2<Float> x, const F2<Float> y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add2(const Float x, const Float y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add2(const Float x, const F2<Float> y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add2(const F2<Float> x, const Float y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto add2(const F2<Float> x, const F2<Float> y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto div(const F2<Float> x, const F2<Float> y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto mul(const Float x, const Float y) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto mul(const F2<Float> x, const Float y) noexcept -> F2<Float>;

    //!
    template <typename ReturnT, std::floating_point Float>
    static constexpr auto mul(const F2<Float> x, const F2<Float> y) noexcept -> ReturnT;

    //!
    template <std::floating_point Float>
    static constexpr auto normalize(const F2<Float> x) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto rec(const Float x) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto rec(const F2<Float> x) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto scale(const F2<Float> x, const Float s) noexcept -> F2<Float>;

    //!
    template <typename ReturnT, std::floating_point Float>
    static constexpr auto squ(const F2<Float> x) noexcept -> ReturnT;

    // Constants

    struct ExpDConstants
    {
      static constexpr auto get(const std::size_t index) noexcept -> double;
    };

    struct LogDConstants
    {
      static constexpr auto get(const std::size_t index) noexcept -> double;
    };

    //!
    template <std::floating_point Float>
    static constexpr auto ln2() noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto l2l() noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto l2u() noexcept -> Float;

    // Poly

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly2(const Float x) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly3(const Float x,
                                const Float x2) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly4(const Float x,
                                const Float x2) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly5(const Float x,
                                const Float x2,
                                const Float x4) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly6(const Float x,
                                const Float x2,
                                const Float x4) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly7(const Float x,
                                const Float x2,
                                const Float x4) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly8(const Float x,
                                const Float x2,
                                const Float x4) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly9(const Float x,
                                const Float x2,
                                const Float x4,
                                const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly10(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly11(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly12(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly13(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly14(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly15(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly16(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly17(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8,
                                 const Float x16) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly18(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8,
                                 const Float x16) noexcept -> Float;

    //!
    template <typename Constans, std::size_t coffset, std::floating_point Float>
    static constexpr auto poly19(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8,
                                 const Float x16) noexcept -> Float;

    // Math

    //!
    template <std::floating_point Float>
    static constexpr auto abs(const Float x) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto copysign(const Float x, const Float y) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto exp(const F2<Float> x) noexcept -> Float;

    //!
    template <std::floating_point Float, Arithmetic Arith1, Arithmetic Arith2>
    static constexpr auto fvalue(const Arith1 v1, const Arith2 v2) noexcept;

    //!
    template <std::floating_point Float>
    static constexpr auto ilogb2(const Float x) noexcept -> IntT<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto isInf(const Float x) noexcept -> bool;

    //!
    template <std::floating_point Float>
    static constexpr auto isInt(const Float x) noexcept -> bool;

    //!
    template <std::floating_point Float>
    static constexpr auto isNan(const Float x) noexcept -> bool;

    //!
    template <std::floating_point Float>
    static constexpr auto isOdd(const Float x) noexcept -> bool;

    //!
    template <std::floating_point Float>
    static constexpr auto ldexp(Float x, int e) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto ldexp3(const Float x, const IntT<Float> e) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto log(Float x) noexcept -> F2<Float>;

    //!
    template <std::floating_point Float>
    static constexpr auto mla(const Float x, const Float y, const Float z) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto mulsign(const Float x, const Float y) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto rint(const Float x) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto sign(const Float x) noexcept -> Float;

    //!
    template <std::floating_point Float>
    static constexpr auto upper(const Float x) noexcept -> Float;
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
constexpr auto fma(const Float x, const Float y, const Float z) noexcept -> Float;

//! Calculate remainder of the floating point division operation
template <std::floating_point Float>
constexpr auto fmod(const Float x, const Float y) noexcept -> Float;

// Power functions

//! Raise a number to the given power
template <Arithmetic Arith1, Arithmetic Arith2>
constexpr auto pow(const Arith1 base, const Arith2 exponent) noexcept -> Arith1;

//! Calculate the square root
template <std::floating_point Float>
constexpr auto sqrt(const Float x) noexcept -> Float;

//! Calculate the square root
template <std::floating_point Float = double, Integer Int>
constexpr auto sqrt(const Int x) noexcept -> Float;

//! Calculate the cubic root
template <std::floating_point Float>
constexpr auto cbrt(const Float x) noexcept -> Float;

//! Calculate the cubic root
template <std::floating_point Float = double, Integer Int>
constexpr auto cbrt(const Int x) noexcept -> Float;

// Exponential functions

//! Calculate e raised to the given power
template <std::floating_point Float>
constexpr auto exp(const Float x) noexcept -> Float;

//! Calculate e raised to the given power
template <std::floating_point Float = double, Integer Int>
constexpr auto exp(const Int x) noexcept -> Float;

//! Calculate 2 raised to the given power
template <std::floating_point Float>
constexpr auto exp2(const Float x) noexcept -> Float;

//! Calculate 2 raised to the given power
template <std::floating_point Float = double, Integer Int>
constexpr auto exp2(const Int x) noexcept -> Float;

//! Calculate the natural logarithm
template <std::floating_point Float>
constexpr auto log(const Float x) noexcept -> Float;

//! Calculate the natural logarithm
template <std::floating_point Float = double, Integer Int>
constexpr auto log(const Int x) noexcept -> Float;

//! Calculate the binary logarithm
template <std::floating_point Float>
constexpr auto log2(const Float x) noexcept -> Float;

//! Calculate the binary logarithm
template <std::floating_point Float = double, Integer Int>
constexpr auto log2(const Int x) noexcept -> Float;

//! Calculate the common logarithm
template <std::floating_point Float>
constexpr auto log10(const Float x) noexcept -> Float;

//! Calculate the common logarithm
template <std::floating_point Float = double, Integer Int>
constexpr auto log10(const Int x) noexcept -> Float;

// Trigonometric functions

//! Calculate sin(theta)
template <std::floating_point Float>
constexpr auto sin(const Float x) noexcept -> Float;

//! Calculate cos(theta)
template <std::floating_point Float>
constexpr auto cos(const Float x) noexcept -> Float;

//! Calculate tan(theta)
template <std::floating_point Float>
constexpr auto tan(const Float x) noexcept -> Float;

//! Calculate asin(x)
template <std::floating_point Float>
constexpr auto asin(const Float x) noexcept -> Float;

//! Calculate acos(x)
template <std::floating_point Float>
constexpr auto acos(const Float x) noexcept -> Float;

//! Calculate atan(x)
template <std::floating_point Float>
constexpr auto atan(const Float x) noexcept -> Float;

//! Calculate atan(x)
template <std::floating_point Float>
constexpr auto atan2(const Float y, const Float x) noexcept -> Float;

} // namespace zisc

#include "math-inl.hpp"

#endif // ZISC_MATH_HPP
