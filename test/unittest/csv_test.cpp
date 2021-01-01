/*!
  \file csv_test.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "googletest.hpp"
// Standard C++ library
#include <list>
#include <memory>
#include <string_view>
#include <sstream>
#include <utility>
// Zisc
#include "zisc/memory/simple_memory_resource.hpp"
#include "zisc/string/csv.hpp"

TEST(CsvTest, MoveTest)
{
  using Csv = zisc::Csv<std::string_view, int, double, bool>;
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

TEST(CsvTest, ParseCsvTest1)
{
  using Csv = zisc::Csv<std::string_view, int, double, bool>;

  const auto pattern = Csv::csvPattern();
  std::cout << "CSV<string_view, int, double, bool> pattern: " << pattern.toCStr()
            << std::endl;

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

  ASSERT_EQ(4, csv.columnSize());
  ASSERT_EQ(2, csv.rowSize());

  EXPECT_STREQ("test1", csv.get<0>(0).data()) << "Parsing csv failed.";
  EXPECT_EQ(1, csv.get<1>(0)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(1.23, csv.get<2>(0)) << "Parsing csv failed.";
  EXPECT_TRUE(csv.get<3>(0)) << "Parsing csv failed.";

  EXPECT_STREQ(" test2 ", csv.get<0>(1).data()) << "Parsing csv failed.";
  EXPECT_EQ(2, csv.get<1>(1)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(4.56, csv.get<2>(1)) << "Parsing csv failed.";
  EXPECT_FALSE(csv.get<3>(1)) << "Parsing csv failed.";
}

TEST(CsvTest, ParseCsvTest2)
{
  using Csv = zisc::Csv<char, unsigned long, float, const char*>;

  const auto pattern = Csv::csvPattern();
  std::cout << "CSV<char, ulong, float, char*> pattern: " << pattern.toCStr()
            << std::endl;

  std::istringstream csv_text;
  {
    std::ostringstream csv_output;
    csv_output << R"(12, 64, 3.14, "\t# CSVtest1 \t")" << std::endl;
    csv_output << R"(-100, 1, 4.56, " CSVtest2 ")" << std::endl;
    csv_text = std::istringstream{csv_output.str()};
  }

  zisc::SimpleMemoryResource mem_resource;
  Csv csv{&mem_resource};
  csv.append(csv_text);

  ASSERT_EQ(4, csv.columnSize());
  ASSERT_EQ(2, csv.rowSize());

  EXPECT_EQ(12, csv.get<0>(0)) << "Parsing csv failed.";
  EXPECT_EQ(64ul, csv.get<1>(0)) << "Parsing csv failed.";
  EXPECT_FLOAT_EQ(3.14f, csv.get<2>(0)) << "Parsing csv failed.";
  EXPECT_STREQ("\t# CSVtest1 \t", csv.get<3>(0)) << "Parsing csv failed.";

  EXPECT_EQ(-100, csv.get<0>(1)) << "Parsing csv failed.";
  EXPECT_EQ(1ul, csv.get<1>(1)) << "Parsing csv failed.";
  EXPECT_FLOAT_EQ(4.56f, csv.get<2>(1)) << "Parsing csv failed.";
  EXPECT_STREQ(" CSVtest2 ", csv.get<3>(1)) << "Parsing csv failed.";
}
