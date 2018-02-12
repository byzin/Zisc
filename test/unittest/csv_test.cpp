/*!
  \file csv_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2018 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// GoogleTest
#include "gtest/gtest.h"
// Standard C++ library
#include <list>
#include <string>
#include <sstream>
// Zisc
#include "zisc/csv.hpp"

TEST(CsvTest, ParseCsvTest)
{
  using Csv = zisc::Csv<std::string, int, double, bool>;

  const auto pattern = Csv::csvPattern();
  std::cout << "CSV<string, int, double, bool> pattern: " << pattern << std::endl;

  std::istringstream csv_text;
  {
    std::ostringstream csv_output;
    csv_output << R"("test1",1,1.23,true)" << std::endl;
    csv_output << R"( " test2 " , 2 , 4.56 , false )" << std::endl;
    csv_text = std::istringstream{csv_output.str()};
  }

  Csv csv;
  std::list<std::string> message_list;
  csv.append(csv_text, &message_list);

  ASSERT_EQ(0u, message_list.size());

  EXPECT_STREQ("test1", csv.get<0>(0).c_str()) << "Parsing csv failed.";
  EXPECT_EQ(1, csv.get<1>(0)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(1.23, csv.get<2>(0)) << "Parsing csv failed.";
  EXPECT_TRUE(csv.get<3>(0)) << "Parsing csv failed.";

  EXPECT_STREQ(" test2 ", csv.get<0>(1).c_str()) << "Parsing csv failed.";
  EXPECT_EQ(2, csv.get<1>(1)) << "Parsing csv failed.";
  EXPECT_DOUBLE_EQ(4.56, csv.get<2>(1)) << "Parsing csv failed.";
  EXPECT_FALSE(csv.get<3>(1)) << "Parsing csv failed.";
}
