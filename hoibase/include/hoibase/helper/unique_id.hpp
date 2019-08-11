// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "library.hpp"

#include <string>

namespace openhoi {

// Generate a Universally Unique Identifier (UUID) as string
OPENHOI_LIB_EXPORT std::string generateUUID();

}  // namespace openhoi