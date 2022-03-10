# Zisc #

Zisc provides C++ utility classes and functions which can be commonly used.

Zisc is a library separated from [Zisc][zisc] for easy maintenance.

**Z**ivc m**isc**ellaneous C++ library

Version: 0.0.70

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

Please see the [document/acknowledgement.md](document/acknowledgement.md).

## License ##

This software is released under the MIT License,
see [LICENSE.md](LICENSE.md).

[zisc]: https://github.com/byzin/Zisc
[cmake]: https://cmake.org/
