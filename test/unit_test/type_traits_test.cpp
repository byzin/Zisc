/*!
  \file type_traits_test.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2016 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <list>
#include <string>
#include <vector>
// GoogleTest
#include "gtest/gtest.h"
// Zisc
#include "zisc/type_traits.hpp"

TEST(TypeTraitsTest, IsBooleanTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsBoolean<void>);
  EXPECT_FALSE(zisc::kIsBoolean<void*>);
  EXPECT_TRUE(zisc::kIsBoolean<bool>);
  EXPECT_FALSE(zisc::kIsBoolean<bool*>);
  EXPECT_FALSE(zisc::kIsBoolean<char>);
  EXPECT_FALSE(zisc::kIsBoolean<char*>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned char>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned char*>);
  EXPECT_FALSE(zisc::kIsBoolean<short>);
  EXPECT_FALSE(zisc::kIsBoolean<short*>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned short>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned short*>);
  EXPECT_FALSE(zisc::kIsBoolean<int>);
  EXPECT_FALSE(zisc::kIsBoolean<int*>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned int>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned int*>);
  EXPECT_FALSE(zisc::kIsBoolean<long>);
  EXPECT_FALSE(zisc::kIsBoolean<long*>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned long>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned long*>);
  EXPECT_FALSE(zisc::kIsBoolean<long long>);
  EXPECT_FALSE(zisc::kIsBoolean<long long*>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned long long>);
  EXPECT_FALSE(zisc::kIsBoolean<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsBoolean<float>);
  EXPECT_FALSE(zisc::kIsBoolean<float*>);
  EXPECT_FALSE(zisc::kIsBoolean<double>);
  EXPECT_FALSE(zisc::kIsBoolean<double*>);
  EXPECT_FALSE(zisc::kIsBoolean<long double>);
  EXPECT_FALSE(zisc::kIsBoolean<long double*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::string>);
  EXPECT_FALSE(zisc::kIsBoolean<std::string*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<int>>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<int>>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<int>*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<int>::iterator>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsBoolean<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<int>::iterator>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsBoolean<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsIntegerTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsInteger<void>);
  EXPECT_FALSE(zisc::kIsInteger<void*>);
  EXPECT_FALSE(zisc::kIsInteger<bool>);
  EXPECT_FALSE(zisc::kIsInteger<bool*>);
  EXPECT_TRUE(zisc::kIsInteger<char>);
  EXPECT_FALSE(zisc::kIsInteger<char*>);
  EXPECT_TRUE(zisc::kIsInteger<unsigned char>);
  EXPECT_FALSE(zisc::kIsInteger<unsigned char*>);
  EXPECT_TRUE(zisc::kIsInteger<short>);
  EXPECT_FALSE(zisc::kIsInteger<short*>);
  EXPECT_TRUE(zisc::kIsInteger<unsigned short>);
  EXPECT_FALSE(zisc::kIsInteger<unsigned short*>);
  EXPECT_TRUE(zisc::kIsInteger<int>);
  EXPECT_FALSE(zisc::kIsInteger<int*>);
  EXPECT_TRUE(zisc::kIsInteger<unsigned int>);
  EXPECT_FALSE(zisc::kIsInteger<unsigned int*>);
  EXPECT_TRUE(zisc::kIsInteger<long>);
  EXPECT_FALSE(zisc::kIsInteger<long*>);
  EXPECT_TRUE(zisc::kIsInteger<unsigned long>);
  EXPECT_FALSE(zisc::kIsInteger<unsigned long*>);
  EXPECT_TRUE(zisc::kIsInteger<long long>);
  EXPECT_FALSE(zisc::kIsInteger<long long*>);
  EXPECT_TRUE(zisc::kIsInteger<unsigned long long>);
  EXPECT_FALSE(zisc::kIsInteger<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsInteger<float>);
  EXPECT_FALSE(zisc::kIsInteger<float*>);
  EXPECT_FALSE(zisc::kIsInteger<double>);
  EXPECT_FALSE(zisc::kIsInteger<double*>);
  EXPECT_FALSE(zisc::kIsInteger<long double>);
  EXPECT_FALSE(zisc::kIsInteger<long double*>);
  EXPECT_FALSE(zisc::kIsInteger<std::string>);
  EXPECT_FALSE(zisc::kIsInteger<std::string*>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<int>>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<int>>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<int>*>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<int>::iterator>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsInteger<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<int>::iterator>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsInteger<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsUnsignedIntegerTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsUnsignedInteger<void>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<void*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<bool>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<bool*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<char>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<char*>);
  EXPECT_TRUE(zisc::kIsUnsignedInteger<unsigned char>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<unsigned char*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<short>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<short*>);
  EXPECT_TRUE(zisc::kIsUnsignedInteger<unsigned short>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<unsigned short*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<int>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<int*>);
  EXPECT_TRUE(zisc::kIsUnsignedInteger<unsigned int>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<unsigned int*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long*>);
  EXPECT_TRUE(zisc::kIsUnsignedInteger<unsigned long>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<unsigned long*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long long>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long long*>);
  EXPECT_TRUE(zisc::kIsUnsignedInteger<unsigned long long>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<float>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<float*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<double>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<double*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long double>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<long double*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::string>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::string*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<int>>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<int>>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<int>*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<int>::iterator>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<int>::iterator>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsUnsignedInteger<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsSignedIntegerTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsSignedInteger<void>);
  EXPECT_FALSE(zisc::kIsSignedInteger<void*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<bool>);
  EXPECT_FALSE(zisc::kIsSignedInteger<bool*>);
  EXPECT_TRUE(zisc::kIsSignedInteger<char>);
  EXPECT_FALSE(zisc::kIsSignedInteger<char*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned char>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned char*>);
  EXPECT_TRUE(zisc::kIsSignedInteger<short>);
  EXPECT_FALSE(zisc::kIsSignedInteger<short*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned short>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned short*>);
  EXPECT_TRUE(zisc::kIsSignedInteger<int>);
  EXPECT_FALSE(zisc::kIsSignedInteger<int*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned int>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned int*>);
  EXPECT_TRUE(zisc::kIsSignedInteger<long>);
  EXPECT_FALSE(zisc::kIsSignedInteger<long*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned long>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned long*>);
  EXPECT_TRUE(zisc::kIsSignedInteger<long long>);
  EXPECT_FALSE(zisc::kIsSignedInteger<long long*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned long long>);
  EXPECT_FALSE(zisc::kIsSignedInteger<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<float>);
  EXPECT_FALSE(zisc::kIsSignedInteger<float*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<double>);
  EXPECT_FALSE(zisc::kIsSignedInteger<double*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<long double>);
  EXPECT_FALSE(zisc::kIsSignedInteger<long double*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::string>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::string*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<int>>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<int>>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<int>*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<int>::iterator>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<int>::iterator>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsSignedInteger<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsFloatTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsFloat<void>);
  EXPECT_FALSE(zisc::kIsFloat<void*>);
  EXPECT_FALSE(zisc::kIsFloat<bool>);
  EXPECT_FALSE(zisc::kIsFloat<bool*>);
  EXPECT_FALSE(zisc::kIsFloat<char>);
  EXPECT_FALSE(zisc::kIsFloat<char*>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned char>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned char*>);
  EXPECT_FALSE(zisc::kIsFloat<short>);
  EXPECT_FALSE(zisc::kIsFloat<short*>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned short>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned short*>);
  EXPECT_FALSE(zisc::kIsFloat<int>);
  EXPECT_FALSE(zisc::kIsFloat<int*>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned int>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned int*>);
  EXPECT_FALSE(zisc::kIsFloat<long>);
  EXPECT_FALSE(zisc::kIsFloat<long*>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned long>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned long*>);
  EXPECT_FALSE(zisc::kIsFloat<long long>);
  EXPECT_FALSE(zisc::kIsFloat<long long*>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned long long>);
  EXPECT_FALSE(zisc::kIsFloat<unsigned long long*>);
  EXPECT_TRUE(zisc::kIsFloat<float>);
  EXPECT_FALSE(zisc::kIsFloat<float*>);
  EXPECT_TRUE(zisc::kIsFloat<double>);
  EXPECT_FALSE(zisc::kIsFloat<double*>);
  EXPECT_TRUE(zisc::kIsFloat<long double>);
  EXPECT_FALSE(zisc::kIsFloat<long double*>);
  EXPECT_FALSE(zisc::kIsFloat<std::string>);
  EXPECT_FALSE(zisc::kIsFloat<std::string*>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<int>>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<int>>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<int>*>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<int>::iterator>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsFloat<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<int>::iterator>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<std::string>::iterator>);
  EXPECT_FALSE(zisc::kIsFloat<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsIteratorTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsIterator<void>);
  EXPECT_FALSE(zisc::kIsIterator<void*>);
  EXPECT_FALSE(zisc::kIsIterator<bool>);
  EXPECT_TRUE(zisc::kIsIterator<bool*>);
  EXPECT_FALSE(zisc::kIsIterator<char>);
  EXPECT_TRUE(zisc::kIsIterator<char*>);
  EXPECT_FALSE(zisc::kIsIterator<unsigned char>);
  EXPECT_TRUE(zisc::kIsIterator<unsigned char*>);
  EXPECT_FALSE(zisc::kIsIterator<short>);
  EXPECT_TRUE(zisc::kIsIterator<short*>);
  EXPECT_FALSE(zisc::kIsIterator<unsigned short>);
  EXPECT_TRUE(zisc::kIsIterator<unsigned short*>);
  EXPECT_FALSE(zisc::kIsIterator<int>);
  EXPECT_TRUE(zisc::kIsIterator<int*>);
  EXPECT_FALSE(zisc::kIsIterator<unsigned int>);
  EXPECT_TRUE(zisc::kIsIterator<unsigned int*>);
  EXPECT_FALSE(zisc::kIsIterator<long>);
  EXPECT_TRUE(zisc::kIsIterator<long*>);
  EXPECT_FALSE(zisc::kIsIterator<unsigned long>);
  EXPECT_TRUE(zisc::kIsIterator<unsigned long*>);
  EXPECT_FALSE(zisc::kIsIterator<long long>);
  EXPECT_TRUE(zisc::kIsIterator<long long*>);
  EXPECT_FALSE(zisc::kIsIterator<unsigned long long>);
  EXPECT_TRUE(zisc::kIsIterator<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsIterator<float>);
  EXPECT_TRUE(zisc::kIsIterator<float*>);
  EXPECT_FALSE(zisc::kIsIterator<double>);
  EXPECT_TRUE(zisc::kIsIterator<double*>);
  EXPECT_FALSE(zisc::kIsIterator<long double>);
  EXPECT_TRUE(zisc::kIsIterator<long double*>);
  EXPECT_FALSE(zisc::kIsIterator<std::string>);
  EXPECT_TRUE(zisc::kIsIterator<std::string*>);
  EXPECT_FALSE(zisc::kIsIterator<std::vector<int>>);
  EXPECT_TRUE(zisc::kIsIterator<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsIterator<std::list<int>>);
  EXPECT_TRUE(zisc::kIsIterator<std::list<int>*>);
  EXPECT_TRUE(zisc::kIsIterator<std::vector<int>::iterator>);
  EXPECT_TRUE(zisc::kIsIterator<std::vector<int>::iterator*>);
  EXPECT_TRUE(zisc::kIsIterator<std::vector<std::string>::iterator>);
  EXPECT_TRUE(zisc::kIsIterator<std::vector<std::string>::iterator*>);
  EXPECT_TRUE(zisc::kIsIterator<std::list<int>::iterator>);
  EXPECT_TRUE(zisc::kIsIterator<std::list<int>::iterator*>);
  EXPECT_TRUE(zisc::kIsIterator<std::list<std::string>::iterator>);
  EXPECT_TRUE(zisc::kIsIterator<std::list<std::string>::iterator*>);
}

TEST(TypeTraitsTest, IsRandomAccessIteratorTest)
{
  // Boolean
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<void>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<void*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<bool>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<bool*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<char>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<char*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<unsigned char>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<unsigned char*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<short>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<short*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<unsigned short>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<unsigned short*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<int>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<int*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<unsigned int>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<unsigned int*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<long>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<long*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<unsigned long>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<unsigned long*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<long long>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<long long*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<unsigned long long>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<unsigned long long*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<float>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<float*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<double>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<double*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<long double>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<long double*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<std::string>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::string*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<std::vector<int>>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::vector<int>*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<std::list<int>>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::list<int>*>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::vector<int>::iterator>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::vector<int>::iterator*>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::vector<std::string>::iterator>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::vector<std::string>::iterator*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<std::list<int>::iterator>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::list<int>::iterator*>);
  EXPECT_FALSE(zisc::kIsRandomAccessIterator<std::list<std::string>::iterator>);
  EXPECT_TRUE(zisc::kIsRandomAccessIterator<std::list<std::string>::iterator*>);
}

template <typename Type>
bool testSfinae(const Type, zisc::EnableIfBoolean<Type> = zisc::kEnabler)
{
  return true;
}

template <typename Type>
bool testSfinae(const Type, zisc::EnableIfInteger<Type> = zisc::kEnabler)
{
  return false;
}

TEST(TypeTraitsTest, SfinaeTest)
{
  {
    const bool value = true;
    ASSERT_TRUE(testSfinae(value));
  }
  {
    const int value = 0;
    ASSERT_FALSE(testSfinae(value));
  }
}
