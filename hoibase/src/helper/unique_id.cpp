// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#include "hoibase/helper/unique_id.hpp"

#include <array>

#include "hoibase/helper/os.hpp"
#if defined(OPENHOI_OS_WINDOWS)
#  include <rpc.h>

#  include <codecvt>
#elif defined(OPENHOI_OS_MACOS)
#  include <CoreFoundation/CFUUID.h>
#else
#  if __has_include(<uuid/uuid.h>)
#    include <uuid/uuid.h>  // libuuid, e.g. Debian/Ubuntu
#  elif __has_include(<uuid/uuid.h>)
#    include <ossp/uuid.h>  // ossp-uuid, e.g. Fedora
#    define OSSP_UUID
#  elif __has_include(<uuid.h>)
#    include <uuid.h>
#    ifdef UUID_VERSION  // Can be either libuuid or ossp-uuid
#      define OSSP_UUID
#    endif
#  else
#    error Could not find external UUID library header
#  endif
#  ifdef OSSP_UUID
#    typedef uuid NIX_UUID_TYPE
#  else
#    typedef uuid_t NIX_UUID_TYPE
#  endif
#endif

namespace openhoi {

// Generate a Universally Unique Identifier (UUID)
std::string UniqueID::generate() {
#ifdef OPENHOI_OS_WINDOWS
  UUID uuid;
  ZeroMemory(&uuid, sizeof(UUID));

  RPC_STATUS result;
  result = UuidCreate(&uuid);
  if (result != RPC_S_OK && result != RPC_S_UUID_LOCAL_ONLY)
    throw "UUID could not be created";  // TODO: Proper error handling

  WCHAR* wszUuid = NULL;
  result = UuidToStringW(&uuid, (RPC_WSTR*)&wszUuid);
  if (result != RPC_S_OK)
    throw "UUID could not be converted to string";  // TODO: Proper error
                                                    // handling

  std::wstring uuidWString(wszUuid);
  int len = WideCharToMultiByte(CP_UTF8, 0, &uuidWString[0],
                                (int)uuidWString.size(), NULL, 0, NULL, NULL);
  std::string uuidString(len, 0);
  WideCharToMultiByte(CP_UTF8, 0, &uuidWString[0], (int)uuidWString.size(),
                      &uuidString[0], len, NULL, NULL);
  RpcStringFreeW((RPC_WSTR*)&wszUuid);
#elif OPENHOI_OS_MACOS
  CFUUIDRef uuid = CFUUIDCreate(NULL);
  NSString* uuidNsString = (NSString*)CFUUIDCreateString(NULL, uuid);
  std::string uuidString = std::string([uuidNsString UTF8String]);
  CFRelease(uuid);
#else
  NIX_UUID_TYPE uuid;

#  ifndef OSSP_UUID
  if (uuid_generate_time_safe(uuid) < 0)
    throw "UUID could not be created";  // TODO: Proper error handling

  char uuidChr[37];
  uuid_unparse_lower(uuid, uuidChr);
#  else
  char* uuidChr;

  uuid_create(&uuid);
  uuid_make(uuid, UUID_MAKE_V4);
  uuid_export(uuid, UUID_FMT_STR, &uuidChr, NULL);
  uuid_destroy(uuid);
#  endif

  std::string uuidString = std::string(uuidChr);
#endif

  return uuidString;
}

}  // namespace openhoi