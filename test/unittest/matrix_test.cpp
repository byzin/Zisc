/*!
  \file matrix_test.cpp
  \author zin
  */

// Standard C++ library
#include <cstddef>
#include <type_traits>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/arith_array.hpp"
#include "zisc/matrix.hpp"
#include "zisc/point.hpp"
#include "zisc/vector.hpp"
#include "zisc/utility.hpp"
#include "zisc/zisc_config.hpp"

TEST(MatrixTest, ConstructionTest)
{
  {
    constexpr zisc::uint n = 2;
    constexpr zisc::Matrix<int, n, n> matrix{};
    ASSERT_EQ(n, matrix.rowSize())
        << "The row size of the matrix isn't " << n;
    ASSERT_EQ(n, matrix.columnSize())
        << "The column size of the matrix isn't " << n;
    {
      constexpr auto v = matrix(0, 0);
      ASSERT_EQ(0, v) << "The default value of m(0,0) isn't 0.";
    }
    {
      constexpr auto v = matrix(0, 1);
      ASSERT_EQ(0, v) << "The default value of m(0,1) isn't 0.";
    }
    {
      constexpr auto v = matrix(1, 0);
      ASSERT_EQ(0, v) << "The default value of m(1,0) isn't 0.";
    }
    {
      constexpr auto v = matrix(1, 1);
      ASSERT_EQ(0, v) << "The default value of m(1,1) isn't 0.";
    }
  }
  {
    constexpr zisc::uint row = 2;
    constexpr zisc::uint column = 4;
    constexpr zisc::Matrix<int, row, column> matrix{{0, 1, 2, 3,
                                                     4, 5, 6, 7}};
    ASSERT_EQ(row, matrix.rowSize())
        << "The row size of the matrix isn't " << row;
    ASSERT_EQ(column, matrix.columnSize())
        << "The column size of the matrix isn't " << column;
    for (zisc::uint r = 0; r < row; ++r) {
      for (zisc::uint c = 0; c < column; ++c) {
        const int i = static_cast<int>(c + r * column);
        ASSERT_EQ(i, matrix(r, c))
            << "The value of m(" << r << "," << c << ") isn't " << i;
      }
    }
  }
  {
    constexpr zisc::uint row = 3;
    constexpr zisc::uint column = 5;
    using Matrix = zisc::Matrix<double, row, column>;
    auto make_matrix = []()
    {
      Matrix matrix;
      for (zisc::uint r = 0; r < Matrix::rowSize(); ++r) {
        for (zisc::uint c = 0; c < Matrix::columnSize(); ++c) {
          const double i = static_cast<double>(c + r * Matrix::columnSize());
          matrix(r, c) = i;
          matrix.set(r, c, i);
        }
      }
      return matrix;
    };
    constexpr auto matrix = make_matrix();
    ASSERT_EQ(row, matrix.rowSize())
        << "The row size of the matrix isn't " << row;
    ASSERT_EQ(column, matrix.columnSize())
        << "The column size of the matrix isn't " << column;
    for (zisc::uint r = 0; r < row; ++r) {
      for (zisc::uint c = 0; c < column; ++c) {
        const double i = static_cast<double>(c + r * column);
        ASSERT_DOUBLE_EQ(i, matrix(r, c))
            << "The value of m(" << r << "," << c << ") isn't " << i;
      }
    }
  }
}

