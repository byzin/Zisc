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
#include <initializer_list>
#include <utility>
// Zisc
#include "arith_array.hpp"
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
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn>::Matrix() noexcept :
    data_{}
{
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn>::Matrix(
    std::initializer_list<Arith> init_list) noexcept :
        data_{init_list}
{
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::operator+=(const Matrix& other)
    noexcept -> Matrix&
{
  data_ += other.data_;
  return *this;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::operator-=(const Matrix& other)
    noexcept -> Matrix&
{
  data_ -= other.data_;
  return *this;
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith& Matrix<Arith, kRow, kColumn>::operator()(
    const uint row,
    const uint column) noexcept
{
  return get(row, column);
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr const Arith& Matrix<Arith, kRow, kColumn>::operator()(
    const uint row,
    const uint column) const noexcept
{
  return get(row, column);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith Matrix<Arith, kRow, kColumn>::cofactor(
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
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::cofactorMatrix() const noexcept -> Matrix
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
template <typename Arith, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arith, kRow, kColumn>::columnSize() noexcept
{
  return kColumn;
}

namespace inner {

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kN> inline
constexpr Arith calculateDeterminant(const Matrix<Arith, kN, kN>& matrix) noexcept
{
  Arith determinant = cast<Arith>(0);
  for (uint column = 0; column < matrix.columnSize(); ++column)
    determinant += matrix(0, column) * matrix.cofactor(0, column);
  return determinant;
}

/*!
  \details
  No detailed.
  */
template <typename Arith> inline
constexpr Arith calculateDeterminant(const Matrix<Arith, 2, 2>& matrix) noexcept
{
  return matrix(0, 0) * matrix(1, 1) - matrix(0, 1) * matrix(1, 0);
}

/*!
  \details
  No detailed.
  */
template <typename Arith> inline
constexpr Arith calculateDeterminant(const Matrix<Arith, 1, 1>& matrix) noexcept
{
  return matrix(0, 0);
}

} // namespace inner

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::data() noexcept -> ArrayType&
{
  return data_;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::data() const noexcept
    -> const ArrayType&
{
  return data_;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith Matrix<Arith, kRow, kColumn>::determinant() const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  return inner::calculateDeterminant(*this);
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith& Matrix<Arith, kRow, kColumn>::get(
    const uint row, 
    const uint column) noexcept
{
  return data_.get(row * kColumn + column);
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr const Arith& Matrix<Arith, kRow, kColumn>::get(
    const uint row, 
    const uint column) const noexcept
{
  return data_.get(row * kColumn + column);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::inverseMatrix() const noexcept -> Matrix
{
  // Check the determinant
  const auto d = determinant();
  // Get a scaler
  constexpr auto one = cast<Arith>(1);
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
template <typename Arith, uint kRow, uint kColumn> inline
constexpr bool Matrix<Arith, kRow, kColumn>::isSquareMatrix() noexcept
{
  return (kRow == kColumn);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith Matrix<Arith, kRow, kColumn>::minorDeterminant(
    const uint row,
    const uint column) const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  // Make submatrix
  Matrix<Arith, rowSize() - 1, columnSize() - 1> submatrix;
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
template <typename Arith, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arith, kRow, kColumn>::rowSize() noexcept
{
  return kRow;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr void Matrix<Arith, kRow, kColumn>::set(const uint row,
                                                 const uint column,
                                                 const Arith value) noexcept
{
  data_.set(row * columnSize() + column, value);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr void Matrix<Arith, kRow, kColumn>::set(
    std::initializer_list<Arith> init_list) noexcept
{
  data_.set(init_list);
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kColumn, kRow> Matrix<Arith, kRow, kColumn>::
    transposedMatrix() const noexcept
{
  Matrix<Arith, kColumn, kRow> transposed_matrix;
  for (uint row = 0; row < rowSize(); ++row) {
    for (uint column = 0; column < columnSize(); ++column) {
      transposed_matrix(column, row) = get(row, column);
    }
  }
  return transposed_matrix;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn> operator+(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept
{
  Matrix<Arith, kRow, kColumn> result{};
  result.data() = lhs.data() + rhs.data();
  return result;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn> operator-(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept
{
  Matrix<Arith, kRow, kColumn> result{};
  result.data() = lhs.data() - rhs.data();
  return result;
}

/*!
 \details
 No detailed.
 */
template <typename Arith, uint L, uint M, uint N> inline
constexpr Matrix<Arith, L, N> operator*(
    const Matrix<Arith, L, M>& lhs,
    const Matrix<Arith, M, N>& rhs) noexcept
{
  Matrix<Arith, L, N> matrix{};
  for (uint l = 0; l < L; ++l) {
    for (uint n = 0; n < N; ++n) {
      Arith value = cast<Arith>(0);
      for (uint m = 0; m < M; ++m)
        value += lhs(l, m) * rhs(m, n);
      matrix(l, n) = value;
    }
  }
  return matrix;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn> operator*(
    const Arith lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept
{
  Matrix<Arith, kRow, kColumn> matrix{};
  matrix.data() = rhs.data() * lhs;
  return matrix;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn> operator*(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Arith rhs) noexcept
{
  return rhs * lhs;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr ArithArray<Arith,  kColumn> operator*(
    const ArithArray<Arith, kRow>& lhs,
    const Matrix<Arith, kRow, kColumn>& rhs) noexcept
{
  ArithArray<Arith, kColumn> result;
  for (uint column = 0; column < kColumn; ++column) {
    Arith value = cast<Arith>(0);
    for (uint row = 0; row < kRow; ++row)
      value += lhs[row] * rhs(row, column);
    result[column] = value;
  }
  return result;
}

/*!
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr ArithArray<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const ArithArray<Arith, kColumn>& rhs) noexcept
{
  ArithArray<Arith, kRow> result;
  for (uint row = 0; row < kRow; ++row) {
    Arith value = cast<Arith>(0);
    for (uint column = 0; column < kColumn; ++column)
      value += lhs(row, column) * rhs[column];
    result[row] = value;
  }
  return result;
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Point<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& matrix,
    const Point<Arith, kColumn>& point) noexcept
{
  return Point<Arith, kRow>{matrix * point.data()};
}

/*!
  \details
  No detailed.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Vector<Arith, kRow> operator*(
    const Matrix<Arith, kRow, kColumn>& matrix,
    const Vector<Arith, kColumn>& vector) noexcept
{
  return Vector<Arith, kRow>{matrix * vector.data()};
}

/*!
  */
template <typename Arith, uint kRow1, uint kColumn1, uint kRow2, uint kColumn2>
inline
constexpr bool operator==(
    const Matrix<Arith, kRow1, kColumn1>& lhs,
    const Matrix<Arith, kRow2, kColumn2>& rhs) noexcept
{
  if constexpr ((kRow1 == kRow2) && (kColumn1 == kColumn2))
    return lhs.data() == rhs.data();
  else
    return false;
}

/*!
  */
template <typename Arith, uint kRow1, uint kColumn1, uint kRow2, uint kColumn2>
inline
constexpr bool operator!=(
    const Matrix<Arith, kRow1, kColumn1>& lhs,
    const Matrix<Arith, kRow2, kColumn2>& rhs) noexcept
{
  return !(lhs == rhs);
}

} // namespace zisc

#endif // ZISC_MATRIX_INL_HPP
