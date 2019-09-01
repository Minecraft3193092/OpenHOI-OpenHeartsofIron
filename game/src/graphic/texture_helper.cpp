// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "graphic/texture_helper.hpp"

#ifdef OPENHOI_OS_WINDOWS
#  define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif
#include <OgreTexture.h>
#include <OgreTextureManager.h>
#ifdef OPENHOI_OS_WINDOWS
#  undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

namespace openhoi {

// Gets the width/height ratio of the given texture
Ogre::Real TextureHelper::getTextureWidthHeightRatio(Ogre::TexturePtr texture) {
  // Ensure that texture is loaded
  assert(texture.operator bool() && texture->isLoaded());

  // Calculate and return width/height ratio
  return (Ogre::Real)texture->getWidth() / texture->getHeight();
}

// Gets the width/height ratio of the given texture
Ogre::Real TextureHelper::getTextureWidthHeightRatio(
    std::string textureName,
    std::string resourceGroupName /* = Ogre::RGN_DEFAULT */) {
  // Get texture from texture manager
  Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(
      textureName, resourceGroupName);

  // Calculate and return width/height ratio
  return TextureHelper::getTextureWidthHeightRatio(texture);
}

// Extracts the texture from a simple material. It is assumed the the texture is
// inside the first pass of the first technique of the material
Ogre::TexturePtr TextureHelper::extractTextureFromSimpleMaterial(
    Ogre::MaterialPtr material,
    std::string resourceGroupName /* = Ogre::RGN_DEFAULT */) {
  // Ensure that material is loaded
  assert(material.operator bool() && material->isLoaded());

  // Get texture name and texture
  std::string textureName = material->getTechnique(0)
                                ->getPass(0)
                                ->getTextureUnitState(0)
                                ->getTextureName();
  return Ogre::TextureManager::getSingleton().getByName(textureName,
                                                        resourceGroupName);
}

// Extracts the texture from a simple material. It is assumed the the texture
// is inside the first pass of the first technique of the material
Ogre::TexturePtr TextureHelper::extractTextureFromSimpleMaterial(
    std::string materialName,
    std::string resourceGroupName /* = Ogre::RGN_DEFAULT */) {
  // Get material from material manager
  Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
      materialName, resourceGroupName);

  // Try to extract texture
  return TextureHelper::extractTextureFromSimpleMaterial(material,
                                                         resourceGroupName);
}

}  // namespace openhoi
