# Zisc #

Zisc provides utility C++ classes and functions which can be commonly used.

**Z** in's m **isc** ellaneous C++ library

Version: 0.0.59

## Import instructions ##

### Prerequisites ###

* C++17 support compiler
* [CMake](https://cmake.org/) (version 3.16 or later)

### Importing ###

Clone the Zisc repository and include the cmake config files of Zisc,

```cmake
  # Determine a configuration type between "Debug", "RelWithDebInfo" or "Release"
  set(CMAKE_CONFIGURATION_TYPES "Debug" "RelWithDebInfo" "Release")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
  endif()

  # Add the Zisc subdirectory
  add_subdirectory("path-to-zisc-root" ${PROJECT_BINARY_DIR}/Zisc)

  # Create an executable of your project
  add_executable("target-name" "source-files")

  # Link the Zisc library.
  # Note that compile features, options, definitions, link liraries, options
  # and include directories properties will be populated
  target_link_libraries("target-name" PRIVATE Zisc)
```

## Acknowledgement ##

I would like to thank the following projects.

* PCG: [PCG, A Family of Better Random Number Generators](http://www.pcg-random.org/)
* Xoshiro\*: [xoshiro / xoroshiro generators and the PRNG shootout](http://xoshiro.di.unimi.it/)
* Correlated Multi-Jittered: [Correlated Multi-Jittered Sampling](https://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf)
* Lock-Free Bounded Queue: [A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue](https://arxiv.org/abs/1908.04511)

## License ##

This software is released under the MIT License,
see [LICENSE.txt](LICENSE.txt).
