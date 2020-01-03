// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include <exception>
#include <string>

#include "hoibase/helper/library.hpp"

namespace openhoi {

class OPENHOI_LIB_EXPORT Debug final {
 public:
  // Pretty print exception with stacktrace
  static std::string prettyPrintException(const std::exception* ex);
};

}  // namespace openhoi