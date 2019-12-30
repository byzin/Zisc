/*!
  \file stopwatch_example.cpp
  \author Sho Ikeda

  Copyright (c) 2015-2020 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

// Standard C++ library
#include <chrono>
#include <iostream>
#include <thread>
// Zisc
#include "zisc/stopwatch.hpp"

int main()
{
  // Stopwatch example
  std::cout << "## Stopwatch example" << std::endl;
  zisc::Stopwatch stopwatch;
  // First waiting
  std::cout << "Start to wating for 3 seconds" << std::endl;
  stopwatch.start();
  std::chrono::seconds wait_time{3};
  std::this_thread::sleep_for(wait_time);
  auto elapsed_time = 
      std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.pause());
  std::cout << "Time: " << elapsed_time.count() << " ms" << std::endl;
  // Second waiting
  std::cout << "Wait for more 2 seconds" << std::endl;
  stopwatch.start();
  wait_time = std::chrono::seconds{2};
  std::this_thread::sleep_for(wait_time);
  elapsed_time = 
      std::chrono::duration_cast<std::chrono::milliseconds>(stopwatch.elapsedTime());
  std::cout << "Time: " << elapsed_time.count() << " ms" << std::endl;
  // Finish
  stopwatch.stop();
  std::cout << "Finish" << std::endl;
  
  return 0;
}
