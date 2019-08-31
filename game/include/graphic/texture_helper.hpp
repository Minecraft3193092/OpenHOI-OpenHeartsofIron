// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <hoibase/helper/os.hpp>

#ifdef OPENHOI_OS_WINDOWS
#  define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#include <Ogre.h>
#ifdef OPENHOI_OS_WINDOWS
#  undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

namespace openhoi {

class TextureHelper final {
 public:
  // Gets the width/height ratio of the given texture
  static Ogre::Real getTextureWidthHeightRatio(Ogre::TexturePtr texture);

  // Gets the width/height ratio of the given texture
  static Ogre::Real getTextureWidthHeightRatio(
      std::string textureName,
      std::string resourceGroupName = Ogre::RGN_DEFAULT);

  // Extracts the texture from a simple material. It is assumed the the texture
  // is inside the first pass of the first technique of the material
  static Ogre::TexturePtr extractTextureFromSimpleMaterial(
      Ogre::MaterialPtr material,
      std::string resourceGroupName = Ogre::RGN_DEFAULT);

  // Extracts the texture from a simple material. It is assumed the the texture
  // is inside the first pass of the first technique of the material
  static Ogre::TexturePtr extractTextureFromSimpleMaterial(
      std::string materialName,
      std::string resourceGroupName = Ogre::RGN_DEFAULT);
};

}  // namespace openhoi