// Copyright 2019 the openhoi authors. See COPYING.md for legal info.

#pragma once

#include "library.hpp"
#include "os.hpp"

#include <OgreLog.h>

#ifndef OPENHOI_OS_WINDOWS
#  include <syslog.h>
#endif

namespace openhoi {

// OGRE log listener
class LogListener final : public Ogre::LogListener {
 public:
  // Log listener constructor
  OPENHOI_LIB_EXPORT LogListener();

  // Log listener destructor
  ~LogListener();

  // This is called whenever the log receives a message and is about to write it
  // out
  OPENHOI_LIB_EXPORT void messageLogged(const Ogre::String& message,
                                        Ogre::LogMessageLevel lml,
                                        bool maskDebug,
                                        const Ogre::String& logName,
                                        bool& skipThisMessage);
};

}  // namespace openhoi