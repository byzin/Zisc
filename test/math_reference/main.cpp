/*!
  \file main.cpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2021 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string_view>
// Zisc
#include "zisc/binary_serializer.hpp"
// Test
#include "basic_test.hpp"
#include "error_function_test.hpp"
#include "exponential_test.hpp"
#include "float_manipulation_test.hpp"
#include "nearest_integer_test.hpp"
#include "power_test.hpp"
#include "trigonometric_test.hpp"

int main(int /* argc */, char** /* argv */)
{
  auto compress_file = [](std::string_view file_path) noexcept
  {
    char command[1024] = "";
    std::sprintf(command, "tar -Jcvf %s.tar.xz %s", file_path.data(),
                                                    file_path.data());
    std::system(command);
  };

  const bool is_debug = false;
  // Create x list
  {
    std::string_view file_path{"math_xonef_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeXList<float>(1.0f, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All one float x" << std::endl;
    auto x_list = makeXList<float, 100>(1.0f);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xoned_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeXList<double>(1.0, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All one double x" << std::endl;
    auto x_list = makeXList<double, 100>(1.0);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpowf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeXList<float>(kPowScale<float>, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All pow float x" << std::endl;
    auto x_list = makeXList<float, 100>(kPowScale<float>);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpowd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeXList<double>(kPowScale<double>, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All pow double x" << std::endl;
    auto x_list = makeXList<double, 100>(kPowScale<double>);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositivepowf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveXList<float>(kPowScale<float>, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All positive pow float x" << std::endl;
    auto x_list = makePositiveXList<float, 100>(kPowScale<float>);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositivepowd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveXList<double>(kPowScale<double>, &output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All positive pow double x" << std::endl;
    auto x_list = makePositiveXList<double, 100>(kPowScale<double>);
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xallf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeAllXList<float>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All normal float x" << std::endl;
    auto x_list = makeAllXList<float, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xall_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeAllSubnormalXList<float>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All subnormal float x" << std::endl;
    auto x_list = makeAllSubnormalXList<float, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xalld_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeAllXList<double>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All normal double x" << std::endl;
    auto x_list = makeAllXList<double, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xall_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makeAllSubnormalXList<double>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## All subnormal double x" << std::endl;
    auto x_list = makeAllSubnormalXList<double, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositivef_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveXList<float>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## Positive normal float x" << std::endl;
    auto x_list = makePositiveXList<float, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositive_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveSubnormalXList<float>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## Positive subnormal float x" << std::endl;
    auto x_list = makePositiveSubnormalXList<float, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositived_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveXList<double>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## Positive normal double x" << std::endl;
    auto x_list = makePositiveXList<double, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }
  {
    std::string_view file_path{"math_xpositive_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    makePositiveSubnormalXList<double>(&output);
    output.close();
    compress_file(file_path);
  }
  if (is_debug) {
    std::cout << "## Positive subnormal double x" << std::endl;
    auto x_list = makePositiveSubnormalXList<double, 100>();
    for (std::size_t i = 0; i < x_list.size(); ++i)
      std::cout << "  x[" << i << "] = " << std::scientific << x_list[i] << std::endl;
  }

  // Basic operations
  // Fma test
  {
    std::string_view file_path{"math_fmaf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFmaF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_fmad_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFmaD(&output);
    output.close();
    compress_file(file_path);
  }
  // Fmod test
  {
    std::string_view file_path{"math_fmodf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFmodF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_fmodd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFmodD(&output);
    output.close();
    compress_file(file_path);
  }

  // Power functions
  // Pow
  {
    std::string_view file_path{"math_powf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powintf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowIntF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_powintd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testPowIntD(&output);
    output.close();
    compress_file(file_path);
  }
  // Sqrt
  {
    std::string_view file_path{"math_sqrtf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSqrtF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sqrtd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSqrtD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sqrt_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSqrtSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sqrt_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSqrtSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  // Cbrt
  {
    std::string_view file_path{"math_cbrtf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCbrtF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cbrtd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCbrtD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cbrt_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCbrtSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cbrt_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCbrtSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  // Exponential functions
  {
    std::string_view file_path{"math_expf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expintf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpIntF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expintd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpIntD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_exp2f_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExp2F(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_exp2d_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExp2D(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_exp2intf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExp2IntF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_exp2intd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExp2IntD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expm1f_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpm1F(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expm1d_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpm1D(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expm1intf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpm1IntF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_expm1intd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testExpm1IntD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_logf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLogF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_logd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLogD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLogSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLogSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log10f_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog10F(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log10d_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog10D(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log10_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog10SubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log10_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog10SubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log2f_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog2F(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log2d_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog2D(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log2_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog2SubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log2_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog2SubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log1pf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog1pF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log1pd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog1pD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log1p_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog1pSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_log1p_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLog1pSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  // Trigonometric
  {
    std::string_view file_path{"math_sinf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sind_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sin_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sin_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cosf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCosF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cosd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCosD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cos_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCosSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cos_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCosSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tanf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tand_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tan_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tan_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asinf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asind_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asin_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asin_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acosf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcosF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acosd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcosD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acos_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcosSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acos_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcosSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atanf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atand_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atan_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atan_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atan2f_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtan2F(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atan2d_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtan2D(&output);
    output.close();
    compress_file(file_path);
  }
  // Hyperbolic
  {
    std::string_view file_path{"math_sinhf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinhF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sinhd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinhD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sinh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinhSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_sinh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testSinhSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_coshf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCoshF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_coshd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCoshD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cosh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCoshSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_cosh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCoshSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tanhf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanhF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tanhd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanhD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tanh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanhSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tanh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTanhSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asinhf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinhF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asinhd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinhD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asinh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinhSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_asinh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAsinhSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acoshf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcoshF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acoshd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcoshD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acosh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcoshSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_acosh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAcoshSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atanhf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanhF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atanhd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanhD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atanh_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanhSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_atanh_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testAtanhSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  // Error and gamma functions
  {
    std::string_view file_path{"math_erff_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erfd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erf_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erf_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erfcf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfcF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erfcd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfcD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erfc_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfcSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_erfc_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testErfcSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tgammaf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTgammaF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tgammad_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTgammaD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tgamma_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTgammaSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_tgamma_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTgammaSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_lgammaf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLgammaF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_lgammad_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLgammaD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_lgamma_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLgammaSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_lgamma_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testLgammaSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  // Nearest integer functions
  {
    std::string_view file_path{"math_rintf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRintF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_rintd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRintD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_rint_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRintSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_rint_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRintSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ceilf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCeilF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ceild_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCeilD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ceil_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCeilSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ceil_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testCeilSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_floorf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFloorF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_floord_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFloorD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_floor_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFloorSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_floor_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFloorSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_truncf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTruncF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_truncd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTruncD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_trunc_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTruncSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_trunc_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testTruncSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_roundf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRoundF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_roundd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRoundD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_round_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRoundSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_round_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testRoundSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  // Floating point manipulation functions
  {
    std::string_view file_path{"math_frldexpf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFrLdexpF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_frldexpd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFrLdexpD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_frldexp_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFrLdexpSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_frldexp_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testFrLdexpSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ilogbf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testIlogbF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ilogbd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testIlogbD(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ilogb_subf_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testIlogbSubnormalF(&output);
    output.close();
    compress_file(file_path);
  }
  {
    std::string_view file_path{"math_ilogb_subd_reference.txt"};
    std::ofstream output{file_path.data(), std::ios_base::binary};
    testIlogbSubnormalD(&output);
    output.close();
    compress_file(file_path);
  }

  return 0;
}
