// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/scripting/scripting_runtime.hpp"

#include <libplatform/libplatform.h>

namespace openhoi {

// Initializes the V8 scripting engine
ScriptingRuntime::ScriptingRuntime() {
  // Initialize V8
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  // Create internal isolate and context. We use these objects for JS
  // interactions outside the game
  v8::Isolate::CreateParams createParams;
  createParams.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  internalIsolate = v8::Isolate::New(createParams);
  v8::Isolate::Scope isolateScope(internalIsolate);
  v8::HandleScope handleScope(internalIsolate);
  internalContext = v8::Context::New(internalIsolate);
}

// Destroys the V8 scripting engine
ScriptingRuntime::~ScriptingRuntime() {
  // Disponse internal isolate
  if (internalIsolate) internalIsolate->Dispose();

  // Tear down V8
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();

  // TODO: delete createParams.array_buffer_allocator;
}

// Gets the internal V8 context
v8::Local<v8::Context> const& ScriptingRuntime::getInternalContext() const {
  return internalContext;
}

}  // namespace openhoi