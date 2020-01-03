// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <string>

#include "library.hpp"

namespace openhoi {

class OPENHOI_LIB_EXPORT UniqueID final {
 public:
  // Generate a Universally Unique Identifier (UUID) as string
  static std::string generate();
};

}  // namespace openhoi