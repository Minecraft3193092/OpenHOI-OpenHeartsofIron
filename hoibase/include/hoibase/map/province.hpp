// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <Ogre.h>

#include <string>
#include <vector>

namespace openhoi {

// Represents a map's province
class Province final {
 public:
  // Province constructor
  OPENHOI_LIB_EXPORT Province(
      std::string id, std::vector<std::vector<Ogre::Vector2>> coordinates,
      Ogre::Vector2 center);

  // Gets the province ID
  OPENHOI_LIB_EXPORT std::string const& getID() const;

  // Gets the province coordinates
  OPENHOI_LIB_EXPORT std::vector<std::vector<Ogre::Vector2>> const&
  getCoordinates() const;

  // Gets the vertices of the triangulated province
  OPENHOI_LIB_EXPORT std::vector<Ogre::Real> const getTriangulatedVertices()
      const;

  // Gets the province center point
  OPENHOI_LIB_EXPORT Ogre::Vector2 const& getCenter() const;

 private:
  std::string id;
  std::vector<std::vector<Ogre::Vector2>> coordinates;
  Ogre::Vector2 center;
};

}  // namespace openhoi