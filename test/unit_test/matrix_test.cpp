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
#include "zisc/matrix.hpp"
#include "zisc/vector.hpp"
#include "zisc/utility.hpp"

TEST(MatrixTest, OperationTest)
{
  using zisc::cast;
  // Determinant test
  {
    const zisc::Matrix<int, 3, 3> matrix{6, 1, 1,
                                         4, -2, 5,
                                         2, 8, 7};
    ASSERT_EQ(-306, matrix.determinant());
  }
  // Cofactor matrix test
  {
    const zisc::Matrix<int, 3, 3> matrix{1, 2, 3,
                                         0, 4, 5,
                                         1, 0, 6};
    const zisc::Matrix<int, 3, 3> reference{24, 5, -4,
                                            -12, 3, 2,
                                            -2, -5, 4};
    const auto cofactor_matrix = matrix.cofactorMatrix();
    for (int row = 0; row < 3; ++row) {
      for (int column = 0; column < 3; ++column) {
        ASSERT_EQ(reference(row, column), cofactor_matrix(row, column));
      }
    }
  }
  // Transposed matrix
  {
    const zisc::Matrix<int, 3, 3> matrix{1, 2, 3,
                                         0, 4, 5,
                                         1, 0, 6};
    const zisc::Matrix<int, 3, 3> reference{1, 0, 1,
                                            2, 4, 0,
                                            3, 5, 6};
    const auto transposed_matrix = matrix.transposedMatrix();
    for (int row = 0; row < 3; ++row) {
      for (int column = 0; column < 3; ++column) {
        ASSERT_EQ(reference(row, column), transposed_matrix(row, column));
      }
    }
  }
  // Inverse matrix test
  {
    const zisc::Matrix<double, 3, 3> matrix{1.0, 2.0, 3.0,
                                            0.0, 4.0, 5.0,
                                            1.0, 0.0, 6.0};
    const auto reference = (1.0 / 22.0) * 
                           zisc::Matrix<double, 3, 3>{24.0, -12.0, -2.0,
                                                      5.0, 3.0, -5.0,
                                                      -4.0, 2.0, 4.0};
    const auto inverse_matrix = matrix.inverseMatrix();
    for (int row = 0; row < 3; ++row) {
      for (int column = 0; column < 3; ++column) {
        ASSERT_DOUBLE_EQ(reference(row, column), inverse_matrix(row, column));
      }
    }
  }
}

template <typename Float>
void testMatrix3x3()
{
  using zisc::cast;

  zisc::Matrix<Float, 3, 3> matrix{0.0, 1.0, 2.0,
                                   3.0, 4.0, 5.0,
                                   6.0, 7.0, 8.0};
  double value;

  // Get value test
  for (zisc::uint row = 0; row < 3; ++row) {
    for (zisc::uint column = 0; column < 3; ++column) {
      value = cast<double>(row * 3 + column);
      ASSERT_DOUBLE_EQ(value, cast<double>(matrix(row, column)))
          << "## Get value test failed.";
    }
  }

  // Multiplication test
  auto multiplication = matrix * matrix;
  const double reference1[] = {15.0, 18.0, 21.0, 42.0, 54.0, 66.0, 69.0, 90.0, 111.0};
  for (int row = 0; row < 3; ++row) {
    for (int column = 0; column < 3; ++column) {
      int index = row * 3 + column;
      ASSERT_DOUBLE_EQ(reference1[index], multiplication(row, column))
          << "## Multiplication test failed.";
    }
  }

  // Matrix-vector multiplication test
  zisc::Vector<Float, 3> vector{1.0, 2.0, 3.0};
  auto multiplication_vector = matrix * vector;
  const double reference2[] = {8.0, 26.0, 44.0};
  for (int i = 0; i < 3; ++i) {
    ASSERT_DOUBLE_EQ(reference2[i], cast<double>(multiplication_vector[i]))
        << "## Matrix-vector multiplication test failed.";
  }
}

template <typename Float>
void testMatrix4x4()
{
  static_assert(std::is_floating_point<Float>::value,
                "## Float must be floating point.");
  using zisc::cast;

  zisc::Matrix<Float, 4, 4> matrix{0.0, 1.0, 2.0, 3.0, 
                                   4.0, 5.0, 6.0, 7.0, 
                                   8.0, 9.0, 10.0, 11.0,
                                   12.0, 13.0, 14.0, 15.0};
  double value;

  // Get value test
  for (zisc::uint row = 0; row < 4; ++row) {
    for (zisc::uint column = 0; column < 4; ++column) {
      value = cast<double>(row * 4 + column);
      ASSERT_DOUBLE_EQ(value, cast<double>(matrix(row, column)))
          << "## Get value test failed.";
    }
  }

  // Multiplication test
  auto multiplication = matrix * matrix;
  const double reference1[] = {56.0, 62.0, 68.0, 74.0,
                               152.0, 174.0, 196.0, 218.0,
                               248.0, 286.0, 324.0, 362.0,
                               344.0, 398.0, 452.0, 506.0};
  for (int row = 0; row < 4; ++row) {
    for (int column = 0; column < 4; ++column) {
      int index = row * 4 + column;
      ASSERT_DOUBLE_EQ(reference1[index], multiplication(row, column))
          << "## Multiplication test failed.";
    }
  }

  zisc::Vector<Float, 4> vector{1.0, 2.0, 3.0, 4.0};
  auto multiplication_vector = matrix * vector;
  const double reference2[] = {20.0, 60.0, 100.0, 140.0};
  for (int i = 0; i < 4; ++i) {
    ASSERT_DOUBLE_EQ(reference2[i], cast<double>(multiplication_vector[i]))
        << "## Matrix-vector multiplication test failed.";
  }
}

