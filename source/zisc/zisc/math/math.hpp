/*!
  \file math.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATH_HPP
#define ZISC_MATH_HPP

// Standard C++ library
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
  template <FloatingPoint Float>
  static constexpr Float pi() noexcept;

//  // Basic operations
//
//  //! Compute the greatest common divisor of the integers m and n
//  template <typename Integer1, typename Integer2>
//  static constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
//                                                              Integer2 n) noexcept;
//
//  //! Compute the least common multiple of the integers m and n
//  template <typename Integer1, typename Integer2>
//  static constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
//                                                              Integer2 n) noexcept;
//
//  //!
//  template <typename Integer>
//  static constexpr Integer factorial(const Integer n) noexcept;
//
//  //!
//  template <typename Integer1, typename Integer2>
//  static constexpr std::common_type_t<Integer1, Integer2> sequence(
//      const Integer1 m,
//      const Integer2 n) noexcept;

  // Power functions

//  //! Raise a number to the given power
//  template <int kExponent, typename Arithmetic>
//  static constexpr Arithmetic power(Arithmetic base) noexcept;
//
//  //! Raise a number to the given power
//  template <typename Arithmetic, typename SignedInteger>
//  static constexpr Arithmetic power(const Arithmetic base,
//                                    const SignedInteger exponent) noexcept;

  //! Raise a number to the given power
  template <Arithmetic Arith, Integer Int>
  static constexpr Arith pow(Arith base, Int exponent) noexcept;

  //! Raise a number to the given power
  template <Arithmetic Arith, FloatingPoint Float>
  static constexpr Arith pow(const Arith base, const Float exponent) noexcept;

//  //! Calculate the square root
//  template <typename Float>
//  static Float sqrt(const Float n) noexcept;
//
//  //! Calculate the cubic root
//  template <typename Float>
//  static Float cbrt(const Float n) noexcept;
//
//  // Exponential functions
//
//  //! Calculate e raised to the given power
//  template <typename Float>
//  static Float exp(const Float n) noexcept;
//
//  //! Calculate the natural logarithm
//  template <typename Float>
//  static Float log(const Float x) noexcept;
//
//  //! Calculate the binary logarithm
//  template <typename Float>
//  static Float log2(const Float x) noexcept;
//
//  //! Calculate the common logarithm
//  template <typename Float>
//  static Float log10(const Float x) noexcept;
//
//  // Trigonometric functions
//
//  //! Calculate sin(theta)
//  template <typename Float>
//  static Float sin(const Float theta) noexcept;
//
//  //! Calculate cos(theta)
//  template <typename Float>
//  static Float cos(const Float theta) noexcept;
//
//  //! Calculate tan(theta)
//  template <typename Float>
//  static Float tan(const Float theta) noexcept;
//
//  //! Calculate asin(x)
//  template <typename Float>
//  static Float asin(const Float x) noexcept;
//
//  //! Calculate acos(x)
//  template <typename Float>
//  static Float acos(const Float x) noexcept;
//
//  //! Calculate atan(x)
//  template <typename Float>
//  static Float atan(const Float x) noexcept;
//
//  // Polynomial
//
//  //! Solve a quadratic equation
//  template <typename Float>
//  static std::tuple<std::array<Float, 2>, uint> solveQuadratic(
//      const Float a, 
//      const Float b, 
//      const Float c) noexcept;
//
//  //! Solve a cubic equation
//  template <typename Float>
//  static Float solveCubicOne(const Float b, 
//                             const Float c, 
//                             const Float d) noexcept;
//
//  //! Solve a cubic equation
//  template <typename Float>
//  static Float solveCubicOne(const Float a, 
//                             const Float b, 
//                             const Float c, 
//                             const Float d) noexcept;
//
//  //! Solve a quartic equation
//  template <typename Float>
//  static std::tuple<std::array<Float, 4>, uint> solveQuartic(
//      const Float a, 
//      const Float b, 
//      const Float c, 
//      const Float d, 
//      const Float e) noexcept;
//
//  class Stl
//  {
//   public:
//    //! Raise a number to the given power
//    template <typename Float>
//    static Float pow(const Float base, const Float exponent) noexcept;
//
//    //! Calculate the square root
//    template <typename Float>
//    static Float sqrt(const Float n) noexcept;
//
//    //! Calculate the cubic root
//    template <typename Float>
//    static Float cbrt(const Float n) noexcept;
//
//    // Exponential functions
//
//    //! Calculate e raised to the given power
//    template <typename Float>
//    static Float exp(const Float n) noexcept;
//
//    //! Calculate the natural logarithm
//    template <typename Float>
//    static Float log(const Float x) noexcept;
//
//    //! Calculate the binary logarithm
//    template <typename Float>
//    static Float log2(const Float x) noexcept;
//
//    //! Calculate the common logarithm
//    template <typename Float>
//    static Float log10(const Float x) noexcept;
//
//    // Trigonometric functions
//
//    //! Calculate sin(theta)
//    template <typename Float>
//    static Float sin(const Float theta) noexcept;
//
//    //! Calculate cos(theta)
//    template <typename Float>
//    static Float cos(const Float theta) noexcept;
//
//    //! Calculate tan(theta)
//    template <typename Float>
//    static Float tan(const Float theta) noexcept;
//
//    //! Calculate asin(x)
//    template <typename Float>
//    static Float asin(const Float x) noexcept;
//
//    //! Calculate acos(x)
//    template <typename Float>
//    static Float acos(const Float x) noexcept;
//
//    //! Calculate atan(x)
//    template <typename Float>
//    static Float atan(const Float x) noexcept;
//  };
//
//  class Zisc
//  {
//   public:
//    //! Raise a number to the given power
//    template <typename Float>
//    static Float pow(const Float base, const Float exponent) noexcept;
//
//    //! Calculate the square root
//    template <typename Float>
//    static Float sqrt(const Float n) noexcept;
//
//    //! Calculate the cubic root
//    template <typename Float>
//    static Float cbrt(const Float n) noexcept;
//
//    // Exponential functions
//
//    //! Calculate e raised to the given power
//    template <typename Float>
//    static Float exp(const Float n) noexcept;
//
//    //! Calculate the natural logarithm
//    template <typename Float>
//    static Float log(const Float x) noexcept;
//
//    //! Calculate the binary logarithm
//    template <typename Float>
//    static Float log2(const Float x) noexcept;
//
//    //! Calculate the common logarithm
//    template <typename Float>
//    static Float log10(const Float x) noexcept;
//
//    // Trigonometric functions
//
//    //! Calculate sin(theta)
//    template <typename Float>
//    static Float sin(const Float theta) noexcept;
//
//    //! Calculate cos(theta)
//    template <typename Float>
//    static Float cos(const Float theta) noexcept;
//
//    //! Calculate tan(theta)
//    template <typename Float>
//    static Float tan(const Float theta) noexcept;
//
//    //! Calculate asin(x)
//    template <typename Float>
//    static Float asin(const Float x) noexcept;
//
//    //! Calculate acos(x)
//    template <typename Float>
//    static Float acos(const Float x) noexcept;
//
//    //! Calculate atan(x)
//    template <typename Float>
//    static Float atan(const Float x) noexcept;
//  };
//
// private:
//  template <typename Float>
//  static Float solveCubicOneY(const Float p, const Float q) noexcept;
//
//  template <typename Float>
//  static void sortQuarticResults(const std::array<Float, 2>& x1,
//                                 const std::array<Float, 2>& x2,
//                                 std::array<Float, 4>& x) noexcept;

 private:
  template <FloatingPoint Float>
  using IntT = std::conditional_t<sizeof(Float) == 4, int32b,
               std::conditional_t<sizeof(Float) == 8, int64b, int>>;
  template <FloatingPoint Float>
  using BitT = std::conditional_t<sizeof(Float) == 4, uint32b,
               std::conditional_t<sizeof(Float) == 8, uint64b, int>>;

  struct Impl
  {
    // Float2

    template <FloatingPoint Float>
    struct F2 
    {
      constexpr F2() {}
      constexpr F2(const Float x) : x_{x} {}
      constexpr F2(const Float x, const Float y) : x_{x}, y_{y} {}
      Float x_ = cast<Float>(0.0);
      Float y_ = cast<Float>(0.0);
    };

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> add(const Float x, const F2<Float> y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> add(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> add2(const Float x, const Float y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> add2(const F2<Float> x, const Float y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> add2(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> div(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> mul(const F2<Float> x, const Float y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> mul(const F2<Float> x, const F2<Float> y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> normalize(const F2<Float> x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> scale(const F2<Float> x, const Float y) noexcept;

    //!
    template <typename ReturnType, FloatingPoint Float>
    static constexpr ReturnType squ(const F2<Float> x) noexcept;

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
    template <FloatingPoint Float>
    static constexpr Float ln2() noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float l2l() noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float l2u() noexcept;

    // Poly

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly2(const Float x) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly3(const Float x,
                                 const Float x2) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly4(const Float x,
                                 const Float x2) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly5(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly6(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly7(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly8(const Float x,
                                 const Float x2,
                                 const Float x4) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly9(const Float x,
                                 const Float x2,
                                 const Float x4,
                                 const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly10(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly11(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly12(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly13(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly14(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly15(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly16(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly17(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly18(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    //!
    template <typename Constans, std::size_t coffset, FloatingPoint Float>
    static constexpr Float poly19(const Float x,
                                  const Float x2,
                                  const Float x4,
                                  const Float x8,
                                  const Float x16) noexcept;

    // Math

    //!
    template <FloatingPoint Float>
    static constexpr Float abs(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float copysign(const Float x, const Float y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float exp(const F2<Float> x) noexcept;

    //!
    template <FloatingPoint Float, Arithmetic Arith1, Arithmetic Arith2>
    static constexpr auto fvalue(const Arith1 v1, const Arith2 v2) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr IntT<Float> ilogb2(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr bool isInf(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr bool isInt(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr bool isNan(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr bool isOdd(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float ldexp(Float x, int e) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float ldexp3(const Float x, const IntT<Float> e) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr F2<Float> log(Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float mla(const Float x, const Float y, const Float z) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float mulsign(const Float x, const Float y) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float rint(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float sign(const Float x) noexcept;

    //!
    template <FloatingPoint Float>
    static constexpr Float upper(const Float x) noexcept;
  };
};

// Constant values

////! Napier's constant
//template <typename Float>
//constexpr Float kE = constant::Math::exp(static_cast<Float>(1.0));

//! Circular constant
template <FloatingPoint Float>
constexpr Float kPi = Math::pi<Float>();

//// Basic operations
//
////! Compute the greatest common divisor of the integers m and n
//template <typename Integer1, typename Integer2>
//constexpr std::common_type_t<Integer1, Integer2> gcd(Integer1 m,
//                                                     Integer2 n) noexcept;
//
////! Compute the least common multiple of the integers m and n
//template <typename Integer1, typename Integer2>
//constexpr std::common_type_t<Integer1, Integer2> lcm(Integer1 m,
//                                                     Integer2 n) noexcept;
//
////!
//template <typename Integer>
//constexpr Integer factorial(const Integer n) noexcept;
//
////!
//template <typename Integer1, typename Integer2>
//constexpr std::common_type_t<Integer1, Integer2> sequence(const Integer1 m,
//                                                          const Integer2 n) noexcept;
//
//// Power functions
//
////! Raise a number to the given power
//template <int kExponent, typename Arithmetic>
//constexpr Arithmetic power(Arithmetic base) noexcept;
//
////! Raise a number to the given power
//template <typename Arithmetic, typename SignedInteger>
//constexpr Arithmetic power(const Arithmetic base,
//                           const SignedInteger exponent) noexcept;

//! Raise a number to the given power
template <Arithmetic Arith1, Arithmetic Arith2>
constexpr Arith1 pow(const Arith1 base, const Arith2 exponent) noexcept;

////! Calculate the square root
//template <typename Float>
//Float sqrt(const Float n) noexcept;
//
////! Calculate the cubic root
//template <typename Float>
//Float cbrt(const Float n) noexcept;
//
//// Exponential functions
//
////! Calculate e raised to the given power
//template <typename Float>
//Float exp(const Float n) noexcept;
//
////! Calculate the natural logarithm
//template <typename Float>
//Float log(const Float x) noexcept;
//
////! Calculate the binary logarithm
//float log2(const float x) noexcept;
//
////! Calculate the common logarithm
//float log10(const float x) noexcept;
//
//// Trigonometric functions
//
////! Calculate sin(theta)
//template <typename Float>
//Float sin(const Float theta) noexcept;
//
////! Calculate cos(theta)
//template <typename Float>
//Float cos(const Float theta) noexcept;
//
////! Calculate tan(theta)
//template <typename Float>
//Float tan(const Float theta) noexcept;
//
////! Calculate asin(x)
//template <typename Float>
//Float asin(const Float x) noexcept;
//
////! Calculate acos(x)
//template <typename Float>
//Float acos(const Float x) noexcept;
//
////! Calculate atan(x)
//template <typename Float>
//Float atan(const Float x) noexcept;

} // namespace zisc

#include "math-inl.hpp"

#endif // ZISC_MATH_HPP
