// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "helper/os.hpp"
#include "version.hpp"

// The game name
#define OPENHOI_GAME_NAME "openhoi"

// The game version
#define OPENHOI_GAME_VERSION \
  OPENHOI_VERSION_MAJOR "." OPENHOI_VERSION_MINOR "." OPENHOI_VERSION_PATCH

// URL to our official GIT repository
#define OPENHOI_GIT_URL "https://github.com/openhoi/openhoi"

// The default TCP game server port
#define OPENHOI_DEFAULT_SERVER_PORT 1936

// Resource group for flag textures
#define OPENHOI_RSG_FLAG_TEXTURES OPENHOI_GAME_NAME "_RSG_FLAG_TEXTURES"

// Resource group for coat of armes textures
#define OPENHOI_RSG_COA_TEXTURES OPENHOI_GAME_NAME "_RSG_COA_TEXTURES"

// Macro the build a dynamic object name (=dynamic objects are e.g. materials
// built by code)
#define OPENHOI_BUILD_DYNAMIC_OBJECT_NAME(objectName) \
  (OPENHOI_GAME_NAME "__dyn__" objectName)

// EOF