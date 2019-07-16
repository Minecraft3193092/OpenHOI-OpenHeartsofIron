// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <OgreVector2.h>
#include <string>
#include <vector>

namespace openhoi {

typedef Ogre::Vector2 OPENHOI_PROVINCE_POINT;

// Represents a map's province
class Province final {
 public:
  // Province constructor
  Province(std::string id,
           std::vector<std::vector<OPENHOI_PROVINCE_POINT>> coordinates,
           OPENHOI_PROVINCE_POINT center);

  // Gets the province ID
  OPENHOI_LIB_EXPORT std::string const& GetID() const;

  // Gets the province coordinates
  OPENHOI_LIB_EXPORT std::vector<std::vector<OPENHOI_PROVINCE_POINT>> const&
  GetCoordinates() const;

  // Gets the vertices of the triangulated province
  OPENHOI_LIB_EXPORT std::vector<float> const GetTriangulatedVertices() const;

  // Gets the province center point
  OPENHOI_LIB_EXPORT OPENHOI_PROVINCE_POINT const& GetCenter() const;

 private:
  std::string id;
  std::vector<std::vector<OPENHOI_PROVINCE_POINT>> coordinates;
  OPENHOI_PROVINCE_POINT center;
};

}  // namespace openhoi