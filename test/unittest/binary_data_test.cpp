/*!
  \file binary_data_test.cpp
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
#include <sstream>
#include <string>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/binary_data.hpp"
#include "zisc/zisc_config.hpp"

struct TestData
{
  char c_[4];
  zisc::uint32b u_;
  double d_;
};

TEST(BinaryDataTest, ReadWriteTest)
{
  std::stringstream binary_stream{std::ios_base::in |
                                  std::ios_base::out |
                                  std::ios_base::binary};
  // Test "write"
  const zisc::int32b data1 = 10;
  zisc::write(&data1, &binary_stream);
  const std::string data2{"test"};
  zisc::write(data2.data(), &binary_stream, data2.size());
  const TestData data3{{'a', 'b', 'c', 'd'}, 1, 3.14};
  zisc::write(&data3, &binary_stream);

  binary_stream.seekg(0, std::ios_base::beg);

  // Test "read"
  {
    int result1 = 0;
    zisc::read(&result1, &binary_stream);
    ASSERT_EQ(data1, result1);
  }
  {
    std::string result2;
    result2.resize(data2.size());
    zisc::read(&result2[0], &binary_stream, result2.size());
    ASSERT_EQ(data2, result2);
  }
  {
    TestData result3;
    zisc::read(&result3, &binary_stream);
    for (int i = 0; i < 4; ++i)
      ASSERT_EQ(data3.c_[i], result3.c_[i]);
    ASSERT_EQ(data3.u_, result3.u_);
    ASSERT_DOUBLE_EQ(data3.d_, result3.d_);
  }
}
