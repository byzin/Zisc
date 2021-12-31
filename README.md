# Zisc #

Zisc provides C++ utility classes and functions which can be commonly used.

Zisc is a library separated from [Zivc][zivc] for easy maintenance.

**Z**ivc m**isc**ellaneous C++ library

Version: 0.0.68

## Import instructions ##

### Prerequisites ###

* C++20 support compiler
* [CMake][cmake] (version 3.22 or later)

### Importing ###

Clone the Zisc repository and include the cmake config files of Zisc,

```cmake
  # Determine a configuration type between "Debug", "RelWithDebInfo" or "Release"
  set(CMAKE_CONFIGURATION_TYPES "Debug" "RelWithDebInfo" "Release")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
  endif()

  # Add the Zisc subdirectory
  add_subdirectory({"path-to-zisc-root"}/source/zisc ${PROJECT_BINARY_DIR}/Zisc)

  # Create an executable of your project
  add_executable({"target-name"} {"source-files"})

  # Link the Zisc library.
  # Note that compile features, options, definitions, link liraries, options and
  # include directories properties will be populated
  target_link_libraries({"target-name"} PRIVATE Zisc)
```

## Acknowledgement ##

I would like to thank the following projects.

* PCG: [PCG, A Family of Better Random Number Generators][pcg]
* Xoshiro\*: [xoshiro / xoroshiro generators and the PRNG shootout][xoshiro]
* Correlated Multi-Jittered: [Correlated Multi-Jittered Sampling][cmj]
* Lock-Free Bounded Queue: [A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue][scalable_portable_lock_free_queue]

## License ##

This software is released under the MIT License,
see [LICENSE.md](LICENSE.md).

[zivc]: https://github.com/byzin/Zivc
[cmake]: https://cmake.org/
[pcg]: http://www.pcg-random.org/
[xoshiro]: http://xoshiro.di.unimi.it/
[cmj]: https://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf
[scalable_portable_lock_free_queue]: https://arxiv.org/abs/1908.04511
