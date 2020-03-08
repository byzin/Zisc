# Zisc #

Zisc provides commonly used C++ classes and functions.

**Z** in's m **isc** ellaneous C++ library

Version: 0.0.56

## Import instructions ##

### Prerequisites ###

* C++17 support compiler
* [CMake](https://cmake.org/) (version 3.14 or later)

### Importing ###

Clone Zisc repository and include the cmake config files of Zisc,

```cmake
  include(${zisc_root}/cmake/project.cmake)
  initPlatform(platform_definitions)

  include(${zisc_root}/cmake/option.cmake)
  initCommandOptions()

  include(${zisc_root}/cmake/compiler.cmake)
  initCompilerOptions()
  getCxxCompilerFlags(cxx_compiler_flags
                      cxx_linker_flags
                      cxx_definitions)

  include(${zisc_root}/source/zisc/config.cmake)
  loadZisc(zisc_header_files
           zisc_include_dirs
           zisc_compile_flags
           zisc_linker_flags
           zisc_definitions)
```

`${zisc_root}` is the path to Zisc root directory.
Then, add loaded properties `zisc_*` to your project.

## Acknowledgement ##

I would like to thank the following projects.

* PCG: [PCG, A Family of Better Random Number Generators](http://www.pcg-random.org/)
* Xoshiro\*: [xoshiro / xoroshiro generators and the PRNG shootout](http://xoshiro.di.unimi.it/)
* Correlated Multi-Jittered: [Correlated Multi-Jittered Sampling](https://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf)
* Lock-Free Bounded Queue: [A Scalable, Portable, and Memory-Efficient Lock-Free FIFO Queue](https://arxiv.org/abs/1908.04511)

## License ##

This software is released under the MIT License,
see [LICENSE.txt](LICENSE.txt).
