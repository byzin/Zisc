/*
  \file matrix-inl.hpp
  \author Sho Ikeda

  Copyright (c) 2015-2017 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATRIX_INL_HPP
#define ZISC_MATRIX_INL_HPP

#include "matrix.hpp"
// Standard C++ library
#include <utility>
// Zisc
#include "arithmetic_array.hpp"
#include "point.hpp"
#include "utility.hpp"
#include "vector.hpp"
#include "zisc/zisc_config.hpp"

namespace zisc {

// public member function

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Matrix<Arithmetic, kRow, kColumn>::Matrix() noexcept :
    elements_{}
{
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint kRow, uint kColumn>
template <typename ...Types> inline
constexpr Matrix<Arithmetic, kRow, kColumn>::Matrix(const Types ...elements) noexcept :
    elements_{elements...}
{
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr bool Matrix<Arithmetic, kRow, kColumn>::operator==(const Matrix& matrix) const 
    noexcept
{
  return (elements_ == matrix.elements_);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr bool Matrix<Arithmetic, kRow, kColumn>::operator!=(const Matrix& matrix) const
    noexcept
{
  return (elements_ != matrix.elements_);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arithmetic, kRow, kColumn>::operator*(const Arithmetic scalar) const
    noexcept -> Matrix
{
  Matrix matrix;
  matrix.elements_ = scalar * elements_;
  return matrix;
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Arithmetic& Matrix<Arithmetic, kRow, kColumn>::operator()(
    const uint row,
    const uint column) noexcept
{
  return get(row, column);
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr const Arithmetic& Matrix<Arithmetic, kRow, kColumn>::operator()(
    const uint row,
    const uint column) const noexcept
{
  return get(row, column);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Arithmetic Matrix<Arithmetic, kRow, kColumn>::cofactor(
    const uint row,
    const uint column) const noexcept
{
  const auto determinant = minorDeterminant(row, column);
  return (isOdd(row + column)) ? -determinant : determinant;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arithmetic, kRow, kColumn>::cofactorMatrix() const noexcept -> Matrix
{
  Matrix cofactor_matrix;
  for (uint row = 0; row < rowSize(); ++row) {
    for (uint column = 0; column < columnSize(); ++column) {
      cofactor_matrix.set(row, column, cofactor(row, column));
    }
  }
  return cofactor_matrix;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arithmetic, kRow, kColumn>::columnSize() noexcept
{
  return kColumn;
}

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
constexpr Arithmetic calculateDeterminant(const Matrix<Arithmetic, kN, kN>& matrix) noexcept
{
  Arithmetic determinant = cast<Arithmetic>(0);
  for (uint column = 0; column < matrix.columnSize(); ++column)
    determinant += matrix(0, column) * matrix.cofactor(0, column);
  return determinant;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic> inline
constexpr Arithmetic calculateDeterminant(const Matrix<Arithmetic, 2, 2>& matrix) noexcept
{
  return matrix(0, 0) * matrix(1, 1) - matrix(0, 1) * matrix(1, 0);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic> inline
constexpr Arithmetic calculateDeterminant(const Matrix<Arithmetic, 1, 1>& matrix) noexcept
{
  return matrix(0, 0);
}

} // namespace inner

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Arithmetic Matrix<Arithmetic, kRow, kColumn>::determinant() const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  return inner::calculateDeterminant(*this);
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Arithmetic& Matrix<Arithmetic, kRow, kColumn>::get(
    const uint row, 
    const uint column) noexcept
{
  return elements_.get(row * kColumn + column);
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr const Arithmetic& Matrix<Arithmetic, kRow, kColumn>::get(
    const uint row, 
    const uint column) const noexcept
{
  return elements_.get(row * kColumn + column);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arithmetic, kRow, kColumn>::inverseMatrix() const noexcept -> Matrix
{
  // Check the determinant
  const auto d = determinant();
  // Get a scaler
  constexpr auto one = cast<Arithmetic>(1);
  const auto k = one / d;
  // Get the inverse matrix
  Matrix inverse_matrix;
  for (uint row = 0; row < rowSize(); ++row) {
    for (uint column = 0; column < columnSize(); ++column) {
      inverse_matrix(row, column) = k * cofactor(column, row);
    }
  }
  return inverse_matrix;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr bool Matrix<Arithmetic, kRow, kColumn>::isSquareMatrix() noexcept
{
  return (kRow == kColumn);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn>
constexpr Arithmetic Matrix<Arithmetic, kRow, kColumn>::minorDeterminant(
    const uint row,
    const uint column) const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  // Make submatrix
  Matrix<Arithmetic, rowSize() - 1, columnSize() - 1> submatrix;
  for (uint i = 0, r = 0; i < rowSize(); ++i) {
    if (i == row)
      continue;
    for (uint j = 0, c = 0; j < columnSize(); ++j) {
      if (j == column)
        continue;
      submatrix(r, c) = get(i, j);
      ++c;
    }
    ++r;
  }
  // Calculate minor determinant
  return submatrix.determinant();
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arithmetic, kRow, kColumn>::rowSize() noexcept
{
  return kRow;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
void Matrix<Arithmetic, kRow, kColumn>::set(const uint row,
                                            const uint column,
                                            const Arithmetic value) noexcept
{
  elements_.set(row * columnSize() + column, value);
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Matrix<Arithmetic, kColumn, kRow> Matrix<Arithmetic, kRow, kColumn>::
    transposedMatrix() const noexcept
{
  Matrix<Arithmetic, kColumn, kRow> transposed_matrix;
  for (uint row = 0; row < rowSize(); ++row) {
    for (uint column = 0; column < columnSize(); ++column) {
      transposed_matrix(column, row) = get(row, column);
    }
  }
  return transposed_matrix;
}

/*!
 \details
 No detailed.
 */