TEST(MatrixTest, FloatTest)
{
  testMatrix3x3<float>();
  testMatrix4x4<float>();
}

TEST(MatrixTest, DoubleTest)
{
  testMatrix3x3<double>();
  testMatrix4x4<double>();
}

TEST(MatrixTest, MultiplicationTest)
{
  zisc::Matrix<double, 3, 2> matrix1{1.0, 2.0,
                                     3.0, 4.0,
                                     5.0, 6.0};
  zisc::Matrix<double, 2, 1> matrix2{2.0,
                                     3.0};
  const auto result1 = matrix1 * matrix2;

  ASSERT_DOUBLE_EQ(8.0, result1(0, 0));
  ASSERT_DOUBLE_EQ(18.0, result1(1, 0));
  ASSERT_DOUBLE_EQ(28.0, result1(2, 0));
}

TEST(MatrixTest, DeterminantTest)
{
  using Matrix2x2 = zisc::Matrix<double, 2, 2>;
  using Matrix3x3 = zisc::Matrix<double, 3, 3>;
  using Matrix4x4 = zisc::Matrix<double, 4, 4>;

  const Matrix2x2 matrix1{ 2.0, 1.0,
                          -1.0, 3.0};
  ASSERT_DOUBLE_EQ(7.0, matrix1.determinant());

  const Matrix2x2 matrix2{1.0,-4.0,
                          2.0,-2.0};
  ASSERT_DOUBLE_EQ(6.0, matrix2.determinant());

  const Matrix3x3 matrix3{ 2.0, 3.0, 0.0,
                           5.0, 4.0, 1.0,
                          -2.0,-3.0,-1.0};
  ASSERT_DOUBLE_EQ(7.0, matrix3.determinant());

  const Matrix3x3 matrix4{1.0, 0.0, 0.0,
                          2.0,-1.0, 3.0,
                          1.0,-1.0, 1.0};
  ASSERT_DOUBLE_EQ(2.0, matrix4.determinant());

  const Matrix4x4 matrix5{2.0,-1.0, 3.0, 4.0,
                          0.0, 1.0, 2.0, 3.0,
                          3.0, 2.0, 4.0, 6.0,
                          0.0, 1.0, 1.0, 1.0};
  ASSERT_DOUBLE_EQ(9.0, matrix5.determinant());

  const Matrix4x4 matrix6{1.0, 2.0, 0.0, 0.0,
                          1.0, 1.0, 2.0, 3.0,
                          2.0, 2.0, 3.0, 4.0,
                          1.0, 2.0, 3.0, 4.0};
  ASSERT_DOUBLE_EQ(-2.0, matrix6.determinant());
}

TEST(MatrixTest, ConversionTest)
{
  using Matrix2x2 = zisc::Matrix<double, 2, 2>;
  using Matrix3x3 = zisc::Matrix<double, 3, 3>;
  using Matrix4x4 = zisc::Matrix<double, 4, 4>;

  // Transposed matrix test
  const Matrix4x4 matrix1{ 1.0,  2.0,  3.0,  4.0,
                           5.0,  6.0,  7.0,  8.0,
                           9.0, 10.0, 11.0, 12.0,
                          13.0, 14.0, 15.0, 16.0};
  const Matrix4x4 matrix2{  1.0,  5.0,  9.0, 13.0,
                            2.0,  6.0, 10.0, 14.0,
                            3.0,  7.0, 11.0, 15.0,
                            4.0,  8.0, 12.0, 16.0};
  const auto transposed_matrix = matrix1.transposedMatrix();
  for (zisc::uint row = 0; row < 4; ++row) {
    for (zisc::uint column = 0; column < 4; ++column) {
      ASSERT_DOUBLE_EQ(matrix2(row, column), transposed_matrix(row, column));
    }
  }

  // Cofactor matrix test
  const Matrix3x3 matrix3{1.0, 2.0, 3.0,
                          6.0, 5.0, 4.0,
                          7.0, 8.0, 9.0};
  const Matrix3x3 matrix4{13.0, -26.0, 13.0,
                          6.0, -12.0, 6.0,
                          -7.0, 14.0, -7.0};
  const auto cofactor_matrix = matrix3.cofactorMatrix();
  for (zisc::uint row = 0; row < 3; ++row) {
    for (zisc::uint column = 0; column < 3; ++column) {
      ASSERT_DOUBLE_EQ(matrix4(row, column), cofactor_matrix(row, column));
    }
  }

  // Inverse matrix test
  const Matrix3x3 matrix5{1.0, 2.0, 1.0,
                          1.0, 0.0, 3.0,
                          2.0, 1.0, 1.0};
  const auto matrix6 = (1.0 / 8.0) * Matrix3x3{-3.0,-1.0, 6.0,
                                                5.0,-1.0,-2.0,
                                                1.0, 3.0,-2.0};
  const auto inverse_matrix1 = matrix5.inverseMatrix();
  for (zisc::uint row = 0; row < 3; ++row) {
    for (zisc::uint column = 0; column < 3; ++column) {
      ASSERT_DOUBLE_EQ(matrix6(row, column), inverse_matrix1(row, column));
    }
  }

  const Matrix2x2 matrix7{4.0, 5.0,
                          2.0, 3.0};
  const auto matrix8 = (1.0 / 2.0) * Matrix2x2{ 3.0,-5.0,
                                                -2.0, 4.0};
  const auto inverse_matrix2 = matrix7.inverseMatrix();
  for (zisc::uint row = 0; row < 2; ++row) {
    for (zisc::uint column = 0; column < 2; ++column) {
      ASSERT_DOUBLE_EQ(matrix8(row, column), inverse_matrix2(row, column));
    }
  }
}
