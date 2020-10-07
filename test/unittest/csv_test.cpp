/*!
  \file csv_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Standard C++ library
#include <list>
#include <memory>
#include <string>
#include <sstream>
#include <utility>
// Zisc
#include "zisc/csv.hpp"
#include "zisc/memory/simple_memory_resource.hpp"

TEST(CsvTest, MoveTest)
{
  using Csv = zisc::Csv<std::string, int, double, bool>;
  zisc::SimpleMemoryResource mem_resource;
  std::unique_ptr<Csv> csv_ptr;
  {
    Csv csv{&mem_resource};
    csv.setCapacity(256);
    csv_ptr = std::make_unique<Csv>(std::move(csv));
  }
  ASSERT_EQ(256, csv_ptr->capacity()) << "Moving CSV data failed.";
  {
    Csv csv{&mem_resource};
    csv.setCapacity(1024);
    *csv_ptr = std::move(csv);
  }
  const auto& data = csv_ptr->data();
  ASSERT_EQ(1024, data.capacity()) << "Moving CSV data failed.";
}

TEST(CsvTest, ParseCsvTest)
{
  using Csv = zisc::Csv<std::string, int, double, bool>;

  const auto pattern = Csv::csvPattern();
  std::cout << "CSV<string, int, double, bool> pattern: " << pattern.toCString() << std::endl;

  std::istringstream csv_text;
  {
    std::ostringstream csv_output;
    csv_output << R"("test1",1,1.23,true)" << std::endl;
    csv_output << R"( " test2 " , 2 , 4.56 , false )" << std::endl;
    csv_text = std::istringstream{csv_output.str()};
  }

  zisc::SimpleMemoryResource mem_resource;
  Csv csv{&mem_resource};
  csv.append(csv_text);

  EXPECT_STREQ("test1", csv.get<0>(0).c_str()) << "Parsing csv failed.";
  EXPECT_EQ(1, csv.get<1>(0)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(1.23, csv.get<2>(0)) << "Parsing csv failed.";
  EXPECT_TRUE(csv.get<3>(0)) << "Parsing csv failed.";

  EXPECT_STREQ(" test2 ", csv.get<0>(1).c_str()) << "Parsing csv failed.";
  EXPECT_EQ(2, csv.get<1>(1)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(4.56, csv.get<2>(1)) << "Parsing csv failed.";
  EXPECT_FALSE(csv.get<3>(1)) << "Parsing csv failed.";
}
