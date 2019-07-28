// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <exception>
#include <string>

namespace openhoi {

class OPENHOI_LIB_EXPORT Debug final {
 public:
  // Pretty print exception with stacktrace
  static std::string PrettyPrintException(const std::exception* ex);
};

}  // namespace openhoi