// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/map/map_factory.hpp"

#include <assert.h>
#include <boost/algorithm/string.hpp>
#include <unordered_set>

namespace openhoi {

// Loads the map file and returns the map data
std::unique_ptr<Map> MapFactory::LoadMap(std::string path) {
  // Generate map object
  std::unique_ptr<Map> map =
      std::make_unique<Map>(6378137 /* TODO: Add to feature collection */);

  // Load map file
  const Map* mapFile = NULL;  // TODO: Get from resource manager!

  // Open GeoJSON map file
  rapidjson::Document doc;
  /*if (doc.Parse(reinterpret_cast<const char*>(mapFile->GetBytes()),
                mapFile->GetSize())
          .HasParseError())
    throw "Unable to parse map file";  // TODO: Proper error handling!
    */ // TODO: Remove comment!

  // Ensure that document is not an array
  if (!doc.IsObject())
    throw "Invalid map file (root object is array)";  // TODO: Proper error
                                                      // handling!

  // Check if type is FeatureCollection
  auto docType = doc.FindMember("type");
  if (docType == doc.MemberEnd() || !docType->value.IsString() ||
      std::string(docType->value.GetString()) != "FeatureCollection")
    throw "Invalid map file (root type is not FeatureCollection)";  // TODO:
                                                                    // Proper
                                                                    // error
                                                                    // handling!

  // Loop through all features
  if (!doc.HasMember("features"))
    throw "Invalid map file (no features found)";  // TODO: Error handling
  auto& features = doc["features"];
  if (!features.IsArray())
    throw "Invalid map file (features must be an array)";  // TODO: Error
                                                           // handling
  std::unordered_set<std::string> importedIds;
  for (auto it = features.Begin(); it != features.End(); ++it) {
    // Check if type is Feature
    auto type = it->FindMember("type");
    if (type == doc.MemberEnd() || !type->value.IsString() ||
        std::string(type->value.GetString()) != "Feature") {
      // TOOD: Log
      continue;
    }

    // Check for properties
    auto properties = it->FindMember("properties");
    if (properties == doc.MemberEnd() || !properties->value.IsObject()) {
      // TOOD: Log
      continue;
    }
    auto propObj = properties->value.GetObject();

    // Get province ID out of name field
    auto idField = propObj.FindMember("name");
    if (idField == propObj.MemberEnd() || !idField->value.IsString()) {
      // TOOD: Log
      continue;
    }
    std::string id = std::string(idField->value.GetString());
    if (id.empty()) {
      // TOOD: Log
      continue;
    } else if (importedIds.find(id) != importedIds.end()) {
      // TOOD: Log
      continue;
    }
    importedIds.insert(id);

    // Check if it is a multipolygon
    auto typeField = propObj.FindMember("type");
    bool multipolygon =
        typeField != propObj.MemberEnd() && typeField->value.IsString() &&
        std::string(typeField->value.GetString()) == "multipolygon";

    // Check for geometry
    auto geometry = it->FindMember("geometry");
    if (geometry == doc.MemberEnd() || !geometry->value.IsObject()) {
      // TOOD: Log
      continue;
    }
    auto geometryObj = geometry->value.GetObject();

    // Check for geometry type
    auto geomTypeField = geometryObj.FindMember("type");
    if (geomTypeField == geometryObj.MemberEnd() ||
        !geomTypeField->value.IsString() ||
        std::string(geomTypeField->value.GetString()) !=
            (multipolygon ? "MultiPolygon" : "LineString")) {
      // TOOD: Log
      continue;
    }

    // Get geometry coordinates
    auto coordinatesField = geometryObj.FindMember("coordinates");
    if (coordinatesField == geometryObj.MemberEnd() ||
        !coordinatesField->value.IsArray()) {
      // TOOD: Log
      continue;
    }
    auto& coords = coordinatesField->value;
    std::vector<std::vector<OPENHOI_PROVINCE_POINT>> coordinates;
    if (coords.IsArray()) {
      if (!multipolygon) {
        // Get coordinates of out geometry
        auto tmp = MapFactory::GetCoordinates(coords);
        if (!tmp.empty()) coordinates.push_back(tmp);
      } else {
        for (auto git1 = coords.Begin(); git1 != coords.End(); ++git1) {
          if (git1->IsArray()) {
            auto coords2 = git1->GetArray();
            for (auto git2 = coords2.Begin(); git2 != coords2.End(); ++git2) {
              // Get coordinates of out geometry
              auto tmp = MapFactory::GetCoordinates(git2[0]);
              if (!tmp.empty()) coordinates.push_back(tmp);
            }
          }
        }
      }
    }

    if (!coordinates.empty()) {
      // Build province and add it to map
      Province province(id, coordinates,
                        OPENHOI_PROVINCE_POINT(0, 0));  // TODO: Center point
      map->AddProvince(province);
    }
  }

  // Return map
  return map;
}

// Get the coordinates for one single way
std::vector<OPENHOI_PROVINCE_POINT> MapFactory::GetCoordinates(
    rapidjson::Value& value) {
  std::vector<OPENHOI_PROVINCE_POINT> coords;

  if (value.IsArray()) {
    for (auto it = value.Begin(); it != std::prev(value.End()); ++it) {
      if (it->IsArray()) {
        auto coordArray = it->GetArray();
        if (coordArray.Size() > 1) {
          auto& lat = coordArray[0];
          auto& lon = coordArray[1];
          if (lat.IsDouble() && lon.IsDouble())
            coords.push_back(OPENHOI_PROVINCE_POINT(
                (Ogre::Real)lat.GetDouble(), (Ogre::Real)lon.GetDouble()));
        }
      }
    }
  }

  return coords;
}

}  // namespace openhoi