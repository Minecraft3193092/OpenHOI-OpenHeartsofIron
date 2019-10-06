# - Find V8
# Find the V8 includes and library
#
#  V8_INCLUDE_DIRS - Where to find V8 includes
#  V8_LIBRARIES   - List of libraries when using V8
#  V8_FOUND       - True if V8 was found

IF(V8_INCLUDE_DIRS)
  SET(V8_FIND_QUIETLY TRUE)
ENDIF()

FIND_PATH(V8_INCLUDE_DIR "v8.h"
  PATHS
  $ENV{V8_HOME}/include
  $ENV{EXTERNLIBS}/v8/include
  ~/Library/Frameworks/include
  /Library/Frameworks/include
  /usr/local/include
  /usr/local/include/v8
  /usr/local/v8/include
  /usr/local/v8/libexec/include
  /usr/include
  /sw/include # Fink
  /opt/local/include # DarwinPorts
  /opt/csw/include # Blastwave
  /opt/include
  DOC "V8 headers"
)

FIND_LIBRARY(V8_LIBRARY NAMES v8 v8.dll.lib
  PATHS
  $ENV{V8_HOME}
  $ENV{V8_HOME}/lib
  $ENV{EXTERNLIBS}/v8
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr/local/lib
  /usr/local/v8/lib
  /usr/local/v8/libexec
  /usr/local/v8/libexec/lib
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "V8 library"
)

FIND_LIBRARY(V8_BASE_LIBRARY NAMES v8_base v8_libbase v8_libbase.dll.lib
  PATHS
  $ENV{V8_HOME}
  $ENV{V8_HOME}/lib
  $ENV{EXTERNLIBS}/v8
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr/local/lib
  /usr/local/v8/lib
  /usr/local/v8/libexec
  /usr/local/v8/libexec/lib
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "V8 Base liibrary"
)

FIND_LIBRARY(V8_PLATFORM_LIBRARY NAMES v8_platform v8_libplatform v8_libplatform.dll.lib
  PATHS
  $ENV{V8_HOME}
  $ENV{V8_HOME}/lib
  $ENV{EXTERNLIBS}/v8
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr/local/lib
  /usr/local/v8/lib
  /usr/local/v8/libexec
  /usr/local/v8/libexec/lib
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "V8 Platform library"
)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(V8 DEFAULT_MSG V8_LIBRARY V8_BASE_LIBRARY V8_PLATFORM_LIBRARY)

MARK_AS_ADVANCED(V8_LIBRARY V8_BASE_LIBRARY V8_PLATFORM_LIBRARY V8_INCLUDE_DIR)

IF(V8_FOUND)
  SET(V8_LIBRARIES ${V8_LIBRARY} ${V8_BASE_LIBRARY} ${V8_PLATFORM_LIBRARY})
  SET(V8_INCLUDE_DIRS ${V8_INCLUDE_DIR})
ENDIF()