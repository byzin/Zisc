/*!
  \file matrix.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATRIX_HPP
#define ZISC_MATRIX_HPP

// Standard C++ library
#include <initializer_list>
#include <tuple>
// Zisc
#include "arith_array.hpp"
#include "dimension.hpp"
#include "point.hpp"
#include "vector.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn>
class Matrix
{
  static_assert(0 < kRow, "kRow size is zero.");
  static_assert(0 < kColumn, "kColumn size is zero.");

 public:
  using ArrayType = ArithArray<Arith, kRow * kColumn>;


  //! Initialize with 0
  constexpr Matrix() noexcept;

  //! Initialize with values
  constexpr Matrix(std::initializer_list<Arith> init_list) noexcept;


  //! Perform addition operation on two matrixes
  constexpr Matrix& operator+=(const Matrix& other) noexcept;

  //! Perform subtraction operation on two matrixes
  constexpr Matrix& operator-=(const Matrix& other) noexcept;

  //! Return the reference by row and column
  constexpr Arith& operator()(const uint row,
                              const uint column) noexcept;

  //! Return the reference by row and column
  constexpr const Arith& operator()(const uint row,
                                    const uint column) const noexcept;

  //! Calculate the cofactor
  constexpr Arith cofactor(const uint row, const uint column) const noexcept;

  //! Return the cofactor matrix
  constexpr Matrix cofactorMatrix() const noexcept;

  //! Return column size
  static constexpr uint columnSize() noexcept;

  //! Return the raw array of the matrix
  constexpr ArrayType& data() noexcept;

  //! Return the raw array of the matrix
  constexpr const ArrayType& data() const noexcept;

  //! Calculate the determinant
  constexpr Arith determinant() const noexcept;

  //! Return the reference by row and column
  constexpr Arith& get(const uint row, const uint column) noexcept;

  //! Return the reference by row and column
  constexpr const Arith& get(const uint row, const uint column) const noexcept;

  //! Return the inverse matrix
  constexpr Matrix inverseMatrix() const noexcept;

  //! Check if the matrix is square matrix
  static constexpr bool isSquareMatrix() noexcept;

  //! Return row size
  static constexpr uint rowSize() noexcept;

  //! Set element
  constexpr void set(const uint row, const uint column, const Arith value) noexcept;

  //! Set elements
  constexpr void set(std::initializer_list<Arith> init_list) noexcept;

  //! Return the transposed matrix
  constexpr Matrix<Arith, kColumn, kRow> transposedMatrix() const noexcept;

 private:
  //! Perform LU decomposition
  constexpr std::tuple<Matrix, ArithArray<uint, kRow+1>> decomposeLu() const noexcept;

  //! Calculate the minor determinant
  constexpr Arith minorDeterminant(const uint row,
                                   const uint column) const noexcept;


  ArrayType data_;
};

//! Perform addition operation on two matrixes
template <typename Arith, uint kRow, uint kColumn>
constexpr Matrix<Arith, kRow, kColumn> operator+(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept;

//! Perform addition operation on two matrixes
template <typename Arith, uint kRow, uint kColumn>
constexpr Matrix<Arith, kRow, kColumn> operator-(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept;

//! Perform multiplication operation on two matrixes
template <typename Arith, uint L, uint M, uint N>
constexpr Matrix<Arith, L, N> operator*(
    const Matrix<Arith, L, M>& lhs,
    const Matrix<Arith, M, N>& rhs) noexcept;

//! Perform multiplication operation on two values
template <typename Arith, uint kRow, uint kColumn>
constexpr Matrix<Arith, kRow, kColumn> operator*(
    const Arith lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept;

//! Perform multiplication operation on two values
template <typename Arith, uint kRow, uint kColumn>
constexpr Matrix<Arith, kRow, kColumn> operator*(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Arith rhs) noexcept;

//! Perform multiplication operation on matrix and vector
template <typename Arith, uint kRow, uint kColumn>
constexpr ArithArray<Arith, kColumn> operator*(
    const ArithArray<Arith, kRow>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept;

//! Perform multiplication operation on matrix and vector
template <typename Arith, uint kRow, uint kColumn>
constexpr ArithArray<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const ArithArray<Arith, kColumn>& rhs) noexcept;

//! Multiply a matrix and a point
template <typename Arith, uint kRow, uint kColumn>
constexpr Point<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& matrix,
    const Point<Arith, kColumn>& point) noexcept;

//! Multiply a matrix and a point
template <typename Arith, uint kRow, uint kColumn>
constexpr Vector<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& matrix,
    const Vector<Arith, kColumn>& vector) noexcept;

//! Check if the two matrixes are same
template <typename Arith, uint kRow1, uint kColumn1, uint kRow2, uint kColumn2>
constexpr bool operator==(
    const Matrix<Arith, kRow1, kColumn1>& lhs,
    const Matrix<Arith, kRow2, kColumn2>& rhs) noexcept;

//! Check if the two matrixes aren't same
template <typename Arith, uint kRow1, uint kColumn1, uint kRow2, uint kColumn2>
constexpr bool operator!=(
    const Matrix<Arith, kRow1, kColumn1>& lhs,
    const Matrix<Arith, kRow2, kColumn2>& rhs) noexcept;

} // namespace zisc

#include "matrix-inl.hpp"

#endif // ZISC_MATRIX_HPP
