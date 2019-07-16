// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <v8.h>

namespace openhoi {

// SpiderMonkey-powered JavaScript scripting runtime
class ScriptingRuntime final {
 public:
  // ScriptingRuntime constructor
  OPENHOI_LIB_EXPORT ScriptingRuntime();

  // ScriptingRuntime destructor
  OPENHOI_LIB_EXPORT ~ScriptingRuntime();

 private:
  v8::Isolate* isolate;
};

}  // namespace openhoi