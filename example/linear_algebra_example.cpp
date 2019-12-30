/*!
  \file linear_algebra_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
// Zisc
#include "zisc/matrix.hpp"
#include "zisc/point.hpp"
#include "zisc/vector.hpp"
#include "zisc/zisc_config.hpp"

template <typename Arithmetic, zisc::uint kRow, zisc::uint kColumn>
void printMatrix(const zisc::Matrix<Arithmetic, kRow, kColumn>& matrix)
{
  for (zisc::uint row = 0; row < kRow; ++row) {
    std::cout << "|";
    for (zisc::uint column = 0; column < kColumn; ++column) {
      std::cout << " " << matrix(row, column);
    }
    std::cout << " |" << std::endl;
  }
}

template <typename Arithmetic, zisc::uint kN>
void printDimension(const zisc::Dimension<Arithmetic, kN>& dimension)
{
  std::cout << "(";
  for (zisc::uint i = 0; i < kN; ++i)
    std::cout << " " << dimension[i];
  std::cout << " )" << std::endl;
}

int main()
{
  // Matrix example
  std::cout << "## Matrix example" << std::endl;
  {
    zisc::Matrix<int, 2, 3> a{1, 2, 3, 
                              4, 5, 6};
    zisc::Matrix<int, 3, 2> b{1, 2, 
                              3, 4, 
                              5, 6};
    zisc::Matrix<double, 3, 3> c{2.0, 5.0, 1.0,
                                 9.0, 6.0, 4.0,
                                 2.0, 3.0, 8.0};
    std::cout << "Matrix a" << std::endl;
    printMatrix(a);
    std::cout << "Matrix b" << std::endl;
    printMatrix(b);
    std::cout << "Matrix c" << std::endl;
    printMatrix(c);
    std::cout << "b x a" << std::endl;
    printMatrix(a * b);
    std::cout << "Determinant of c" << std::endl;
    std::cout << c.determinant() << std::endl;
    std::cout << "Transposed matrix of c" << std::endl;
    printMatrix(c.transposedMatrix());
    std::cout << "Inverse matrix of c" << std::endl;
    printMatrix(c.inverseMatrix());
  }
  std::cout << std::endl;
  // Vector and point example
  std::cout << "## Vector and point example" << std::endl;
  {
    zisc::Vector<double, 3> v1{2.0, 5.0, 1.0};
    zisc::Vector<double, 3> v2{-4.0, 1.0, -3.0};
    zisc::Point<double, 3> p{0.0, 0.0, 0.0};
    std::cout << "Vector v1 = ";
    printDimension(v1);
    std::cout << "Vector v2 = ";
    printDimension(v2);
    std::cout << "Point p = ";
    printDimension(p);
    std::cout << "v1 + v2 = ";
    printDimension(v1 + v2);
    std::cout << "v1 - v2 = ";
    printDimension(v1 - v2);
    std::cout << "norm(v1) = " << v1.norm() << std::endl;
    std::cout << "normalized(v1) = ";
    printDimension(v1.normalized());
    std::cout << "v1 * v2 = " << zisc::dot(v1, v2) << std::endl;
    std::cout << "v1 x v2 = ";
    printDimension(zisc::cross(v1, v2));
    std::cout << "p1 + v1";
    printDimension(p + v1);
  }
}
