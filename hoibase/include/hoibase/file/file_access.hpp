// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/file/filesystem.hpp"
#include "hoibase/helper/library.hpp"
#include "hoibase/openhoi.hpp"

// The OGRE STBI codec plugin name
#define OGRE_PLUGIN_STBI "Codec_STBI"

// The OGRE ParticleFX plugin name
#define OGRE_PLUGIN_PARTICLEFX "Plugin_ParticleFX"

// The OGRE Direct3D 11 render system plugin name
#define OGRE_PLUGIN_RS_D3D11 "RenderSystem_Direct3D11"

// The OGRE OpenGL 3+ render system plugin name
#define OGRE_PLUGIN_RS_GL3PLUS "RenderSystem_GL3Plus"

// The OGRE legacy OpenGL render system plugin name
#define OGRE_PLUGIN_RS_GL "RenderSystem_GL"

namespace openhoi {

// Visual Studio warnings
#ifdef _MSC_VER
#  pragma warning( \
      disable : 4251)  //  class 'std::filesystem::path' needs to have dll -
                       //  interface to be used by clients
#endif

class OPENHOI_LIB_EXPORT FileAccess final {
 public:
  // Get the temp directory. If it cannot be found, an exception will be thrown.
  static filesystem::path getTempDirectory();

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

  // Gets the OGRE library and plugin extension. This can be something like
  // ".so.1.12.5-openhoi" or an empty string.
  static std::string getOgreLibraryFileExtension();

#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  // Gets the OGRE media directory. If it cannot be found, an exception will
  // be thrown.
  static filesystem::path getOgreMediaRootDirectory();
#endif

  // Read the provided file to the provided data pointer. The file length is
  // returned or -1 in case the file could not be read.
  static long readFile(filesystem::path file, unsigned char** data);

  // Custom fopen function for Windows compatibility
  static FILE* fopen(char const* fileName, char const* mode);

  // Custom fopen function for Windows compatibility
  static FILE* fopen(filesystem::path file, char const* mode);

 private:
  // Get the current user's home directory. If it cannot be found, an exception
  // will be thrown.
  static filesystem::path getUserHomeDirectory();

  // Cached game config directory
  static filesystem::path gameConfigDirectory;

  // Cached game asset root directory
  static filesystem::path gameAssetRootDirectory;

#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  // Gets the path where our used libOgreMain resides
  static filesystem::path getOgreMainLibPath();

  // Cached OGRE plugin directory
  static filesystem::path ogrePluginDirectory;

  // Cached OGRE media directory
  static filesystem::path ogreMediaDirectory;

  // Cached OGRE library file extension
  static std::string ogreLibraryFileExtension;
#endif
};

}  // namespace openhoi