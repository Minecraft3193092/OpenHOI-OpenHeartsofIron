// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/file/filesystem.hpp"
#include "hoibase/helper/library.hpp"
#include "hoibase/openhoi.hpp"

#include <string>

#define OPENHOI_GENERAL_RESOURCE_GROUP "openhoi_general"

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
  static filesystem::path GetUserGameConfigDirectory();

  // Get the game asset root directory. If it cannot be found, an exception will
  // be thrown.
  static filesystem::path GetAssetRootDirectory();

 private:
  // Get the current user's home directory. If it cannot be found, an exception
  // will be thrown.
  static filesystem::path GetUserHomeDirectory();

  // Cached game config directory
  static filesystem::path gameConfigDirectory;

  // Cached game asset root directory
  static filesystem::path gameAssetRootDirectory;
};

}  // namespace openhoi