TEST(MatrixTest, OperatorTest)
{
  {
    auto make_matrix = [](const int v)
    {
      constexpr zisc::uint row = 3;
      constexpr zisc::uint column = 3;
      zisc::Matrix<int, row, column> matrix{};
      for (zisc::uint r = 0; r < row; ++r) {
        for (zisc::uint c = 0; c < column; ++c) {
          matrix(r, c) = v;
        }
      }
      return matrix;
    };

    constexpr int v = 1;
    constexpr auto m1 = make_matrix(v);

    {
      constexpr auto result = m1 + m1;
      for (zisc::uint r = 0; r < m1.rowSize(); ++r) {
        for (zisc::uint c = 0; c < m1.columnSize(); ++c) {
          ASSERT_EQ(2 * v, result(r, c)) << "The addition operation failed.";
        }
      }
    }
    {
      constexpr auto result = m1 - m1;
      for (zisc::uint r = 0; r < m1.rowSize(); ++r) {
        for (zisc::uint c = 0; c < m1.columnSize(); ++c) {
          ASSERT_EQ(0, result(r, c)) << "The subtraction operation failed.";
        }
      }
    }

    constexpr auto m2 = make_matrix(2);

    {
      constexpr bool result1 = m1 == m1;
      ASSERT_TRUE(result1);
      constexpr bool result2 = m1 != m1;
      ASSERT_FALSE(result2);
    }
    {
      constexpr bool result1 = m1 == m2;
      ASSERT_FALSE(result1);
      constexpr bool result2 = m1 != m2;
      ASSERT_TRUE(result2);
    }

    constexpr zisc::Matrix<int, 2, 2> m3{};
    {
      ASSERT_FALSE(m1 == m3);
      ASSERT_TRUE(m1 != m3);
    }
  }
  {
    constexpr zisc::uint row = 3;
    constexpr zisc::uint column = 3;
    constexpr zisc::Matrix<double, row, column> matrix{0.0, 1.0, 2.0,
                                                       3.0, 4.0, 5.0,
                                                       6.0, 7.0, 8.0};
    constexpr auto result1 = matrix * matrix;
    const double expected[] = {15.0, 18.0, 21.0, 42.0, 54.0, 66.0, 69.0, 90.0, 111.0};
    for (zisc::uint r = 0; r < row; ++r) {
      for (zisc::uint c = 0; c < column; ++c) {
        const auto index = c + r * column;
        ASSERT_DOUBLE_EQ(expected[index], result1(r, c))
            << "The multiplication operation failed.";
      }
    }

    constexpr auto result2 = matrix * 2.0;
    for (zisc::uint r = 0; r < row; ++r) {
      for (zisc::uint c = 0; c < column; ++c) {
        ASSERT_DOUBLE_EQ(2.0 * matrix(r, c), result2(r, c))
            << "The multiplication operation failed.";
      }
    }
  }
  {
    constexpr zisc::uint row = 3;
    constexpr zisc::uint column = 4;
    constexpr zisc::Matrix<double, row, column> matrix{0.0, 1.0, 2.0, 3.0,
                                                       4.0, 5.0, 6.0, 7.0,
                                                       8.0, 9.0, 10.0, 11.0};
    constexpr zisc::ArithArray<double, row> a{1.0, 2.0, 3.0};

    constexpr auto result = a * matrix;
    ASSERT_EQ(column, result.size());
    constexpr double expected[] = {32.0, 38.0, 44.0, 50.0};
    for (zisc::uint c = 0; c < column; ++c)
      ASSERT_DOUBLE_EQ(expected[c], result[c]);
  }
  {
    constexpr zisc::uint row = 4;
    constexpr zisc::uint column = 3;
    constexpr zisc::Matrix<double, row, column> matrix{0.0, 1.0, 2.0,
                                                       3.0, 4.0, 5.0,
                                                       6.0, 7.0, 8.0,
                                                       9.0, 10.0, 11.0};
    constexpr zisc::ArithArray<double, column> a{1.0, 2.0, 3.0};

    constexpr auto result = matrix * a;
    ASSERT_EQ(row, result.size());
    constexpr double expected[] = {8.0, 26.0, 44.0, 62.0};
    for (zisc::uint r = 0; r < row; ++r)
      ASSERT_DOUBLE_EQ(expected[r], result[r]);
  }
  {
    constexpr zisc::uint row = 4;
    constexpr zisc::uint column = 3;
    constexpr zisc::Matrix<double, row, column> matrix{0.0, 1.0, 2.0,
                                                       3.0, 4.0, 5.0,
                                                       6.0, 7.0, 8.0,
                                                       9.0, 10.0, 11.0};
    constexpr zisc::Point<double, column> a{1.0, 2.0, 3.0};

    constexpr auto result = matrix * a;
    ASSERT_EQ(row, result.size());
    constexpr double expected[] = {8.0, 26.0, 44.0, 62.0};
    for (zisc::uint r = 0; r < row; ++r)
      ASSERT_DOUBLE_EQ(expected[r], result[r]);
  }
  {
    constexpr zisc::uint row = 4;
    constexpr zisc::uint column = 3;
    constexpr zisc::Matrix<double, row, column> matrix{0.0, 1.0, 2.0,
                                                       3.0, 4.0, 5.0,
                                                       6.0, 7.0, 8.0,
                                                       9.0, 10.0, 11.0};
    constexpr zisc::Vector<double, column> a{1.0, 2.0, 3.0};

    constexpr auto result = matrix * a;
    ASSERT_EQ(row, result.size());
    constexpr double expected[] = {8.0, 26.0, 44.0, 62.0};
    for (zisc::uint r = 0; r < row; ++r)
      ASSERT_DOUBLE_EQ(expected[r], result[r]);
  }
}

