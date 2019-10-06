// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <v8.h>

namespace openhoi {

// V8-powered JavaScript scripting runtime
class ScriptingRuntime final {
 public:
  static ScriptingRuntime& getInstance() {
    // Thread-safe C++11 singleton
    static ScriptingRuntime instance;
    return instance;
  }

  // Gets the internal V8 context
  OPENHOI_LIB_EXPORT v8::Local<v8::Context> const& getInternalContext() const;

 protected:
  // Initializes the V8 scripting engine
  OPENHOI_LIB_EXPORT ScriptingRuntime();

  // Destroys the V8 scripting engine
  OPENHOI_LIB_EXPORT ~ScriptingRuntime();

 private:
  v8::Isolate* internalIsolate;
  v8::Local<v8::Context> internalContext;
};

}  // namespace openhoi