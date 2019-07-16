// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/file/file_access.hpp"
#include "hoibase/helper/library.hpp"
#include "hoibase/map/map.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include <rapidjson/document.h>
#include <memory>

namespace openhoi {

class MapFactory {
 public:
  // Loads the map file and returns the map data
  OPENHOI_LIB_EXPORT static std::unique_ptr<Map> LoadMap(std::string path);

 private:
  // Get coordinates out of object
  static std::vector<OPENHOI_PROVINCE_POINT> GetCoordinates(
      rapidjson::Value& value);
};

}  // namespace openhoi