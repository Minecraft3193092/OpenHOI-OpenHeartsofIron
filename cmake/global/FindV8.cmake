# - Find V8
# Find the V8 includes and library
#
#  V8_INCLUDE_DIR - Where to find V8 includes
#  V8_LIBRARIES   - List of libraries when using V8
#  V8_FOUND       - True if V8 was found

IF(V8_INCLUDE_DIR)
  SET(V8_FIND_QUIETLY TRUE)
ENDIF(V8_INCLUDE_DIR)

FIND_PATH(V8_INCLUDE_DIR "v8.h"
  PATHS
  $ENV{V8_HOME}/include
  $ENV{EXTERNLIBS}/v8/include
  ~/Library/Frameworks/include
  /Library/Frameworks/include
  /usr/local/include
  /usr/include
  /sw/include # Fink
  /opt/local/include # DarwinPorts
  /opt/csw/include # Blastwave
  /opt/include
  DOC "V8 - Headers"
)

SET(V8_NAMES v8 v8_base v8_base.lib v8_libbase.dll.lib)
SET(V8S_NAMES v8_snapshot v8_snapshot.lib)
SET(V8_DBG_NAMES v8D v8_baseD v8_baseD.lib)
SET(V8S_DBG_NAMES v8_snapshotD v8_snapshotD.lib)

FIND_LIBRARY(V8_LIBRARY NAMES ${V8_NAMES}
  PATHS
  $ENV{V8_HOME}
  $ENV{V8_HOME}/lib
  $ENV{EXTERNLIBS}/v8
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "V8 - Library"
)

FIND_LIBRARY(V8S_LIBRARY NAMES ${V8S_NAMES}
  PATHS
  $ENV{V8_HOME}
  $ENV{V8_HOME}/lib
  $ENV{EXTERNLIBS}/v8
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
  PATH_SUFFIXES lib lib64
  DOC "V8S - Library"
)

INCLUDE(FindPackageHandleStandardArgs)

SET(V8_LIBRARIES ${V8_LIBRARY})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(V8 DEFAULT_MSG V8_LIBRARY V8_INCLUDE_DIR)

MARK_AS_ADVANCED(V8_LIBRARY V8_INCLUDE_DIR)


IF(V8_FOUND)
  SET(V8_INCLUDE_DIRS ${V8_INCLUDE_DIR})
ENDIF(V8_FOUND)