// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/helper/unique_id.hpp"
#include "hoibase/helper/os.hpp"

#include <array>
#if defined(OPENHOI_OS_WINDOWS)
#  include <rpc.h>
#  pragma comment(lib, "rpcrt4.lib")
#elif defined(OPENHOI_OS_APPLE)
#  include <CoreFoundation/CFUUID.h>
#else
#  include <uuid/uuid.h>
#endif

namespace openhoi {

// Generate a Universally Unique Identifier (UUID)
std::string generateUUID() {
  std::string uuidString;

#ifdef OPENHOI_OS_WINDOWS
  UUID uuid;
  ZeroMemory(&uuid, sizeof(UUID));
  UuidCreate(&uuid);
  WCHAR* wszUuid = NULL;
  UuidToStringW(&uuid, (RPC_WSTR*)&wszUuid);
  std::wstring uuidWString(wszUuid);
  uuidString = std::string(uuidWString.begin(), uuidWString.end());
  RpcStringFreeW((RPC_WSTR*)&wszUuid);
#elif OPENHOI_OS_APPLE
  CFUUIDRef uuid = CFUUIDCreate(NULL);
  NSString* uuidNsString = (NSString*)CFUUIDCreateString(NULL, uuid);
  uuidString = std::string([uuidNsString UTF8String]);
  CFRelease(uuid);
#else
  uuid_t uuid;
  uuid_generate_time_safe(uuid);
  char uuidChr[37];
  uuid_unparse_lower(uuid, uuidChr);
  uuidString = std::string(uuidChr);
#endif

  return uuidString;
}

}  // namespace openhoi