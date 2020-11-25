/*!
  \file main.cpp
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
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string_view>
// Zisc
#include "zisc/binary_serializer.hpp"
// Test
#include "basic_test.hpp"
#include "error_function_test.hpp"
#include "float_manipulation_test.hpp"
#include "nearest_integer_test.hpp"
#include "power_test.hpp"

int main(int /* argc */, char** /* argv */)
{
  auto compress_file = [](std::string_view file_path) noexcept
  {
    char command[1024] = "";
    std::sprintf(command, "tar -Jcvf %s.tar.xz %s", file_path.data(),
                                                    file_path.data());
    std::system(command);
  };

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
