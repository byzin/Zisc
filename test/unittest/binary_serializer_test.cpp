/*!
  \file binary_serializer_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstddef>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
// GoogleTest
#include "googletest.hpp"
// Zisc
#include "zisc/binary_serializer.hpp"
#include "zisc/zisc_config.hpp"

namespace {

struct TestData
{
  zisc::int8b c_[4];
  zisc::uint32b u_;
  double d_;
};
static_assert(std::alignment_of_v<TestData> == 8);
static_assert(sizeof(TestData) == 16);

void testDistance(const std::streamsize expected, std::istream* data_stream) noexcept
{
  const bool good = data_stream->good();
  const bool eof = data_stream->eof();
  const bool fail = data_stream->fail();
  const bool bad = data_stream->bad();
  const std::streamsize distance = zisc::BSerializer::getDistance(data_stream);
  ASSERT_EQ(expected, distance);
  ASSERT_EQ(good, data_stream->good());
  ASSERT_EQ(eof, data_stream->eof());
  ASSERT_EQ(fail, data_stream->fail());
  ASSERT_EQ(bad, data_stream->bad());
}

} // namespace

TEST(BinarySerializerTest, ReadWriteTest)
{
  std::stringstream data_stream{std::ios_base::in |
                                std::ios_base::out |
                                std::ios_base::binary};

  std::size_t data_size = 0;
  // Test "write"
  const zisc::int32b data1 = 10;
  zisc::BSerializer::write(&data1, &data_stream);
  data_size += sizeof(data1);
  const float data2 = 10.0f;
  zisc::BSerializer::write(&data2, &data_stream);
  data_size += sizeof(data2);
  const std::string data3{"BinarySerializerTest"};
  zisc::BSerializer::write(data3.data(), &data_stream, data3.size());
  data_size += data3.size();
  const ::TestData data4{{'a', 'b', 'c', 'd'}, 1, 3.14};
  zisc::BSerializer::write(&data4, &data_stream);
  data_size += sizeof(data4);


  zisc::BSerializer::backToBegin(&data_stream);


  // Test "read"
  ::testDistance(data_size, &data_stream);
  {
    zisc::int32b result = 0;
    zisc::BSerializer::read(&result, &data_stream);
    ASSERT_EQ(data1, result);
  }
  data_size -= sizeof(data1);
  testDistance(data_size, &data_stream);
  {
    float result = 0.0f;
    zisc::BSerializer::read(&result, &data_stream);
    ASSERT_FLOAT_EQ(data2, result);
  }
  data_size -= sizeof(data2);
  ::testDistance(data_size, &data_stream);
  {
    std::string result;
    result.resize(data3.size());
    zisc::BSerializer::read(&result[0], &data_stream, result.size());
    ASSERT_EQ(data3, result);
  }
  data_size -= data3.size();
  ::testDistance(data_size, &data_stream);
  {
    ::TestData result;
    zisc::BSerializer::read(&result, &data_stream);
    for (int i = 0; i < 4; ++i)
      ASSERT_EQ(data4.c_[i], result.c_[i]);
    ASSERT_EQ(data4.u_, result.u_);
    ASSERT_DOUBLE_EQ(data4.d_, result.d_);
  }
  data_size = 0;
  ::testDistance(data_size, &data_stream);
}

TEST(BinarySerializerTest, StreamToVectorTest)
{
  const std::vector<char> data{{'a', 'b', 'c', 'd',
                                'e', 'f', 'g', 'h',
                                'i', 'j', 'k', 'l',
                                'm', 'n', 'o', 'p',
                                'q', 'r', 's', 't',
                                'u', 'v', 'w', 'x',
                                'y', 'z'}};

  std::stringstream data_stream{std::ios_base::in |
                                std::ios_base::out |
                                std::ios_base::binary};
  for (auto c : data)
    data_stream << c;

  const auto distance = zisc::BSerializer::getDistance(&data_stream);
  ASSERT_EQ(data.size(), distance);

  std::vector<char> result;
  result.resize(distance);
  zisc::BSerializer::read(result.data(), &data_stream, distance);

  ASSERT_EQ(data.size(), result.size());
  for (std::size_t i = 0; i < data.size(); ++i)
    ASSERT_EQ(data[i], result[i]);
}
