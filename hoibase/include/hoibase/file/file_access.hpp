// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/file/filesystem.hpp"
#include "hoibase/helper/library.hpp"
#include "hoibase/openhoi.hpp"

#include <string>

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

  // Cached OGRE plugin directory
  static filesystem::path ogrePluginDirectory;
};

}  // namespace openhoi