// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"
#include "province.hpp"

#include <unordered_map>

namespace openhoi {

// Represents a map
class Map final {
 public:
  // Map constructor
  OPENHOI_LIB_EXPORT Map(int radius);

  // Add province to map
  OPENHOI_LIB_EXPORT void addProvince(Province province);

  // Gets the map's provinces
  OPENHOI_LIB_EXPORT std::unordered_map<std::string, Province> const& getProvinces() const;

  // Gets the map's radius
  OPENHOI_LIB_EXPORT int const& getRadius() const;

 private:
  int radius;
  std::unordered_map<std::string, Province> provinces;
};

}  // namespace openhoi