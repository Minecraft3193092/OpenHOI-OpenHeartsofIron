// Copyright 2018-2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#if defined(_MSC_VER)
//  Microsoft Visual Studio
#  define OPENHOI_LIB_EXPORT __declspec(dllexport)
#  define OPENHOI_LIB_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#  define OPENHOI_LIB_EXPORT __attribute__((visibility("default")))
#  define OPENHOI_LIB_IMPORT
#else
//  do nothing and hope for the best?
#  define OPENHOI_LIB_EXPORT
#  define OPENHOI_LIB_IMPORT
#  pragma warning Unknown dynamic link import / export semantics.
#endif