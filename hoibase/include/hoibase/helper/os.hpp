// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "hoibase/helper/library.hpp"

#include <string>

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
#  define OPENHOI_OS_WINDOWS
#  ifdef _WIN64
#    define OPENHOI_OS_WINDOWS64
#  endif
#endif

////////////////////////////////////////////////////////////////////////////
//
//   DETECT LINUX
//
////////////////////////////////////////////////////////////////////////////
#ifdef __linux__
#  define OPENHOI_OS_LINUX
#endif

////////////////////////////////////////////////////////////////////////////
//
//   DETECT APPLE
//
////////////////////////////////////////////////////////////////////////////
#if defined(__APPLE__) && defined(__MACH__)
#  define OPENHOI_OS_APPLE
#  include <TargetConditionals.h>
#  if TARGET_IPHONE_SIMULATOR
#    define OPENHOI_OS_APPLE_IOS
#    define OPENHOI_OS_APPLE_IOS_SIMULATOR
#  elif TARGET_OS_IPHONE
#    define OPENHOI_OS_APPLE_IOS
#  elif TARGET_OS_MAC
#    define OPENHOI_OS_APPLE_MACOS
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
#    define OPENHOI_OS_BSD
#    ifdef __DragonFly__
#      define OPENHOI_OS_DRAGONFLY_BSD
#    elif defined(__FreeBSD__)
#      define OPENHOI_OS_FREEBSD
#    elif defined(__NetBSD__)
#      define OPENHOI_OS_NETBSD
#    elif defined(__OpenBSD__)
#      define OPENHOI_OS_OPENBSD
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
  // Execute command line program and return result
  static std::string executeCommand(std::string command);

  // Checks if we are currently running inside a virtual machine
  static bool isRunningInVirtualMachine();

 private:
#if defined(OPENHOI_OS_LINUX) || defined(OPENHOI_OS_BSD)
  // Check if system is running systemd
  static bool isRunningSystemd();
#endif
};

}  // namespace openhoi