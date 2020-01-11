/*!
  \file matrix-inl.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef ZISC_MATRIX_INL_HPP
#define ZISC_MATRIX_INL_HPP

#include "matrix.hpp"
// Standard C++ library
#include <initializer_list>
#include <tuple>
#include <utility>
// Zisc
#include "arith_array.hpp"
#include "math.hpp"
#include "utility.hpp"
#include "zisc_config.hpp"

namespace zisc {

/*!
  \details No detailed description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn>::Matrix() noexcept :
    data_{}
{
}

/*!
  \details No detailed description

  \param [in] init_list No description.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn>::Matrix(
    std::initializer_list<Arith> init_list) noexcept :
        data_{init_list}
{
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::operator+=(const Matrix& other)
    noexcept -> Matrix&
{
  data_ += other.data_;
  return *this;
}

/*!
  \details No detailed description

  \param [in] other No description.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::operator-=(const Matrix& other)
    noexcept -> Matrix&
{
  data_ -= other.data_;
  return *this;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith& Matrix<Arith, kRow, kColumn>::operator()(
    const uint row,
    const uint column) noexcept
{
  return get(row, column);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr const Arith& Matrix<Arith, kRow, kColumn>::operator()(
    const uint row,
    const uint column) const noexcept
{
  return get(row, column);
}

/*!
  \details No detailed description

  \param [in] row No description.
  \param [in] column No description.
  \return No description
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
  \details No detailed description

  \return No description
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
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arith, kRow, kColumn>::columnSize() noexcept
{
  return kColumn;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::data() noexcept -> ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::data() const noexcept
    -> const ArrayType&
{
  return data_;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith Matrix<Arith, kRow, kColumn>::determinant() const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  constexpr uint n = kRow;
  Arith determinant = cast<Arith>(0);
  if constexpr (n == 1) {
    determinant = get(0, 0);
  }
  else if constexpr (n == 2) {
    determinant = get(0, 0) * get(1, 1) - get(0, 1) * get(1, 0);
  }
  else {
    const auto result = decomposeLu();
    const auto& lu = std::get<0>(result);
    const auto& p = std::get<1>(result);

    determinant = lu(0, 0);
    for (uint i = 1; i < n; ++i)
      determinant = determinant * lu(i, i);
    determinant = isOdd(p[n]) ? -determinant : determinant;
  }
  return determinant;
}

/*!
  \details No detailed description

  \param [in] row No description.
  \param [in] column No description.
  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith& Matrix<Arith, kRow, kColumn>::get(
    const uint row, 
    const uint column) noexcept
{
  return data_.get(row * kColumn + column);
}

/*!
  \details No detailed description

  \param [in] row No description.
  \param [in] column No description.
  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr const Arith& Matrix<Arith, kRow, kColumn>::get(
    const uint row, 
    const uint column) const noexcept
{
  return data_.get(row * kColumn + column);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::inverseMatrix() const noexcept
    -> Matrix
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");

  const auto result = decomposeLu();
  const auto& lu = std::get<0>(result);
  const auto& p = std::get<1>(result);

  Matrix inverse_matrix{};
  for (uint c = 0; c < kColumn; ++c) {
    for (uint r = 0; r < kRow; ++r) {
      inverse_matrix(r, c) = (p[r] == c) ? cast<Arith>(1) : cast<Arith>(0);
      for (uint k = 0; k < r; ++k) {
        inverse_matrix(r, c) = inverse_matrix(r, c) -
                               (lu(r, k) * inverse_matrix(k, c));
      }
    }
    for (uint i = 0; i < kRow; ++i) {
      const uint r = kRow - 1 - i;
      const Arith inv_r = invert(lu.get(r, r));
      for (uint k = r + 1; k < kRow; ++k) {
        inverse_matrix(r, c) = inverse_matrix(r, c) -
                               (lu(r, k) * inverse_matrix(k, c));
      }
      inverse_matrix(r, c) = inverse_matrix(r, c) * inv_r;
    }
  }
  return inverse_matrix;
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr bool Matrix<Arith, kRow, kColumn>::isSquareMatrix() noexcept
{
  return (kRow == kColumn);
}

/*!
  \details No detailed description

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr uint Matrix<Arith, kRow, kColumn>::rowSize() noexcept
{
  return kRow;
}

/*!
  \details No detailed description

  \param [in] row No description.
  \param [in] column No description.
  \param [in] value No description.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr void Matrix<Arith, kRow, kColumn>::set(const uint row,
                                                 const uint column,
                                                 const Arith value) noexcept
{
  data_.set(row * columnSize() + column, value);
}

/*!
  \details No detailed description

  \param [in] init_list No description.
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr void Matrix<Arith, kRow, kColumn>::set(
    std::initializer_list<Arith> init_list) noexcept
{
  data_.set(init_list);
}

/*!
  \details No detailed description

  \return No description
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
  \details Decompose a matrix A into LU matrixes
    | a00, a01, a02 |         | l00, u01, u02 |
  A | a10, a11, a12 | into LU | l10, l11, u12 |
    | a20, a21, a22 |         | l20, l21, l22 |

  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr auto Matrix<Arith, kRow, kColumn>::decomposeLu() const noexcept
    -> std::tuple<Matrix, ArithArray<uint, kRow+1>>
{
  constexpr uint n = kRow;

  Matrix lu{};
  lu.data_ = data_;
  ArithArray<uint, n+1> p{};

  for (uint i = 0; i < n; ++i)
    p[i] = i; // Unit permutation matrix
  p[n] = 0;

  for (uint i = 0; i < n; ++i) {
    Arith max_a = cast<Arith>(0);
    uint max_i = i;
    for (uint k = i; k < n; ++k) {
      const auto abs_a = abs(lu(k, i));
      if (max_a < abs_a) {
        max_a = abs_a;
        max_i = k;
      }
    }

    if (max_i != i) {
      //  Pivoting p
      const uint j = p[i];
      p[i] = p[max_i];
      p[max_i] = j;

      // Pivoting rows of A
      for (uint c = 0; c < n; ++c) {
        auto temp = lu(i, c);
        lu(i, c) = lu(max_i, c);
        lu(max_i, c) = temp;
      }
      // Counting pivots starting from N (for determinant)
      p[n] = p[n] + 1;
    }

    const Arith inv_i = invert(lu(i, i));
    for (uint j = i + 1; j < n; ++j) {
      lu(j, i) = lu(j, i) * inv_i;
      for (uint k = i + 1; k < n; ++k)
        lu(j, k) = lu(j, k) - (lu(j, i) * lu(i, k));
    }
  }

  return std::make_tuple(lu, p);
}

/*!
  \details No detailed description

  \param [in] row No description.
  \param [in] column No description.
  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Arith Matrix<Arith, kRow, kColumn>::minorDeterminant(
    const uint row,
    const uint column) const noexcept
{
  static_assert(isSquareMatrix(), "Matrix isn't square matrix.");
  // Make submatrix
  Matrix<Arith, rowSize() - 1, columnSize() - 1> submatrix{};
  for (uint r = 0, i = 0; r < rowSize() - 1; ++r, ++i) {
    if (r == row)
      ++i;
    for (uint c = 0, j = 0; c < columnSize() - 1; ++c, ++j) {
      if (c == column)
        ++j;
      submatrix(r, c) = get(i, j);
    }
  }
  // Calculate minor determinant
  return submatrix.determinant();
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam L No description.
  \tparam M No description.
  \tparam N No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
  */
template <typename Arith, uint kRow, uint kColumn> inline
constexpr Matrix<Arith, kRow, kColumn> operator*(
    const Matrix<Arith, kRow, kColumn>& lhs,
    const Arith rhs) noexcept
{
  return rhs * lhs;
}

/*!
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow No description.
  \tparam kColumn No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow1 No description.
  \tparam kColumn1 No description.
  \tparam kRow2 No description.
  \tparam kColumn2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
  \details No detailed description

  \tparam Arith No description.
  \tparam kRow1 No description.
  \tparam kColumn1 No description.
  \tparam kRow2 No description.
  \tparam kColumn2 No description.
  \param [in] lhs No description.
  \param [in] rhs No description.
  \return No description
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
