/*!
  \file linear_algebra_example.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <iostream>
// Zisc
#include "zisc/matrix.hpp"
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
}