template <typename Arithmetic, uint L, uint M, uint N>
constexpr Matrix<Arithmetic, L, N> operator*(
    const Matrix<Arithmetic, L, M>& a,
    const Matrix<Arithmetic, M, N>& b) noexcept
{
  Matrix<Arithmetic, L, N> matrix;
  for (uint l = 0; l < L; ++l) {
    for (uint n = 0; n < N; ++n) {
      Arithmetic value = cast<Arithmetic>(0);
      for (uint m = 0; m < M; ++m)
        value += a(l, m) * b(m, n);
      matrix(l, n) = value;
    }
  }
  return matrix;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kRow, uint kColumn> inline
constexpr Matrix<Arithmetic, kRow, kColumn> operator*(
    const Arithmetic scalar,
    const Matrix<Arithmetic, kRow, kColumn>& matrix) noexcept
{
  return matrix * scalar;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN>
constexpr ArithmeticArray<Arithmetic, kN> operator*(
    const Matrix<Arithmetic, kN, kN>& matrix,
    const ArithmeticArray<Arithmetic, kN>& array) noexcept
{
  ArithmeticArray<Arithmetic, kN> result;
  for (uint row = 0; row < kN; ++row) {
    Arithmetic value = cast<Arithmetic>(0);
    for (uint column = 0; column < kN; ++column)
      value += matrix(row, column) * array[column];
    result[row] = value;
  }
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
Point<Arithmetic, kN> operator*(const Matrix<Arithmetic, kN, kN>& matrix,
                                const Point<Arithmetic, kN>& point) noexcept
{
  return Point<Arithmetic, kN>{matrix * point.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arithmetic, uint kN> inline
Vector<Arithmetic, kN> operator*(const Matrix<Arithmetic, kN, kN>& matrix,
                                 const Vector<Arithmetic, kN>& vector) noexcept
{
  return Vector<Arithmetic, kN>{matrix * vector.data()};
}

} // namespace zisc

#endif // ZISC_MATRIX_INL_HPP
