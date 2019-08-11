// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/file/filesystem.hpp"
#include "hoibase/helper/library.hpp"
#include "hoibase/openhoi.hpp"

#include <string>

// The OGRE STBI codec plugin name
#define OGRE_PLUGIN_STBI "Codec_STBI"

// The OGRE Direct3D 11 render system plugin name
#define OGRE_PLUGIN_RS_D3D11 "RenderSystem_Direct3D11"

// The OGRE OpenGL 3+ render system plugin name
#define OGRE_PLUGIN_RS_GL3PLUS "RenderSystem_GL3Plus"

// The OGRE legacy OpenGL render system plugin name
#define OGRE_PLUGIN_RS_GL "RenderSystem_GL"

// Macro the build a dynamic object's name (=dynamic objects are e.g. materials
// built by code)
#define BUILD_DYNAMIC_OBJECT_NAME(materialName) \
  (OPENHOI_GAME_NAME "__dyn__" materialName)

namespace openhoi {

// Visual Studio warnings
#ifdef _MSC_VER
#  pragma warning( \
      disable : 4251)  //  class 'std::filesystem::path' needs to have dll -
                       //  interface to be used by clients
#endif

class OPENHOI_LIB_EXPORT FileAccess final {
 public:
  // Get the game config directory. If it does not exist, this function also
  // tries to create it. In case something bad happens, an exception will be
  // thrown.
  static filesystem::path getUserGameConfigDirectory();

  // Get the game asset root directory. If it cannot be found, an exception will
  // be thrown.
  static filesystem::path getAssetRootDirectory();

  // Gets the OGRE plugin directory. In case the plugins should be located
  // relatively to the executables, an empty path is returned.
  static filesystem::path getOgrePluginDirectory();

 private:
  // Get the current user's home directory. If it cannot be found, an exception
  // will be thrown.
  static filesystem::path getUserHomeDirectory();

  // Cached game config directory
  static filesystem::path gameConfigDirectory;

  // Cached game asset root directory
  static filesystem::path gameAssetRootDirectory;

#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  // Cached OGRE plugin directory
  static filesystem::path ogrePluginDirectory;
#endif
};

}  // namespace openhoi