TEST(MatrixTest, FunctionTest)
{
  // Cofactor matrix
  {
    constexpr zisc::Matrix<int, 3, 3> matrix{{1, 2, 3,
                                              0, 4, 5,
                                              1, 0, 6}};
    constexpr zisc::Matrix<int, 3, 3> expected{{24, 5, -4,
                                                -12, 3, 2,
                                                -2, -5, 4}};
    constexpr auto result = matrix.cofactorMatrix();
    ASSERT_EQ(expected, result);
  }
  {
    constexpr zisc::Matrix<double, 3, 3> matrix{{1.0, 2.0, 3.0,
                                                 6.0, 5.0, 4.0,
                                                 7.0, 8.0, 9.0}};
    constexpr zisc::Matrix<double, 3, 3> expected{{13.0, -26.0, 13.0,
                                                    6.0, -12.0, 6.0,
                                                   -7.0, 14.0, -7.0}};
    constexpr auto result = matrix.cofactorMatrix();
    for (zisc::uint row = 0; row < 3; ++row) {
      for (zisc::uint column = 0; column < 3; ++column) {
        EXPECT_DOUBLE_EQ(expected(row, column), result(row, column));
      }
    }
  }
  // Transposed matrix
  {
    constexpr zisc::Matrix<int, 3, 3> matrix{{1, 2, 3,
                                              0, 4, 5,
                                              1, 0, 6}};
    constexpr zisc::Matrix<int, 3, 3> expected{{1, 0, 1,
                                                2, 4, 0,
                                                3, 5, 6}};
    constexpr auto result = matrix.transposedMatrix();
    ASSERT_EQ(expected, result);
  }
  {
    constexpr zisc::Matrix<double, 4, 4> matrix{{ 1.0,  2.0,  3.0,  4.0,
                                                  5.0,  6.0,  7.0,  8.0,
                                                  9.0, 10.0, 11.0, 12.0,
                                                 13.0, 14.0, 15.0, 16.0}};
    constexpr zisc::Matrix<double, 4, 4> expected{{1.0,  5.0,  9.0, 13.0,
                                                   2.0,  6.0, 10.0, 14.0,
                                                   3.0,  7.0, 11.0, 15.0,
                                                   4.0,  8.0, 12.0, 16.0}};
    constexpr auto result = matrix.transposedMatrix();
    for (zisc::uint row = 0; row < 4; ++row) {
      for (zisc::uint column = 0; column < 4; ++column) {
        EXPECT_DOUBLE_EQ(expected(row, column), result(row, column));
      }
    }
  }
  // Inverse matrix test
  {
    constexpr zisc::Matrix<float, 3, 3> matrix{{1.0f, 2.0f, 3.0f,
                                                0.0f, 4.0f, 5.0f,
                                                1.0f, 0.0f, 6.0f}};
    constexpr auto expected = (1.0f / 22.0f) * 
                               zisc::Matrix<float, 3, 3>{{24.0f, -12.0f, -2.0f,
                                                           5.0f, 3.0f, -5.0f,
                                                          -4.0f, 2.0f, 4.0f}};
    constexpr auto result = matrix.inverseMatrix();
    for (int row = 0; row < 3; ++row) {
      for (int column = 0; column < 3; ++column) {
        EXPECT_FLOAT_EQ(expected(row, column), result(row, column));
      }
    }
  }
  {
    constexpr zisc::Matrix<double, 3, 3> matrix{{1.0, 2.0, 1.0,
                                                 1.0, 0.0, 3.0,
                                                 2.0, 1.0, 1.0}};
    constexpr auto expected = (1.0 / 8.0) *
                              zisc::Matrix<double, 3, 3>{{-3.0,-1.0, 6.0,
                                                           5.0,-1.0,-2.0,
                                                           1.0, 3.0,-2.0}};
    constexpr auto result = matrix.inverseMatrix();
    for (zisc::uint row = 0; row < 3; ++row) {
      for (zisc::uint column = 0; column < 3; ++column) {
        EXPECT_DOUBLE_EQ(expected(row, column), result(row, column));
      }
    }
  }
  {
    constexpr zisc::Matrix<double, 2, 2> matrix{{4.0, 5.0,
                                                 2.0, 3.0}};
    constexpr auto expected = (1.0 / 2.0) *
                              zisc::Matrix<double, 2, 2>{{ 3.0,-5.0,
                                                          -2.0, 4.0}};
    constexpr auto result = matrix.inverseMatrix();
    for (zisc::uint row = 0; row < 2; ++row) {
      for (zisc::uint column = 0; column < 2; ++column) {
        EXPECT_DOUBLE_EQ(expected(row, column), result(row, column));
      }
    }
  }
  // Determinant test
  {
    constexpr zisc::Matrix<double, 2, 2> matrix{{ 2.0, 1.0,
                                                 -1.0, 3.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(7.0, d);
  }
  {
    constexpr zisc::Matrix<double, 2, 2> matrix{{1.0,-4.0,
                                                 2.0,-2.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(6.0, d);
  }
  {
    constexpr zisc::Matrix<double, 3, 3> matrix{{ 2.0, 3.0, 0.0,
                                                  5.0, 4.0, 1.0,
                                                 -2.0,-3.0,-1.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(7.0, d);
  }
  {
    constexpr zisc::Matrix<double, 3, 3> matrix{{1.0, 0.0, 0.0,
                                                 2.0,-1.0, 3.0,
                                                 1.0,-1.0, 1.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(2.0, d);
  }
  {
    constexpr zisc::Matrix<double, 4, 4> matrix{{2.0,-1.0, 3.0, 4.0,
                                                 0.0, 1.0, 2.0, 3.0,
                                                 3.0, 2.0, 4.0, 6.0,
                                                 0.0, 1.0, 1.0, 1.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(9.0, d);
  }
  {
    constexpr zisc::Matrix<double, 4, 4> matrix{{1.0, 2.0, 0.0, 0.0,
                                                 1.0, 1.0, 2.0, 3.0,
                                                 2.0, 2.0, 3.0, 4.0,
                                                 1.0, 2.0, 3.0, 4.0}};
    constexpr double d = matrix.determinant();
    ASSERT_DOUBLE_EQ(-2.0, d);
  }
}
