// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/map/map.hpp"

#include <cassert>

namespace openhoi {

// Map constructor
Map::Map(int radius) { this->radius = radius; }

// Add province to map
void Map::addProvince(Province province) {
  // Check for duplicate provinces
  assert(provinces.find(province.getID()) == provinces.end());

  // Add the province
  provinces.insert({province.getID(), province});
}

// Gets the map's provinces
std::unordered_map<std::string, Province> const& Map::getProvinces() const {
  return provinces;
}

// Gets the map's radius
int const& Map::getRadius() const { return radius; }

}  // namespace openhoi