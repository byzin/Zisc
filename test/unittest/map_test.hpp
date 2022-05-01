/*!
  \file map_test.hpp
  \author Sho Ikeda
  \brief No brief description

  \details
  No detailed description.

  \copyright
  Copyright (c) 2015-2022 Sho Ikeda
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
  */

#ifndef TEST_MAP_TEST_HPP
#define TEST_MAP_TEST_HPP

// Zisc
#include "zisc/zisc_config.hpp"
#include "zisc/structure/map.hpp"

namespace test {

// Forward declaration
class MovableMValue;

//!
template <typename MapClass>
void testSimpleBoundedMap(zisc::Map<MapClass, int>* map);

//!
template <typename MapClass>
void testMovableValueMap(zisc::Map<MapClass, int, MovableMValue>* map);

//!
template <typename MapClass>
void testTinyCapacityMap(zisc::Map<MapClass, int>* map);

} /* namespace test */

#include "map_test-inl.hpp"

#endif /* TEST_SEARCH_TREE_TEST_HPP */
