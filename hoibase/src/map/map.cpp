// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/map/map.hpp"

#include <cassert>

namespace openhoi {

// Map constructor
Map::Map(int radius) { this->radius = radius; }

// Add province to map
void Map::AddProvince(Province province) {
  // Check for duplicate provinces
  assert(this->provinces.find(province.GetID()) == this->provinces.end());

  // Add the province
  this->provinces.insert({province.GetID(), province});
}

// Gets the map's provinces
std::unordered_map<std::string, Province> const& Map::GetProvinces() const {
  return this->provinces;
}

// Gets the map's radius
int const& Map::GetRadius() const { return this->radius; }

}  // namespace openhoi