// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

////////////////////////////////////////////////////////////////////////////
//
//   DETECT WINDOWS
//
////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  define OPENHOI_OS_WINDOWS 1
#  ifdef _WIN64
#    define OPENHOI_OS_WINDOWS64 1
#  endif
#endif

////////////////////////////////////////////////////////////////////////////
//
//   DETECT LINUX
//
////////////////////////////////////////////////////////////////////////////
#ifdef __linux__
#  define OPENHOI_OS_LINUX 1
#endif

////////////////////////////////////////////////////////////////////////////
//
//   DETECT APPLE
//
////////////////////////////////////////////////////////////////////////////
#if defined(__APPLE__) && defined(__MACH__)
#  define OPENHOI_OS_APPLE 1
#  include <TargetConditionals.h>
#  if TARGET_IPHONE_SIMULATOR
#    define OPENHOI_OS_APPLE_IOS 1
#    define OPENHOI_OS_APPLE_IOS_SIMULATOR 1
#  elif TARGET_OS_IPHONE
#    define OPENHOI_OS_APPLE_IOS 1
#  elif TARGET_OS_MAC
#    define OPENHOI_OS_APPLE_MACOS 1
#  endif
#endif

////////////////////////////////////////////////////////////////////////////
//
//   DETECT BSD
//
////////////////////////////////////////////////////////////////////////////
#if !defined(OPENHOI_OS_LINUX) && \
    (defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)))
#  include <sys/param.h>
#  if defined(BSD)
#    define OPENHOI_OS_BSD 1
#    ifdef __DragonFly__
#      define OPENHOI_OS_DRAGONFLY_BSD 1
#    elif defined(__FreeBSD__)
#      define OPENHOI_OS_FREEBSD 1
#    elif defined(__NetBSD__)
#      define OPENHOI_OS_NETBSD 1
#    elif defined(__OpenBSD__)
#      define OPENHOI_OS_OPENBSD 1
#    endif
#  endif
#endif

////////////////////////////////////////////////////////////////////////////
//
//   COMPATIBILITY LOGIC
//
////////////////////////////////////////////////////////////////////////////
#ifndef byte
typedef unsigned char byte;
#endif

namespace openhoi {

class OPENHOI_LIB_EXPORT OS final {
 public:
  // Checks if we are currently running inside a virtual machine
  static bool isRunningInVirtualMachine();
};

}  // namespace openhoi